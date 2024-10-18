#include "TurretHead.h"
#include "../../AudioEngine.h"

TurretHead::TurretHead(const std::string& name, float x, float y)
   : SquareObject(name, DrawPriority::CharacterAccent, x, y, "turret_head.png") {}
