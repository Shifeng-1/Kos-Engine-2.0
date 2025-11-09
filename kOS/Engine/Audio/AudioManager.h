#pragma once
#include <FMOD/fmod.hpp>

namespace ecs { 
    class AudioSystem; 
}

namespace FMOD {
    class System;
}

namespace audio {

    class AudioManager {
    public:

        AudioManager();                                 
        ~AudioManager();

        FMOD::System* GetCore() { return s_fmod; }

        void Init();

        void SetPaused(bool paused);
        void StopAll();

    private:
        FMOD::System* s_fmod = nullptr;
        bool s_paused = false;
    };

} // namespace audio
