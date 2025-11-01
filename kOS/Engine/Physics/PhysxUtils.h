/********************************************************************/
/*!
\file		PhysxUtils.h
\author		Toh Yu Heng 2301294
\par		t.yuheng@digipen.edu
\date		Oct 01 2025
\brief		This header provides utility functions and conversion helpers 
            for integrating custom ECS-based physics components with the 
            NVIDIA PhysX engine.

            The utilities here serve as a translation layer between 
            engine-specific data structures (e.g., Constraints, ForceMode, 
            CollisionDetectionMode) and PhysX API flags and enums.

            Core functionalities:
            - Convert engine-defined constraints to PhysX rigid body lock flags
            - Configure collision detection modes (discrete or continuous)
            - Set PhysX shape trigger or simulation modes
            - Map custom ForceMode values to PhysX force modes

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#ifndef PHYSXUTILS_H
#define PHYSXUTILS_H

#include "ECS/Component/RigidbodyComponent.h"
#include "Physics/PhysicsLayer.h"
#include "PHYSX/PxPhysicsAPI.h"

using namespace ecs;
using namespace physx;

inline bool HasConstraint(Constraints mask, Constraints flag) {
	return (static_cast<uint32_t>(mask) & static_cast<uint32_t>(flag)) != 0;
}

inline void ToPhysXContraints(PxRigidDynamic* actor, Constraints constraints) {
	PxRigidDynamicLockFlags flags{}; 

	switch (constraints) {
		case Constraints::None:
			break;
		case Constraints::FreezePositionX:
			flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
			break;
		case Constraints::FreezePositionY:
			flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
			break;
		case Constraints::FreezePositionZ:
			flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
			break;
		case Constraints::FreezeRotationX:
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
			break;
		case Constraints::FreezeRotationY:
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
			break;
		case Constraints::FreezeRotationZ:
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
			break;
		case Constraints::FreezePosition:
			flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
			flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
			flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
			break;
		case Constraints::FreezeRotation:
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
			break;
		case Constraints::FreezeAll:
			flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
			flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
			flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
			break;
	}

	actor->setRigidDynamicLockFlags(flags);
}

inline void ToPhysxCollisionDetectionMode(PxRigidDynamic* actor, CollisionDetectionMode mode) {
	switch (mode) {
	case CollisionDetectionMode::Discrete:
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, false);
		break;
	case CollisionDetectionMode::Continuous:
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, false);
		break;
	case CollisionDetectionMode::ContinuousDynamic:
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, true);
		break;
	case CollisionDetectionMode::ContinuousSpeculative:
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, true);
		break;
	}
}

inline void ToPhysxInterpolation(PxRigidDynamic* actor, InterpolationMode mode) {
	switch (mode) {
	case InterpolationMode::Interpolate:
	case InterpolationMode::Extrapolate:
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_POSE_INTEGRATION_PREVIEW, true);
		break;
	default:
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_POSE_INTEGRATION_PREVIEW, false);
		break;
	}
}

inline void ToPhysxIsTrigger(PxShape* shape, bool isTrigger) {
	if (!shape) { return; }
	if (isTrigger) {
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	else {
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	}
}

enum class ForceMode {
	Force = 0,
	Impulse,
	VelocityChange,
	Acceleration
};

inline PxForceMode::Enum ToPhysxForceMode(ForceMode mode) {
	switch (mode) {
	case ForceMode::Force:
		return PxForceMode::eFORCE;
	case ForceMode::Impulse:
		return PxForceMode::eIMPULSE;
	case ForceMode::VelocityChange:
		return PxForceMode::eVELOCITY_CHANGE;
	case ForceMode::Acceleration:
		return PxForceMode::eACCELERATION;
	default:
		return PxForceMode::eFORCE;
	}
}

inline PxFilterFlags ToPhysxCustomFilter(PxFilterObjectAttributes a0, PxFilterData d0, PxFilterObjectAttributes a1, PxFilterData d1, PxPairFlags& pairFlags, const void* block, PxU32 blockSize) {
	if (PxFilterObjectIsTrigger(a0) || PxFilterObjectIsTrigger(a1)) {
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	PxU32 layer0 = d0.word0;
	PxU32 layer1 = d1.word0;
	bool shouldCollide = physicslayer::PhysicsLayer::m_GetInstance()->m_GetCollide(layer0, layer1);
	if (!shouldCollide) { return PxFilterFlag::eSUPPRESS; }
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
	return PxFilterFlag::eDEFAULT;
}

struct RaycastHit {
	PxRigidActor* rigidbody = nullptr;
	PxShape* collider = nullptr;
	glm::vec3 point;
	glm::vec3 normal;
	float distance = 0.0f;
};

#endif