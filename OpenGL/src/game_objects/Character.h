#pragma once
#include "SquareObject.h"

class Character : public SquareObject {
public:
    Character() = default;
    Character(const std::string& name, float x, float y);
    virtual void update() override;
};
