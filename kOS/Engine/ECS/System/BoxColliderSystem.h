#ifndef BOXCOLLIDERSYSTEM_H
#define BOXCOLLIDERSYSTEM_H

#include "ECS/ECS.h"
#include "System.h"

namespace ecs {
	class BoxColliderSystem : public ISystem {
	public:
		using ISystem::ISystem;
		void Init() override;
		void Update() override;
		REFLECTABLE(BoxColliderSystem)
	};
}

#endif