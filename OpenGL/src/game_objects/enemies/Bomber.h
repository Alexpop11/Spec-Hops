#pragma once
#include "../Character.h"
#include "../Player.h"
#include "../../World.h"
#include "../Bomb.h"

class Bomber : public Character {
public:
    Bomber(const std::string& name, float x, float y);
    virtual void update() override;
    virtual void move(float new_x, float new_y) override;
};
