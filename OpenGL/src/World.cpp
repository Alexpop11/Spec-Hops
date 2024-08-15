#include "World.h"

std::vector<std::unique_ptr<GameObject>> World::gameobjects = {};
std::vector<std::unique_ptr<GameObject>> World::gameobjectstoadd = {};