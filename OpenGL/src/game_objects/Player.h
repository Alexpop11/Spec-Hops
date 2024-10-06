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
   virtual void render(Renderer& renderer) override;

   std::unique_ptr<Text> healthText;
   std::unique_ptr<Text> topText;

   virtual std::vector<GameObject*> children() override { return {healthText.get(), topText.get()}; }

   // Powerups
   int playerBunnyHopCoolDown = 5;
};
