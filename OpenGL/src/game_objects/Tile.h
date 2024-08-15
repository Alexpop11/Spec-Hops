#pragma once
#include "SquareObject.h"

class Tile: public SquareObject {
public:
    Tile() = default;
    Tile(const std::string& name, bool wall, float x, float y);
    Tile(const std::string& name, float x, float y);
    virtual void update() override;
    virtual void explode();
    bool wall;

};

