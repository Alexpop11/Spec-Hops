#include "Bomber.h"

Bomber::Bomber(const std::string& name, float x, float y)
    : Character(name, x, y) {
    drawPriority = 3;
    health = 1;
    r = 0.8;
    g = 0.2;
    b = 0.2;
}

void Bomber::move(float new_x, float new_y) {
    
    auto nearbyBombsCurrent = World::where<Bomb>([&](const Bomb& bomb) {
        return (std::abs(x - bomb.x) + std::abs(y - bomb.y) < 3);
        });
    auto nearbyBombsNew = World::where<Bomb>([&](const Bomb& bomb) {
        return (std::abs(new_x - bomb.x) + std::abs(new_y - bomb.y) < 3);
        });
    if (nearbyBombsNew.empty() || !nearbyBombsCurrent.empty()) {
        Character::move(new_x, new_y);
    }
    if (nearbyBombsCurrent.empty()) {
        for (auto& tile : World::at<Tile>(new_x, new_y)) {
            if (tile->wall) {
                // Check for nearby players
                auto nearbyPlayers = World::where<Player>([&](const Player& player) {
                    return (std::abs(x - player.x) + std::abs(y - player.y) < 14);
                    });
                if (!nearbyPlayers.empty()) {
                    auto player = nearbyPlayers[0];
                    World::gameobjectstoadd.push_back(std::make_unique<Bomb>(Bomb("CoolBomb", x, y)));
                    Character::move(x - sign(player->x - x), y);
                    Character::move(x, y - sign(player->y - y));
                }
                break;
            }
        };
    }
}

void Bomber::update() {
    if (health <= 0) {
        ShouldDestroy = true;
    }

    // Check for nearby players
    auto nearbyPlayers = World::where<Player>([&](const Player& player) {
        return (std::abs(x - player.x) + std::abs(y - player.y) < 14);
        });

    // Check for nearby bombs
    auto nearbyBombs = World::where<Bomb>([&](const Bomb& bomb) {
        return (std::abs(x - bomb.x) + std::abs(y - bomb.y) < 3);
        });

    // Move to player
    if (!nearbyBombs.empty()) {
        auto bomb = nearbyBombs[0];
        // Move away from bomb
        move(x + (x > bomb->x ? 1 : -1), y);
        move(x, y + (y > bomb->y ? 1 : -1));
    }
    else if (!nearbyPlayers.empty() && nearbyBombs.empty()) {
        auto player = nearbyPlayers[0];
        move(x + sign(player->x - x), y);
        move(x, y + sign(player->y - y));

        if (std::abs(x - player->x) + std::abs(y - player->y) < 2) {
            // Drop a bomb
            World::gameobjectstoadd.push_back(std::make_unique<Bomb>(Bomb("CoolBomb", x, y)));

            // Move away from player after dropping bomb
            move(x - sign(player->x - x), y);
            move(x, y - sign(player->y - y));
        }
    }
}
