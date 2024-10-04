#pragma once
#include "Character.h"
#include "ui/Text.h"


class Player : public Character {
public:
   Player(const std::string& name, int tile_x, int tile_y);
   virtual void update() override;
   bool         moved_last_tick = false;
   virtual void tickUpdate() override;
   bool         key_pressed_last_frame = false;
   virtual void move(int new_x, int new_y) override;
   virtual void hurt() override;
   bool         second_step = false;

   std::unique_ptr<Text> healthText;

   virtual std::vector<GameObject*> children() override { return {healthText.get()}; }

   // Powerups
   int playerBunnyHopCoolDown = 5;
};
