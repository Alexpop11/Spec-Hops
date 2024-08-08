#pragma once
#include "Character.h"

class Player : public Character {
public:
    Player(const std::string& name, float x, float y);
    virtual void update() override;
};

