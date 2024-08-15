#pragma once


#include "game_objects/GameObject.h"

class World {
public:
	static std::vector<std::unique_ptr<GameObject>> gameobjects;
};