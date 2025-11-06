#pragma once
#include <FMOD/fmod.hpp>
#include "Resources/R_Audio.h"
#include "ECS/System/AudioSystem.h"

namespace ecs { class AudioSystem; }

namespace FMOD {
    class System;
}

namespace audio {

    class AudioManager {
    public:

        AudioManager();                                 
        ~AudioManager();

        static FMOD::System* GetCore() { return s_fmod; }

        void Init();


        void SetPaused(bool paused);
        void StopAll();

    private:
        static FMOD::System* s_fmod;
        static bool s_paused;
    };

} // namespace audio
