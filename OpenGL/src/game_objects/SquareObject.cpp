#include "SquareObject.h"

SquareObject::SquareObject(const std::string& name, int drawPriority, int x, int y)
    : GameObject(name, drawPriority, x, y) {
    r = 0.5;
    g = 0.5;
    b = 0.5;
    tile_x = x;
    tile_y = y;
    shader = std::make_shared<Shader>(Renderer::ResPath() + "shaders/shader.shader");
}

void SquareObject::setUpShader(Renderer& renderer)
{
    GameObject::setUpShader(renderer);
    shader->SetUniform4f("u_Color", r, g, b, 1.0f);
}

void SquareObject::render(Renderer& renderer) {

    GameObject::render(renderer);

    float positions[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f, 0.5f,
         -0.5f, 0.5f,
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

    renderer.Draw(va, ib, *shader);

}

void SquareObject::update() {
    // Update logic for SquareObject
}
