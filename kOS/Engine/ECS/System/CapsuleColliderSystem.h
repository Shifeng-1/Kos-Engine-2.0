#ifndef CAPSULECOLLIDERSYSTEM_H
#define CAPSULECOLLIDERSYSTEM_H

#include "ECS/ECS.h"
#include "System.h"

namespace ecs {
	class CapsuleColliderSystem : public ISystem {
	public:
		using ISystem::ISystem;
		void Init() override;
		void Update() override;
		REFLECTABLE(CapsuleColliderSystem)
	};
}

#endif