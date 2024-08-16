#include "Bomb.h"
#include "../World.h"
#include "Tile.h"
#include "Player.h"

Bomb::Bomb(const std::string& name, float x, float y)
    : SquareObject(name, 4, x, y) {
    r = 0.2;
    g = 0.2;
    b = 0.2;
    ExplodeTime = CreationTime + 1.5;
}

void Bomb::update() {
    // Explode the bomb
    
    if (glfwGetTime() >= ExplodeTime) {
        auto nearbyWalls = World::where<Tile>([&](const Tile& tile) {
            return (std::abs(x - tile.x) + std::abs(y - tile.y) < 3);
        });

        for (auto* wall : nearbyWalls) {
            wall->explode();
        }

        auto nearbyPlayers = World::where<Player>([&](const Player& player) {
            return (std::abs(x - player.x) + std::abs(y - player.y) < 3);
        });
        for (auto* player : nearbyPlayers) {
            player->health -= 1;
            std::cout << "bomb damaged player. their health is now " << player->health << std::endl;
        }

        ShouldDestroy = true;
    }
}
