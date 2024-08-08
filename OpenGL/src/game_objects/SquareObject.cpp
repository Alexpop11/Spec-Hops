#include "SquareObject.h"

SquareObject::SquareObject(const std::string& name, int drawPriority, float x, float y)
    : GameObject(name, drawPriority, x, y) {}

void SquareObject::render(Renderer& renderer) {
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

    Shader shader("res/shaders/shader.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    int width, height;
    glfwGetWindowSize(renderer.window, &width, &height); // Get the current window size
    glViewport(0, 0, width, height);

    /* Render here */
    float currentTime = glfwGetTime(); // Or any other method to get the elapsed time

    shader.Bind(); //mby remove
    shader.SetUniform4f("u_Color", 1, 0.3f, 0.8f, 1.0f);
    shader.SetUniform1f("u_AspectRatio", float(width) / float(height));
    shader.SetUniform2f("u_Position", x, y);

    renderer.Draw(va, ib, shader);

}

void SquareObject::update() {
    // Update logic for SquareObject
}
