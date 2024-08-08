#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

class GameObject {
public:
    GameObject(const std::string& name, int drawPriority, float x, float y);

    void render(Renderer& renderer);
    void update();

    std::string name;
    int drawPriority;
    float x;
    float y;

private:
    // Add any private members here if needed
};