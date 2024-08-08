#pragma once
#include "GameObject.h"

class SquareObject : public GameObject {
public:
    SquareObject(const std::string& name, int drawPriority, float x, float y);
    virtual void render(Renderer& renderer) override;
    virtual void update() override;
protected:
    float r = 0;
    float g = 0;
    float b = 0;
};
