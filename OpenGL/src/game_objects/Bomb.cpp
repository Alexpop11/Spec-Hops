#include "Bomb.h"
#include "../World.h"
#include "Tile.h"

Bomb::Bomb(const std::string& name, float x, float y)
    : SquareObject(name, 4, x, y) {
    r = 0.2;
    g = 0.2;
    b = 0.2;
    ExplodeTime = CreationTime + 1.5;
}

void Bomb::update() {
    // Update logic for Bomb
    if (glfwGetTime() >= ExplodeTime) {
        std::cout << "This bomb has 'sploded." << std::endl;
        for (auto& gameobject : World::gameobjects) {
            auto tile = dynamic_cast<Tile*>(&*gameobject);
            if (tile != nullptr) {
                if (((x == tile->x + 1 || x == tile->x - 1 || x == tile->x) && (y == tile->y + 1 || y == tile->y - 1 || y == tile->y)) && tile->wall) {
                    tile->explode();
                }
            }
        };
        ShouldDestroy = true;
    }
}
