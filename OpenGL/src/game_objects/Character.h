#pragma once
#include "SquareObject.h"
#include "../World.h"
#include "Tile.h"


class Character : public SquareObject {
public:
    Character() = default;
    Character(const std::string& name, float x, float y);
    virtual void move(float new_x, float new_y);
    virtual void update() override;
    void die();
    int health;
};

template <typename T>
int sign(T val) {
    return (T(0) < val) - (val < T(0));
}