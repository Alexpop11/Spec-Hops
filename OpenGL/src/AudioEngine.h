#pragma once

#include <memory>
#include <string>
#include "soloud.h"
#include "soloud_wav.h"

class Sound {
private:
   SoLoud::Soloud& soloud;
   SoLoud::Wav     wav;

public:
   Sound(std::string filename, SoLoud::Soloud& soloud);
   void play();
};

class AudioEngine {
private:
   SoLoud::Soloud soloud;

   Sound getSound(const std::string& name, SoLoud::Soloud& audioEngine);

public:
   AudioEngine();
   void play(Sound& sound);
   ~AudioEngine();

   Sound Walk;
};
