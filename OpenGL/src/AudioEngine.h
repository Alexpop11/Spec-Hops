#pragma once

#include <memory>
#include <string>
#include <iostream>
#include "miniaudio.h"

class Sound {
private:
   ma_engine* engine;
   ma_sound   sound;

public:
   Sound(const std::string& filename, ma_engine* engine);
   Sound(const Sound&)            = delete;
   Sound& operator=(const Sound&) = delete;
   Sound(Sound&& other) noexcept;
   Sound& operator=(Sound&& other) noexcept;
   void   play();
   void   setPitch(float pitch); // New method to set pitch
   ~Sound();
};


class MiniAudioEngine {
public:
   ma_engine engine;
   MiniAudioEngine() {
      ma_result result;

      result = ma_engine_init(NULL, &engine);
      if (result != MA_SUCCESS) {
         std::cout << "Failed to initialize audio engine - " << result << std::endl;
      }
   }
};

class AudioEngine {
private:
   MiniAudioEngine engine;

   Sound getSound(const std::string& name);

public:
   AudioEngine();
   void play(Sound& sound);
   void Update(float newTimeSpeed);

   Sound Walk;
   Sound Walk1;
   Sound Bomb_Sound;
   Sound Death_Sound;
   Sound Bullet_Sound;
   Sound Hurt_Sound;
   Sound Bomb_Place;
   Sound Bomb_Tick;
   Sound Enemy_Hurt;
   Sound Zap;
   Sound Impact;
   Sound Scuff;
   Sound Song;
};

AudioEngine& audio();
