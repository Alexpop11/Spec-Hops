

#include "GameObject.h"
#include "Input.h"


GameObject::GameObject(const std::string& name, int drawPriority, float x, float y)
    : name(name), drawPriority(drawPriority), x(x), y(y) {
    // TODO: Add any additional initialization if needed
}

void GameObject::update() 
{
    if (Input::keys_pressed_down[GLFW_KEY_W])
    {
        y += 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_A])
    {
        x -= 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_S])
    {
        y -= 1;
    }
    if (Input::keys_pressed_down[GLFW_KEY_D])
    {
        x += 1;
    }
}

void GameObject::render(Renderer& renderer) {
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

    // This function should handle drawing the GameObject
}