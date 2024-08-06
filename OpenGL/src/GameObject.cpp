#include "GameObject.h"

GameObject::GameObject(const std::string& name, int drawPriority, float x, float y)
    : name(name), drawPriority(drawPriority), x(x), y(y) {
}

void GameObject::render() {
    // TODO: Implement rendering logic
    // This function should handle drawing the GameObject
}
