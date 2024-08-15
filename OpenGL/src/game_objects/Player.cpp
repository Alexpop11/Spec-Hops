#include "Player.h"
#include "../Input.h"
#include "Camera.h"
#include "../World.h"
#include "Tile.h"
#include "Bomb.h"

Player::Player(const std::string& name, float x, float y)
    : Character(name, x, y) 
    {
    drawPriority = 3;
    r = 0.5;
    g = 0.8;
    b = 0.5;
}

void Player::update() {
    float new_x = x;
    float new_y = y;
    bool new_spot_occupied = false;
    if (Input::keys_pressed_down[GLFW_KEY_W] || Input::keys_pressed_down[GLFW_KEY_UP])
    {
        new_y += 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_A] || Input::keys_pressed_down[GLFW_KEY_LEFT])
    {
        new_x -= 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_S] || Input::keys_pressed_down[GLFW_KEY_DOWN])
    {
        new_y -= 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_D] || Input::keys_pressed_down[GLFW_KEY_RIGHT])
    {
        new_x += 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_SPACE])
    {
        World::gameobjectstoadd.push_back(std::make_unique<Bomb>(Bomb("CoolBomb", x, y)));
    }
    for (auto& gameobject : World::gameobjects) {
        auto tile = dynamic_cast<Tile*>(&*gameobject);
        if (tile != nullptr) {
            if (new_x == tile->x && new_y == tile->y && tile->wall) {
                new_spot_occupied = true;
            }
        }
    };

    if (!new_spot_occupied) {
        x = new_x;
        y = new_y;
        Camera::x = x;
        Camera::y = y;
    }
}
