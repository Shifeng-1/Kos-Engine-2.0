#ifndef SPHERERENDERSYS_H
#define SPHERERENDERSYS_H

#include "System.h"
#include "ECS/ECSList.h"

namespace ecs {

    class SphereRenderSystem : public ISystem {

    public:
        using ISystem::ISystem;
        void Init() override;
        void Update() override;

        REFLECTABLE(SphereRenderSystem)
    };

}

#endif