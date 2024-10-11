#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include "glm/glm.hpp"

class Input {
public:
   static bool  keys_pressed[GLFW_KEY_LAST];
   static bool  keys_pressed_down[GLFW_KEY_LAST];
   static bool  left_mouse_pressed;
   static bool  left_mouse_pressed_down;
   static bool  right_mouse_pressed;
   static bool  right_mouse_pressed_down;
   
   static float startTime;
   static float deltaTime;
   static float currentTime;
   static double realTimeLastFrame;
   static double lastTick;

private:
   static bool keys_pressed_last_frame[GLFW_KEY_LAST];

public:
   // Method to update key states
   static void updateKeyStates(GLFWwindow* window) {
      std::copy(std::begin(keys_pressed), std::end(keys_pressed), std::begin(keys_pressed_last_frame));
      for (int key = 0; key < GLFW_KEY_LAST; ++key) {
         keys_pressed[key] = glfwGetKey(window, key) == GLFW_PRESS;
      }
      for (int key = 0; key < GLFW_KEY_LAST; ++key) {
         keys_pressed_down[key] = !keys_pressed_last_frame[key] && keys_pressed[key];
      }

      left_mouse_pressed_down  = !left_mouse_pressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
      left_mouse_pressed       = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
      right_mouse_pressed_down = !right_mouse_pressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;
      right_mouse_pressed      = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;
   }
};

float     zeno(float current, float target, float timeConstant);
glm::vec2 zeno(const glm::vec2& current, const glm::vec2& target, float timeConstant);
glm::vec3 zeno(const glm::vec3& current, const glm::vec3& target, float timeConstant);
glm::vec4 zeno(const glm::vec4& current, const glm::vec4& target, float timeConstant);
