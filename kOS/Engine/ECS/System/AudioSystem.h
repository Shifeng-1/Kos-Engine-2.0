/******************************************************************/
/*!
\file      AudioSystem.h
\author    Chiu Jun Jie
\par       junjie.c@digipen.edu
\date      Oct 03,2025
\brief     This file contains the declaration of the AudioSystem
           class. It handles the logic behind the Audio played in
           the engine and works with the Audio Manager interface.
            


Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#ifndef AUDIOSYS_H
#define AUDIOSYS_H

#include "ECS/ECS.h"
#include "System.h"


namespace FMOD { 
    class System; 
}

namespace ecs {
    class AudioSystem : public ISystem {
    public:
        using ISystem::ISystem;

        void Init() override;
        void Update() override;
 
        void SetListener(const glm::vec3& pos,
            const glm::vec3& fwd,
            const glm::vec3& up);

        REFLECTABLE(AudioSystem)

    private:
        glm::vec3 m_listenerPos{ 0.0f, 0.0f, 0.0f };
        glm::vec3 m_listenerFwd{ 0.0f, 0.0f, -1.0f };
        glm::vec3 m_listenerUp{ 0.0f, 1.0f,  0.0f };
    };
}

#endif // AUDIOSYS_H

