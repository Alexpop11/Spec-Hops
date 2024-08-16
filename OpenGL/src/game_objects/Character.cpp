#include "Character.h"

Character::Character(const std::string& name, float x, float y)
    : SquareObject(name, 1, x, y) {}

void Character::update() {
    // Update logic for Character
}

void Character::die() {
    r = 0.4;
    g = 0.3;
    b = 0.3;
}
