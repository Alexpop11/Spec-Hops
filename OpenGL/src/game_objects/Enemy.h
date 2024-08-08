#pragma once
#include "Character.h"

class Enemy : public Character {
public:
    Enemy(const std::string& name, float x, float y);
    virtual void render(Renderer& renderer) override;
    virtual void update() override;
};
