#ifndef STATICRIGIDBODYSYSTEM_H
#define STATICRIGIDBODYSYSTEM_H

#include "ECS/ECS.h"
#include "System.h"

namespace ecs {
	class StaticRigidbodySystem : public ISystem {
	public:
		using ISystem::ISystem;
		void Init() override;
		void Update() override;
		REFLECTABLE(StaticRigidbodySystem)
	private:
		bool IsShapeAttachedToActor(PxRigidActor*, PxShape*);
	};
}

#endif