#ifndef SPHERERENDERSYS_H
#define SPHERERENDERSYS_H

#include "System.h"
#include "ECS/ECSList.h"

namespace ecs {

    class SphereRenderSystem : public ISystem {

    public:

        void Init() override;
        void Update() override;

        REFLECTABLE(SphereRenderSystem)
    };

}

#endif