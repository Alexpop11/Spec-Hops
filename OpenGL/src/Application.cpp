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
#include "game_objects/Fog.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION

#if defined(IMGUI_IMPL_OPENGL_ES2)
   #include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// Utility function to draw text without any frame
void DrawTextOverlay(const char* text, float x, float y, ImU32 color = IM_COL32(255, 255, 255, 255)) {
   ImDrawList* draw_list = ImGui::GetForegroundDrawList();
   draw_list->AddText(ImVec2(x, y), color, text);
}

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

   // Initialize ImGui
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO();
   (void)io;
   ImGui::StyleColorsDark();
   const char* glsl_version = "#version 330";
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init(glsl_version);

   Renderer renderer(window);

   World::LoadMap("maps/SpaceShip.txt");
   World::gameobjects.push_back(std::make_unique<Fog>());

   double currentTime = glfwGetTime();
   double lastTick    = Input::startTime;

   // -------------------
   // Main rendering loop
   // -------------------
   while (!glfwWindowShouldClose(window)) {
      double lastFrameTime = currentTime;
      currentTime          = glfwGetTime();
      Input::deltaTime     = currentTime - lastFrameTime;

      // Set the viewport size
      auto [width, height] = renderer.WindowSize();
      glViewport(0, 0, (GLsizei)width, (GLsizei)height);

      sortGameObjectsByPriority(World::gameobjects);
      renderer.Clear();
      Input::updateKeyStates(window);

      World::UpdateObjects();

      sortGameObjectsByPriority(World::gameobjects);

      if (World::shouldTick || lastTick + (1.0 / TICKS_PER_SECOND) <= currentTime) {
         World::TickObjects();
         lastTick          = currentTime;
         World::shouldTick = false;
      }

      // Render all objects
      for (auto& gameobject : World::gameobjects)
         gameobject->render(renderer);

      // Render debug lines
      renderer.DrawDebug();

      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // Example ImGui window with text (optional)
      {
         ImGui::Begin("Debug Info");
         ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
         ImGui::End();
      }

      // Draw text without any frame
      DrawTextOverlay("Hello, World!", 50.0f, 50.0f, IM_COL32(255, 255, 0, 255));    // Yellow text at (50, 50)
      DrawTextOverlay("Player Health: 100", 50.0f, 80.0f, IM_COL32(0, 255, 0, 255)); // Green text at (50, 80)

      // Render ImGui
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      // Swap front and back buffers
      glfwSwapBuffers(window);

      // Poll for and process events
      glfwPollEvents();
   }

   // Cleanup ImGui
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   glfwTerminate();
   return 0;
}
