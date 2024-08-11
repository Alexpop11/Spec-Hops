#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <AL/al.h>
#include <AL/alc.h>

#define STB_IMAGE_IMPLEMENTATION // for icon
#include "stb_image.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "game_objects/GameObject.h"
#include "Input.h"
#include "game_objects/Player.h"

#define ASSERT(x) if (!(x)) __debugbreak(); // Remove this line if you are not using visual studio
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while(GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file << " " << std::endl;
        return false;
    }
    return true;
}

void setWindowIcon(GLFWwindow* window, const char* iconPath) {
    int width, height, channels;
    unsigned char* pixels = stbi_load(iconPath, &width, &height, &channels, 4);
    if (pixels) {
        GLFWimage images[1];
        images[0].width = width;
        images[0].height = height;
        images[0].pixels = pixels;
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(pixels);
    }
    else {
        std::cerr << "Failed to load icon: " << iconPath << std::endl;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        std::cout << "Escape key was pressed" << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void sortGameObjectsByPriority(std::vector<std::unique_ptr<GameObject>>& gameObjects) {
    std::sort(gameObjects.begin(), gameObjects.end(),
        [](const std::unique_ptr<GameObject>& a, const std::unique_ptr<GameObject>& b) {
            return a->drawPriority < b->drawPriority;
        });
}

std::vector<std::string> mapLoader(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    file.close();
    return lines;
}

int main(void)
{

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    // Get the primary monitor
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // Create a fullscreen window
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "SpaceBoom", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    // Set the window icon
    setWindowIcon(window, "res/Images/Logo.png");

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << "current version of GL: " << glGetString(GL_VERSION) << std::endl;

    Renderer renderer;
    renderer.window = window;

    float r = 0.0f;
    float increment = 0.05f;
    
    //std::string MapToUse = "../res/maps/SpaceView.txt";
    //std::vector<std::string> lines = mapLoader(MapToUse);
    std::vector<std::unique_ptr<GameObject>> gameobjects;
#include "../res/maps/SpaceView.txt"
    //for (const auto& line : lines) {
    //    std::cout << line << std::endl;
    //}

    Input::startTime = glfwGetTime();

    // LOOP
    while (!glfwWindowShouldClose(window))
    {
        sortGameObjectsByPriority(gameobjects);
        renderer.Clear();
        Input::updateKeyStates(window);

        for (auto &gameobject : gameobjects) 
        {   
            gameobject->update();
        }

        for (auto& gameobject : gameobjects)
        {
            gameobject->render(renderer);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    //cleanupOpenAL();
    glfwTerminate();
    return 0;
}