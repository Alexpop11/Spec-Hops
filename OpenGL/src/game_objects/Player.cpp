#include "Player.h"
#include "../Input.h"
#include "Camera.h"

Player::Player(const std::string& name, float x, float y)
    : Character(name, x, y) 
    {
    drawPriority = 3;
    r = 0.5;
    g = 0.8;
    b = 0.5;
}

void Player::update() {
    if (Input::keys_pressed_down[GLFW_KEY_W] || Input::keys_pressed_down[GLFW_KEY_UP])
    {
        y += 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_A] || Input::keys_pressed_down[GLFW_KEY_LEFT])
    {
        x -= 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_S] || Input::keys_pressed_down[GLFW_KEY_DOWN])
    {
        y -= 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_D] || Input::keys_pressed_down[GLFW_KEY_RIGHT])
    {
        x += 1;
    }
    Camera::x = x;
    Camera::y = y;
}
