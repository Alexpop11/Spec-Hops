#include "AudioEngine.h"
#include <cstring>
#include "World.h"


Sound::Sound(const std::filesystem::path& filename, ma_engine* engine)
   : engine(engine) {
   std::string filenameStr = filename.string();
   ma_result   result = ma_sound_init_from_file(engine, filenameStr.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, &sound);
   if (result != MA_SUCCESS) {
      std::cout << "Failed to load sound - " << result << std::endl;
   }
}

Sound::Sound(Sound&& other) noexcept
   : engine(other.engine)
   , sound(other.sound) {
   other.engine = nullptr;
   std::memset(&other.sound, 0, sizeof(ma_sound));
}

Sound& Sound::operator=(Sound&& other) noexcept {
   if (this != &other) {
      if (engine != nullptr) {
         ma_sound_uninit(&sound);
      }
      engine       = other.engine;
      sound        = other.sound;
      other.engine = nullptr;
      std::memset(&other.sound, 0, sizeof(ma_sound));
   }
   return *this;
}

void Sound::setPitch(float pitch) {
   if (engine != nullptr) {
      ma_sound_set_pitch(&sound, pitch);
   }
}

void Sound::play() {
   if (engine != nullptr) {
      ma_sound_set_pitch(&sound, World::timeSpeed);
      ma_sound_start(&sound);
   }
}

Sound::~Sound() {
   if (engine != nullptr) {
      ma_sound_uninit(&sound);
   }
}


AudioEngine::AudioEngine()
   : Walk(getSound("walk1.wav"))
   , Walk1(getSound("walk2.wav"))
   , Bomb_Sound(getSound("bomb1.wav"))
   , Death_Sound(getSound("death2.wav"))
   , Bullet_Sound(getSound("bullet.wav"))
   , Hurt_Sound(getSound("ouch2.wav"))
   , Bomb_Place(getSound("bomb_place.wav"))
   , Bomb_Tick(getSound("bomb_tick.wav"))
   , Enemy_Hurt(getSound("enemy_ouch.wav"))
   , Zap(getSound("zap.wav"))
   , Impact(getSound("impact.wav"))
   , Scuff(getSound("scuff.wav"))
   , Song(getSound("acid_splash.wav")) {

   Update(World::timeSpeed);
}

Sound AudioEngine::getSound(const std::filesystem::path& name) {
   return Sound(Application::get().res_path / "sounds" / name, &engine.engine);
}

void AudioEngine::Update(float newTimeSpeed) {
   Walk.setPitch(newTimeSpeed);
   Walk1.setPitch(newTimeSpeed);
   Bomb_Sound.setPitch(newTimeSpeed);
   Death_Sound.setPitch(newTimeSpeed);
   Bullet_Sound.setPitch(newTimeSpeed);
   Hurt_Sound.setPitch(newTimeSpeed);
   Bomb_Place.setPitch(newTimeSpeed);
   Bomb_Tick.setPitch(newTimeSpeed);
   Enemy_Hurt.setPitch(newTimeSpeed);
   Zap.setPitch(newTimeSpeed);
   Impact.setPitch(newTimeSpeed);
   Scuff.setPitch(newTimeSpeed);
   Song.setPitch(newTimeSpeed);
}

void AudioEngine::play(Sound& sound) {
   sound.play();
}

AudioEngine& audio() {
   static AudioEngine audioEngine = AudioEngine(); // Initialized first time this function is called
   return audioEngine;
}
