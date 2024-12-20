#include "Character.h"
#include "Input.h"
#include <cmath>
#include "../rendering/Renderer.h"
#include "Decal.h"

Character::Character(const std::string& name, int tile_x, int tile_y, std::string texturepath)
   : Entity(name, DrawPriority::Character, tile_x, tile_y, texturepath) {}

void Character::update() {
   SquareObject::update();
}

void Character::tickUpdate() {
   // Powerup cooldowns
   if (bombCoolDown > 0) {
      bombCoolDown--;
   }
   if (bunnyHopCoolDown > 0) {
      bunnyHopCoolDown--;
   }
}

bool Character::move(int new_x, int new_y) {
   auto tile = getTile();
   if (stunnedLength == 0) {
      int dx    = new_x - tile.x;
      int dy    = new_y - tile.y;
      int steps = std::max(std::abs(dx), std::abs(dy));

      for (int i = 1; i <= steps; ++i) {
         int check_x = tile.x + (dx * i) / steps;
         int check_y = tile.y + (dy * i) / steps;

         bool                    spot_occupied   = false;
         std::shared_ptr<Entity> other_character = nullptr;

         // Check for walls
         for (auto& tile : World::at<Tile>(check_x, check_y)) {
            if (tile->wall) {
               spot_occupied = true;
               break;
            }
         }

         if (!spot_occupied) {
            // Check for other characters
            for (auto& entity : World::at<Entity>(check_x, check_y)) {
               if (entity.get() != this) {
                  spot_occupied   = true;
                  other_character = entity;
                  break;
               }
            }
         }

         if (spot_occupied) {
            if (other_character && steps > 1) {
               // Bunny-hopping into an enemy: perform kick
               int knockback_dx = (dx != 0) ? dx / std::abs(dx) : 0;
               int knockback_dy = (dy != 0) ? dy / std::abs(dy) : 0;

               bool      can_knockback          = true;
               const int max_knockback_distance = 3;
               int       knockback_distance     = max_knockback_distance;

               // Check if enemy can be knocked back 3 tiles
               for (int k = 1; k <= knockback_distance; ++k) {
                  int new_enemy_x = other_character->getTile().x + knockback_dx * k;
                  int new_enemy_y = other_character->getTile().y + knockback_dy * k;

                  // Check for obstacles
                  bool obstacle = false;
                  for (auto& tile : World::at<Tile>(new_enemy_x, new_enemy_y)) {
                     if (tile->wall) {
                        obstacle = true;
                        break;
                     }
                  }
                  if (!obstacle) {
                     for (auto& character : World::at<Character>(new_enemy_x, new_enemy_y)) {
                        if (character != other_character && character.get() != this) {
                           obstacle = true;
                           break;
                        }
                     }
                  }
                  if (obstacle) {
                     knockback_distance = k - 1;
                     break;
                  }
               }

               // Move enemy back as far as possible
               if (knockback_distance > 0) {
                  KickState kicking;
                  kicking.victim    = other_character;
                  kicking.direction = glm::ivec2(knockback_dx * knockback_distance, knockback_dy * knockback_distance);
                  kicking.intoWall  = knockback_distance < max_knockback_distance;
                  this->kicking     = kicking;

                  // Stop the player at the collision spot
                  setTile({check_x, check_y});
                  return true; // Move succeeded with kick
               } else {
                  // Enemy can't be moved; treat as normal collision
                  spot_occupied = true;
               }
            }

            // Handle normal collision (walking or unable to kick enemy)
            if (i <= 1 && !hoppedLastTurn) {
               bunnyHopCoolDown = 0;
            }
            if (i > 1) {
               setTile({tile.x + (dx * (i - 1)) / steps, tile.y + (dy * (i - 1)) / steps});
            }
            return false;
         }
      }

      // Path is clear; move the character
      setTile({new_x, new_y});
      return true;
   } else if (stunnedLength > 0) {
      stunnedLength--;
   }
   return false;
}

void Character::kick(bool hitWall, int dx, int dy) {
   Entity::kick(hitWall, dx, dy);
   World::gameobjectstoadd.push_back(std::make_unique<Decal>("impactDecal", getTile().x, getTile().y, "impact"));
   stunnedLength = 9;
   tintColor     = {1.0, 0.5, 0.0, 0.5};
}


void Character::die() {
   tintColor.r = 0.4f;
   tintColor.g = 0.3f;
   tintColor.b = 0.3f;
   tintColor.a = 0.5f;
}

void Character::hurt() {
   if (health > 0) {
      health--;
      tintColor = {1.0, 0.0, 0.0, 0.5};
   }
}
