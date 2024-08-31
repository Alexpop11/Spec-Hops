#pragma once
#include <GLFW/glfw3.h>
#include <iostream>

class Input {
public:
   static bool  keys_pressed[GLFW_KEY_LAST];
   static bool  keys_pressed_down[GLFW_KEY_LAST];
   static float startTime;
   static float deltaTime;

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
   }
};

float zeno(float current, float target, float timeConstant);
