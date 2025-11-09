/******************************************************************/
/*!
\file      R_Audio.h
\author    Chiu Jun Jie
\par
\date      Oct 03, 2025
\brief     This file contains the definition of the Audio Resource
           class. It handles the loading and unloading of the Audio
           through the resource manager.


Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#pragma once
#include "Config/pch.h"
#include "Resource.h"
#include <FMOD/fmod.hpp>

namespace FMOD { class System; class Sound; }

class R_Audio : public Resource {
public:
    using Resource::Resource;

    void Load() override;
    void Unload() override;

    ~R_Audio() override { Unload(); }

    FMOD::Sound* GetSound()  const { return m_sound; }
    FMOD::System* GetSystem() const { return m_system; }

    void SetSystem(FMOD::System* sys) { m_system = sys; }

    REFLECTABLE(R_Audio);

private:
    FMOD::System* m_system = nullptr;        
    FMOD::Sound* m_sound = nullptr;       
    unsigned int  m_createFlags = 0;
};
