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

   moved_last_tick = true;
   int new_x       = tile_x;
   int new_y       = tile_y;
   // bool  new_spot_occupied = false;
   if (Input::keys_pressed[GLFW_KEY_W] || Input::keys_pressed[GLFW_KEY_UP]) {
      new_y += 1;
   }
   if (Input::keys_pressed[GLFW_KEY_A] || Input::keys_pressed[GLFW_KEY_LEFT]) {
      new_x -= 1;
   }
   if (Input::keys_pressed[GLFW_KEY_S] || Input::keys_pressed[GLFW_KEY_DOWN]) {
      new_y -= 1;
   }
   if (Input::keys_pressed[GLFW_KEY_D] || Input::keys_pressed[GLFW_KEY_RIGHT]) {
      new_x += 1;
   }
   if (Input::keys_pressed[GLFW_KEY_SPACE]) {
      World::gameobjectstoadd.push_back(std::make_unique<Bomb>(Bomb("CoolBomb", tile_x, tile_y)));
   }

   if (!key_pressed_last_frame) {
       
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

   move(new_x, new_y);
}
