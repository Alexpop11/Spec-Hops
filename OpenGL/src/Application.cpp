#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>

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
#include "Texture.h"

#include "WeakMemoizeConstructor.h"

void setWindowIcon(GLFWwindow* window, const char* iconPath) {
   int            width, height, channels;
   unsigned char* pixels = stbi_load(iconPath, &width, &height, &channels, 4);
   if (pixels) {
      GLFWimage images[1];
      images[0] = {.width = width, .height = height, .pixels = pixels};
      glfwSetWindowIcon(window, 1, images);
      stbi_image_free(pixels);
   } else {
      std::cerr << "Failed to load icon: " << iconPath << std::endl;
   }
}

void key_callback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mods */) {
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

int main(void) {

   const float TICKS_PER_SECOND = 12.0f;

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
   std::string icon_path = Renderer::ResPath() + "Images/Logo2.png";
   setWindowIcon(window, icon_path.c_str());

   /* Make the window's context current */
   glfwMakeContextCurrent(window);

   glfwSwapInterval(1);

   if (glewInit() != GLEW_OK)
      std::cout << "Error!" << std::endl;

   std::cout << "current version of GL: " << glGetString(GL_VERSION) << std::endl;

   GLCall(glEnable(GL_BLEND));
   GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

   Renderer renderer(window);

   World::LoadMap("maps/SpaceShip.txt");

   double currentTime = glfwGetTime();
   double lastTick    = Input::startTime;

   // -------------------
   // Main rendering loop
   // -------------------
   while (!glfwWindowShouldClose(window)) {
      double lastFrameTime = currentTime;
      currentTime          = glfwGetTime();
      Input::deltaTime     = currentTime - lastFrameTime;

      // set the viewport size
      auto [width, height] = renderer.WindowSize();
      glViewport(0, 0, (GLsizei)width, (GLsizei)height);

      sortGameObjectsByPriority(World::gameobjects);
      renderer.Clear();
      Input::updateKeyStates(window);

      // gather all different shaders that are currently in use, and check if we need to
      // recompile the shader because the file was changed.
      // -------------------------------------------------------------------------------
      /* std::set<std::shared_ptr<Shader>> shaders;
      for (auto& gameobject : World::gameobjects) {
         if (gameobject->shader && !shaders.contains(gameobject->shader)) {
            shaders.insert(gameobject->shader);
            gameobject->shader->UpdateIfNeeded();
         }
      }*/

      World::UpdateObjects();

      if (World::shouldTick || lastTick + (1.0 / TICKS_PER_SECOND) <= currentTime) {
         World::TickObjects();
         lastTick          = currentTime;
         World::shouldTick = false;
      }

      // render all objects
      // ------------------
      for (auto& gameobject : World::gameobjects)
         gameobject->render(renderer);

      // swap front and back buffers
      // ---------------------------
      glfwSwapBuffers(window);

      // Poll for and process events
      // ---------------------------
      glfwPollEvents();
   }

   glfwTerminate();
   return 0;
}
