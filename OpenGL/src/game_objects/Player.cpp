#include "Player.h"
#include "../Input.h"

Player::Player(const std::string& name, float x, float y)
    : Character(name, x, y) {}

void Player::update() {
    if (Input::keys_pressed_down[GLFW_KEY_W])
    {
        y += 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_A])
    {
        x -= 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_S])
    {
        y -= 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_D])
    {
        x += 1;
    }
}
