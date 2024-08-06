#pragma once

#include <string>

class GameObject {
public:
    GameObject(const std::string& name, int drawPriority, float x, float y);

    void render();

    std::string name;
    int drawPriority;
    float x;
    float y;

private:
    // Add any private members here if needed
};