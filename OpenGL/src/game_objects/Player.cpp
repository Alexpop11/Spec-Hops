#include <cmath>

#include "Player.h"
#include "../Input.h"
#include "Camera.h"
#include "../World.h"
#include "Tile.h"
#include "Bomb.h"

Player::Player(const std::string& name, int x, int y)
   : Character(name, x, y) {
   drawPriority = 3;
   r            = 0.5f;
   g            = 0.8f;
   b            = 0.5f;
   health       = 3;
   Camera::x    = x;
   Camera::y    = y;
}

void Player::move(int new_x, int new_y) {
   Character::move(new_x, new_y);
   Renderer::audioEngine.Walk.play();
}

void Player::update() {
   Character::update();
   // smooth camera movement
   Camera::x = zeno(Camera::x, x, 1);
   Camera::y = zeno(Camera::y, y, 1);

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

   if (!key_pressed_last_frame && key_pressed_this_frame) {
      World::shouldTick = true;
   }
   key_pressed_last_frame = key_pressed_this_frame;
}

void Player::tickUpdate() {
   if (moved_last_tick) {
      moved_last_tick = false;
      return;
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
         bombCoolDown = 3;
      }
   }

   if (new_x != tile_x || new_y != tile_y) {
      move(new_x, new_y);
   }

   if (health == 1) {
      if (std::fmod(glfwGetTime(), 0.3) < 0.15) {
         r = 0.5f;
         g = 0.8f;
         b = 0.5f;
      } else {
         r = 0.75f;
         g = 0.75f;
         b = 0.0f;
      }
   }

   if (health <= 0) {
      die();
   }
}
