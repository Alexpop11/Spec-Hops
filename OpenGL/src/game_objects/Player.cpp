#include <cmath>

#include "Player.h"
#include "../Input.h"
#include "Camera.h"
#include "../World.h"
#include "Tile.h"
#include "Bomb.h"

Player::Player(const std::string& name, int tile_x, int tile_y)
   : Character(name, tile_x, tile_y, "Textures/alternate-player.png") {
   drawPriority     = DrawPriority::Character;
   health           = 3;
   Camera::position = {tile_x, tile_y};

   healthText = std::make_unique<Text>("Health", Renderer::jacquard12_big, glm::vec2{20, 20});
   topText = std::make_unique<Text>("Hello!", Renderer::Pixelify, glm::vec2{1280,650});
}

bool Player::move(int new_x, int new_y) {
   if (Character::move(new_x, new_y)) {
      if (!second_step) {
         audio().Walk.play();
         second_step = true;
      } else if (second_step) {
         audio().Walk1.play();
         second_step = false;
      }
      return true;
   }
   return false;
}

void Player::update() {
   Character::update();
   // smooth camera movement
   Camera::position = zeno(Camera::position, position, 0.1);
   tintColor.a      = zeno(tintColor.a, 0.0, 0.1);

   bool key_pressed_this_frame = false;

   if (Input::keys_pressed[GLFW_KEY_W] || Input::keys_pressed[GLFW_KEY_UP]) {
      key_pressed_this_frame = true;
   }
   if (Input::keys_pressed[GLFW_KEY_A] || Input::keys_pressed[GLFW_KEY_LEFT]) {
      key_pressed_this_frame = true;
   }
   if (Input::keys_pressed[GLFW_KEY_S] || Input::keys_pressed[GLFW_KEY_DOWN]) {
      key_pressed_this_frame = true;
   }
   if (Input::keys_pressed[GLFW_KEY_D] || Input::keys_pressed[GLFW_KEY_RIGHT]) {
      key_pressed_this_frame = true;
   }
   if (Input::keys_pressed[GLFW_KEY_SPACE]) {
      key_pressed_this_frame = true;
   }
   if (Input::keys_pressed[GLFW_KEY_O]) {
      health = 100000;
   }

   if (!key_pressed_last_frame && key_pressed_this_frame || Input::keys_pressed_down[GLFW_KEY_LEFT_SHIFT]) {
      World::shouldTick = true;
   }
   key_pressed_last_frame = key_pressed_this_frame;

   healthText->name = "Health: " + std::to_string(health);

   // When user clicks the mouse, 

}

void Player::render(Renderer& renderer) {
   Character::render(renderer);
   if (Input::mouse_pressed_down) {
      if (gunCooldown == 0) {
         Renderer::DebugLine(position, renderer.MousePos(), {1, 0, 0, 1});
         audio().Zap.play();
         // get what is at mouse position
         for (auto& character : World::at<Character>(renderer.MousePos().x + 0.5, renderer.MousePos().y + 0.5)) {
            character->stunnedLength = 5;
            character->tintColor     = {1.0, 0.5, 0.0, 0.5};
         }
         for (auto& bomb : World::at<Bomb>(renderer.MousePos().x + 0.5, renderer.MousePos().y + 0.5)) {
            bomb->explode();
         }
            gunCooldown = playerGunCooldown;
      }
   }
}

void Player::hurt() {
   if (health > 0) {
      health--;
      tintColor = {1.0, 0.0, 0.0, 0.5};
      audio().Hurt_Sound.play();
   }
   if (health == 0) {
      audio().Death_Sound.play();
      die();
   }
}

void Player::tickUpdate() {
   if (moved_last_tick) {
      moved_last_tick = false;
      //return;
   }
   // This logic is already in character, but it's not applying to the player for some reason - so for now I'm just
   // going to copy it here. pls help Powerup cooldowns
   if (bombCoolDown > 0) {
      bombCoolDown--;
   }
   if (bunnyHopCoolDown > 0) {
      if (bunnyHopCoolDown < playerBunnyHopCoolDown) {
         hoppedLastTurn = false;
      }
      bunnyHopCoolDown--;
   }

   if (gunCooldown > 0) {
      gunCooldown--;
   }

   int boostJumpCount =
      0; // For bunny hop mechanic. Resets to 0 here, and if the player is holding shift it increases temporarily.

   moved_last_tick = true;
   int new_x       = tile_x;
   int new_y       = tile_y;

   if (Input::keys_pressed[GLFW_KEY_LEFT_SHIFT] || Input::keys_pressed[GLFW_KEY_RIGHT_SHIFT]) {
      if (hasBunnyHop && bunnyHopCoolDown <= 0) {
         boostJumpCount = 2;
         hoppedLastTurn = true;
      }
   }

   // bool  new_spot_occupied = false;
   if (Input::keys_pressed[GLFW_KEY_W] || Input::keys_pressed[GLFW_KEY_UP]) {
      new_y += 1 + boostJumpCount;
      if (boostJumpCount > 0) {
         bunnyHopCoolDown = playerBunnyHopCoolDown;
      }
   }
   if (Input::keys_pressed[GLFW_KEY_A] || Input::keys_pressed[GLFW_KEY_LEFT]) {
      new_x -= 1 + boostJumpCount;
      if (boostJumpCount > 0) {
         bunnyHopCoolDown = playerBunnyHopCoolDown;
      }
   }
   if (Input::keys_pressed[GLFW_KEY_S] || Input::keys_pressed[GLFW_KEY_DOWN]) {
      new_y -= 1 + boostJumpCount;
      if (boostJumpCount > 0) {
         bunnyHopCoolDown = playerBunnyHopCoolDown;
      }
   }
   if (Input::keys_pressed[GLFW_KEY_D] || Input::keys_pressed[GLFW_KEY_RIGHT]) {
      new_x += 1 + boostJumpCount;
      if (boostJumpCount > 0) {
         bunnyHopCoolDown = playerBunnyHopCoolDown;
      }
   }
   if (Input::keys_pressed[GLFW_KEY_SPACE]) {
      if (hasBomb && bombCoolDown <= 0) {
         World::gameobjectstoadd.push_back(std::make_unique<Bomb>(Bomb("CoolBomb", tile_x, tile_y)));
         audio().Bomb_Place.play();
         bombCoolDown = 3;
      }
   }

   if (new_x != tile_x || new_y != tile_y) {
      move(new_x, new_y);
   }

   if (health == 1) {
      if (std::fmod(glfwGetTime(), 0.3) < 0.15) {
         tintColor = {0.5, 0.8, 0.5, 0.5};
      } else {
         tintColor = {0.75, 0.75, 0.0, 0.5};
      }
   }
}
