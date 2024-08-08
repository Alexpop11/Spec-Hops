#include "Input.h"

bool Input::keys_pressed[GLFW_KEY_LAST] = { false };
bool Input::keys_pressed_last_frame[GLFW_KEY_LAST] = { false };
bool Input::keys_pressed_down[GLFW_KEY_LAST] = { false };
