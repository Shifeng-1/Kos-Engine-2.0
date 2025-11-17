#ifndef SPHERECOLLIDERSYSTEM_H
#define SPHERECOLLIDERSYSTEM_H

#include "ECS/ECS.h"
#include "System.h"

namespace ecs {
	class SphereColliderSystem : public ISystem {
	public:
		using ISystem::ISystem;
		void Init() override;
		void Update() override;
		REFLECTABLE(SphereColliderSystem)
	};
}

#endif