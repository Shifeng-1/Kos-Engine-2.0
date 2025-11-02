#ifndef DEBUGCAPSULESYSTEM_H
#define DEBUGCAPSULESYSTEM_H

#include "ECS/ECS.h"
#include "System.h"

namespace ecs {
    class DebugCapsuleColliderRenderSystem : public ISystem {
    public:
        void Init() override;
        void Update() override;
        
        REFLECTABLE(DebugCapsuleColliderRenderSystem)
    };
}

#endif