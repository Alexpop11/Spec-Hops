#include "AudioEngine.h"
#include "Renderer.h"

Sound::Sound(std::string filename, SoLoud::Soloud& soloud)
   : soloud(soloud) {
   wav.load(filename.c_str());
}

void Sound::play() {
   soloud.play(wav);
}

AudioEngine::AudioEngine()
   : Walk(getSound("walk1.wav", soloud)) {
   soloud.init();
}

Sound AudioEngine::getSound(const std::string& name, SoLoud::Soloud& audioEngine) {
   return Sound(Renderer::ResPath() + "Sounds/" + name, soloud);
}

void AudioEngine::play(Sound& sound) {
   sound.play();
}

AudioEngine::~AudioEngine() {
   soloud.deinit();
}
