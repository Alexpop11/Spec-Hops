#pragma once
#include "GameObject.h"

class Background : public GameObject {
public:
    Background(const std::string& name, int drawPriority, float x, float y);
    virtual void render(Renderer& renderer) override;
    virtual void update() override;
    virtual void setUpShader(Renderer& renderer) override;
};
