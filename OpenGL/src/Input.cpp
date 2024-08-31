#include "Input.h"

float Input::startTime                              = 0;
float Input::deltaTime                              = 0.01;
bool  Input::keys_pressed[GLFW_KEY_LAST]            = {false};
bool  Input::keys_pressed_last_frame[GLFW_KEY_LAST] = {false};
bool  Input::keys_pressed_down[GLFW_KEY_LAST]       = {false};

float zeno(float current, float target, float timeConstant) {
   float alpha = 1.0f - std::exp(-Input::deltaTime / timeConstant);
   return current + alpha * (target - current);
}
