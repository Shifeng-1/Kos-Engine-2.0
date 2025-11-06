/********************************************************************/
/*!
\file		PhysicsManager.h
\author		Toh Yu Heng 2301294
\par		t.yuheng@digipen.edu
\date		Oct 01 2025
\brief		Declares the PhysicsManager class responsible for managing 
			the initialization, simulation, and shutdown of the PhysX 
			physics engine in the application.

			The PhysicsManager acts as a singleton, providing centralized 
			access to PhysX core systems such as:
			- PxFoundation, PxPhysics, and PxScene
			- Default CPU dispatcher and material
			- Character Controller Manager

			It also exposes interfaces to apply forces and torques to 
			entities via Rigidbody components managed by the ECS.

			Key responsibilities:
			- Initialize and configure the PhysX environment
			- Update physics simulation at a fixed timestep
			- Manage and safely release all PhysX resources
			- Provide utility methods for applying physical interactions

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include "PhysicsEventCallback.h"
#include "PhysxUtils.h"
#include "PhysicsLayer.h"
#include "Events/Delegate.h"

using namespace physx;

namespace physics {
	class PhysicsManager {
	public:
		PhysicsManager() = default;  
		~PhysicsManager() = default;
		
		void Init();
		void Update(float);
		void Shutdown();

		PxPhysics* GetPhysics() { return m_physics; }
		PxScene* GetScene() { return m_scene; }
		PxMaterial* GetDefaultMaterial() { return m_defaultMaterial; }
		PxControllerManager* GetControllerManager() { return m_controllerManager; }
		PhysicsEventCallback* GetEventCallback() const { return m_eventCallback; }

		float FixedDeltaTime() const { return m_fixedDeltaTime; }
		int FrameCount() const { return m_frameCount; }

		void AddForce(void*, const glm::vec3&, ForceMode mode = ForceMode::Force);
		void AddTorque(void*, const glm::vec3&, ForceMode mode = ForceMode::Force);

		bool Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& outHit, void* actorToIgnore);

		physicslayer::PhysicsLayer layers;
	private:
		PhysicsManager(const PhysicsManager&) = delete;
		PhysicsManager& operator=(const PhysicsManager&) = delete;
		
		PxDefaultAllocator m_allocator;
		PxDefaultErrorCallback m_errorCallback;
		PxFoundation* m_foundation = nullptr;
		PxPhysics* m_physics = nullptr;
		PxScene* m_scene = nullptr;
		PxDefaultCpuDispatcher* m_cpuDispatcher = nullptr;
		PxMaterial* m_defaultMaterial = nullptr;
		PxControllerManager* m_controllerManager = nullptr;
		PhysicsEventCallback* m_eventCallback = nullptr;

		float m_fixedDeltaTime = 1.0f / 50.0f;
		float m_maximumDeltaTIme = 1.0f / 3.0f;
		float m_accumulator = 0.0f;
		int m_frameCount = 0;
	};
}

#endif