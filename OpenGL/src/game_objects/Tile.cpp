#include "Tile.h"


Tile::Tile(const std::string& name, bool wall, float x, float y)
    : SquareObject(name, 2, x, y), wall(wall)
{
    if (wall == true) {
        r = 0.4;
        g = 0.4;
        b = 0.4;
    }
    else {
        r = 0.5;
        g = 0.5;
        b = 0.5;
    }
}

Tile::Tile(const std::string& name, float x, float y)
    : Tile(name, false, x, y)
{

}

void Tile::explode() {
    wall = false;
    r = 0.5;
    g = 0.5;
    b = 0.5;
}

void Tile::update() {
    
}
