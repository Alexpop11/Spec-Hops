

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
        int width, height;
        glfwGetWindowSize(renderer.window, &width, &height); // Get the current window size
        glViewport(0, 0, width, height);

        /* Render here */
        shader->Bind();
        float currentTime = glfwGetTime();
        shader->SetUniform1f("u_Time", currentTime);
        shader->SetUniform1f("u_StartTime", Input::startTime);
        shader->SetUniform1f("u_AspectRatio", float(width) / float(height));
        shader->SetUniform2f("u_Position", x, y);
    }
}

void GameObject::render(Renderer& renderer) {
    setUpShader(renderer);
}