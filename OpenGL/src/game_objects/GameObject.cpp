

#include "GameObject.h"
#include "../Input.h"


GameObject::GameObject(const std::string& name, int drawPriority, float x, float y)
    : name(name), drawPriority(drawPriority), x(x), y(y) {
    // TODO: Add any additional initialization if needed
}

void GameObject::update() 
{
    
}

void GameObject::setUpShader(Renderer& renderer)
{
    if (shader.has_value()) {
        auto [width, height] = renderer.WindowSize();

        /* Render here */
        shader->Bind();
        float currentTime = glfwGetTime();
        shader->SetUniform1f("u_Time", currentTime);
        shader->SetUniform1f("u_StartTime", Input::startTime);
        shader->SetUniform1f("u_AspectRatio", float(width) / float(height));
        shader->SetUniform2f("u_Position", x, y);
        shader->SetUniform2f("u_Resolution", width, height);

    }
}

void GameObject::render(Renderer& renderer) {
    setUpShader(renderer);
}
