#ifndef DEBUGSPHERESYSTEM_H
#define DEBUGSPHERESYSTEM_H

#include "ECS/ECS.h"
#include "System.h"

namespace ecs {
    class  DebugSphereColliderRenderSystem : public ISystem {
    public:
        void Init() override;
        void Update() override;

        REFLECTABLE(DebugSphereColliderRenderSystem)
    };
}

#endif