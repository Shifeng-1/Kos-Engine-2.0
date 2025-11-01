/********************************************************************/
/*!
\file       RigidbodyComponent.h
\author		Toh Yu Heng 2301294
\par		t.yuheng@digipen.edu
\date		Oct 01 2025
\brief		Defines the RigidbodyComponent class, representing a 
            dynamic physics body used for motion simulation in the ECS.

            Provides configuration for mass, drag, gravity, kinematic 
            state, interpolation, collision detection mode, and 
            movement constraints.

            The PhysX actor pointer is stored as a void* to decouple 
            ECS from PhysX dependencies at the component level.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include "Component.h"

namespace ecs {
	enum class InterpolationMode {
		None = 0,
		Interpolate,
		Extrapolate
	};

	enum class CollisionDetectionMode {
		Discrete = 0,
		Continuous,
		ContinuousDynamic,
		ContinuousSpeculative
	};

	enum class Constraints : uint32_t {
		None = 0,
		FreezePositionX,
		FreezePositionY,
		FreezePositionZ,
		FreezeRotationX,
		FreezeRotationY,
		FreezeRotationZ,
		FreezePosition,
		FreezeRotation,
		FreezeAll
	};

	class RigidbodyComponent : public Component {
	public:
		float mass = 1.0f;
		float drag = 0.0f;
		float angularDrag = 0.05f;
		bool useGravity = true;
		bool isKinematic = false;
		InterpolationMode interpolation = InterpolationMode::None;
		CollisionDetectionMode collisionDetection = CollisionDetectionMode::Discrete;
		Constraints constraints = Constraints::None;
		void* actor = nullptr;

		REFLECTABLE(RigidbodyComponent, mass, drag, angularDrag, useGravity, isKinematic, interpolation, collisionDetection, constraints)
	};
}

#endif 