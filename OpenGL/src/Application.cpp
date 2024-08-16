#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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
#include "World.h"
#include "game_objects/Player.h"
#include "game_objects/Background.h"
#include "game_objects/Camera.h"
#include "game_objects/Tile.h"
#include "game_objects/enemies/Bomber.h"

#define GLCall(x)  \
   GLClearError(); \
   x;              \
   ASSERT(GLLogCall(#x, __FILE__, __LINE__));
#if 0
void GLClearError)
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while(GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file << " " << std::endl;
        return false;
    }
    return true;
}
#endif
void setWindowIcon(GLFWwindow* window, const char* iconPath) {
   int            width, height, channels;
   unsigned char* pixels = stbi_load(iconPath, &width, &height, &channels, 4);
   if (pixels) {
      GLFWimage images[1];
      images[0].width  = width;
      images[0].height = height;
      images[0].pixels = pixels;
      glfwSetWindowIcon(window, 1, images);
      stbi_image_free(pixels);
   } else {
      std::cerr << "Failed to load icon: " << iconPath << std::endl;
   }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
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
   std::ifstream            file(filename);

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

void createMap() {
    std::string filename = "res/maps/SpaceShip.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    std::vector<std::string> lines;
    std::string line;

    // Read all lines into a vector
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    int total_rows = lines.size();

    for (int row = 0; row < total_rows; ++row) {
        for (unsigned long x = 0; x < lines[row].length(); ++x) {
            char c = lines[row][x];
            int y = total_rows - row;
            if (c != '\n') {
                if (c == 'b') { // Background
                    World::gameobjects.push_back(std::make_unique<Background>(Background("Background", 0, 0, 0)));
                }
                if (c == 'p') { // player
                    World::gameobjects.push_back(std::make_unique<Player>(Player("Coolbox", x, y)));
                    World::gameobjects.push_back(std::make_unique<Tile>(Tile("Floor", x, y)));
                }
                if (c == 'f') { // floor
                    World::gameobjects.push_back(std::make_unique<Tile>(Tile("Floor", x, y)));
                }
                if (c == 'w') { // wall
                    World::gameobjects.push_back(std::make_unique<Tile>(Tile("Wall", true, x, y)));
                }
                if (c == 'e') { // enemy Bomber 
                    World::gameobjects.push_back(std::make_unique<Bomber>(Bomber("bomber", x, y)));
                    World::gameobjects.push_back(std::make_unique<Tile>(Tile("Floor", x, y)));
                }
            }

        }
    }

    file.close();
}

int main(void) {

   /* Initialize the library */
   if (!glfwInit())
      return -1;

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



   // Get the primary monitor
   GLFWmonitor*       primaryMonitor = glfwGetPrimaryMonitor();
   const GLFWvidmode* mode           = glfwGetVideoMode(primaryMonitor);

   // Create a fullscreen window
   GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "SpaceBoom", NULL, NULL);
   if (!window) {
      std::cerr << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }

   glfwSetKeyCallback(window, key_callback);

   // Set the window icon
   setWindowIcon(window, "res/Images/Logo2.png");

   /* Make the window's context current */
   glfwMakeContextCurrent(window);

   glfwSwapInterval(1);

   if (glewInit() != GLEW_OK)
      std::cout << "Error!" << std::endl;

   std::cout << "current version of GL: " << glGetString(GL_VERSION) << std::endl;

   Renderer renderer;
   renderer.window = window;

   float r         = 0.0f;
   float increment = 0.05f;

   createMap();

   Input::startTime = glfwGetTime();

   // LOOP
   while (!glfwWindowShouldClose(window)) {
      // set the viewport size
      auto [width, height] = renderer.WindowSize();
      glViewport(0, 0, width, height);


      sortGameObjectsByPriority(World::gameobjects);
      renderer.Clear();
      Input::updateKeyStates(window);

      for (auto& gameobject : World::gameobjects) {
         gameobject->update();
      }

      World::gameobjects.erase(std::remove_if(World::gameobjects.begin(), World::gameobjects.end(),
          [](const std::unique_ptr<GameObject>& gameobject) { return gameobject->ShouldDestroy; }),
          World::gameobjects.end());


      while (!World::gameobjectstoadd.empty()) {
          World::gameobjects.push_back(std::move(World::gameobjectstoadd.back()));
          World::gameobjectstoadd.pop_back();
      }

      for (auto& gameobject : World::gameobjects) {
         gameobject->render(renderer);
      }

      /* Swap front and back buffers */
      glfwSwapBuffers(window);

      /* Poll for and process events */
      glfwPollEvents();
   }

   glfwTerminate();
   return 0;
}
