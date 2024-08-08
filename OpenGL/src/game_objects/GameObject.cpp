

#include "GameObject.h"
#include "../Input.h"


GameObject::GameObject(const std::string& name, int drawPriority, float x, float y)
    : name(name), drawPriority(drawPriority), x(x), y(y) {
    // TODO: Add any additional initialization if needed
}

void GameObject::update() 
{
    
}

void GameObject::render(Renderer& renderer) {
    std::cout << "GameObject render was called" << std::endl;
}