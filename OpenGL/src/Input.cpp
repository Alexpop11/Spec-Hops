#include "Input.h"
#include "glm/glm.hpp"

float Input::startTime                              = 0;
float Input::deltaTime                              = 0.01;

bool  Input::keys_pressed[GLFW_KEY_LAST]            = {false};
bool  Input::keys_pressed_last_frame[GLFW_KEY_LAST] = {false};
bool  Input::keys_pressed_down[GLFW_KEY_LAST]       = {false};
bool  Input::left_mouse_pressed                          = false;
bool  Input::left_mouse_pressed_down                     = false;
bool  Input::right_mouse_pressed                          = false;
bool  Input::right_mouse_pressed_down                     = false;

float zeno(float current, float target, float timeConstant) {
   float alpha = 1.0f - std::exp(-Input::deltaTime / timeConstant);
   return current + alpha * (target - current);
}

glm::vec2 zeno(const glm::vec2& current, const glm::vec2& target, float timeConstant) {
   float alpha = 1.0f - std::exp(-Input::deltaTime / timeConstant);
   return current + alpha * (target - current);
}

glm::vec3 zeno(const glm::vec3& current, const glm::vec3& target, float timeConstant) {
   float alpha = 1.0f - std::exp(-Input::deltaTime / timeConstant);
   return current + alpha * (target - current);
}

glm::vec4 zeno(const glm::vec4& current, const glm::vec4& target, float timeConstant) {
   float alpha = 1.0f - std::exp(-Input::deltaTime / timeConstant);
   return current + alpha * (target - current);
}


