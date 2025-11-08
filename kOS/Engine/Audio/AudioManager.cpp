#include "AudioManager.h"

namespace audio {
    AudioManager::AudioManager() { s_fmod = nullptr; s_paused = false; }

    AudioManager::~AudioManager(){
        if (s_fmod) {
            s_fmod->release();
            s_fmod = nullptr;
        }
    }

    void AudioManager::Init() {

        if (s_fmod) return; 

        FMOD::System* core = nullptr;
        FMOD_RESULT fr = FMOD::System_Create(&core);
        if (fr != FMOD_OK || !core) return;

        core->set3DSettings(1.0f, 1.0f, 1.0f);

        // initialize core
        fr = core->init(512, FMOD_INIT_NORMAL, nullptr);
        if (fr != FMOD_OK) { core->release(); return; }

        s_fmod = core;
        s_paused = false;
    }

    void AudioManager::SetPaused(bool paused) {
        s_paused = paused;

        if (!s_fmod) return;

        FMOD::ChannelGroup* master = nullptr;
        if (s_fmod->getMasterChannelGroup(&master) == FMOD_OK && master) {
            master->setPaused(paused);
        }
    }

    void AudioManager::StopAll() {
        if (!s_fmod) return;

        FMOD::ChannelGroup* master = nullptr;
        if (s_fmod->getMasterChannelGroup(&master) == FMOD_OK && master) {
            master->stop();
        }
    }
} 
