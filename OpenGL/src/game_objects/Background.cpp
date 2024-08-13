#include "Background.h"

Background::Background(const std::string& name, int drawPriority, float x, float y)
    : GameObject(name, drawPriority, x, y) {
    shader = std::move(std::make_optional<Shader>("res/shaders/stars.shader"));
    drawPriority = 0;
}

void Background::setUpShader(Renderer& renderer)
{
    GameObject::setUpShader(renderer);
}

void Background::render(Renderer& renderer) {

    GameObject::render(renderer);

    float positions[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f, 1.0f,
         -1.0f, 1.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    VertexArray va;
    VertexBuffer vb(positions, 4 * 2 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);


    IndexBuffer ib(indices, 6);

    renderer.Draw(va, ib, shader.value());

}

void Background::update() {
    // Update logic for Background
}
