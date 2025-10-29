/********************************************************************/
/*!
\file		PhysicsManager.cpp
\author		Toh Yu Heng 2301294
\par		t.yuheng@digipen.edu
\date		Oct 01 2025
\brief		This file defines the implementation of the PhysicsManager 
			class responsible for initializing, updating, and shutting 
			down the physics simulation using NVIDIA PhysX.

			The PhysicsManager handles the creation and management of 
			the PhysX foundation, physics instance, scene, dispatcher, 
			materials, and character controller manager. 

			It also provides interfaces for applying forces and torques 
			to entities with Rigidbody components in the ECS system, 
			allowing for physical interactions and motion simulation.

			Key functionalities:
			- Initialization of the PhysX SDK and scene
			- Fixed timestep physics simulation update
			- Safe release of PhysX resources during shutdown
			- Force and torque application to rigid bodies

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "PhysicsManager.h"
#include "Inputs/Input.h"

namespace physics {
	std::shared_ptr<PhysicsManager> PhysicsManager::m_instancePtr = nullptr;

	void PhysicsManager::Init() {
		static PxDefaultAllocator g_Allocator;
		static PxDefaultErrorCallback g_ErrorCallback;

		m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_Allocator, g_ErrorCallback);
		PX_ASSERT(m_foundation);

		m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale{});
		PX_ASSERT(m_physics);

		PxSceneDesc sceneDesc{ m_physics->getTolerancesScale() };
		sceneDesc.gravity = PxVec3{ 0.0f, -9.81f, 0.0f };
		m_cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = m_cpuDispatcher;
		sceneDesc.filterShader = ToPhysxCustomFilter;
		
		m_eventCallback = new PhysicsEventCallback();
		sceneDesc.simulationEventCallback = m_eventCallback;

		m_scene = m_physics->createScene(sceneDesc);
		PX_ASSERT(m_scene);

		m_defaultMaterial = m_physics->createMaterial(0.5f, 0.5f, 0.0f);
		PX_ASSERT(m_defaultMaterial);

		m_controllerManager = PxCreateControllerManager(*m_scene);
		PX_ASSERT(m_controllerManager);
	}

	void PhysicsManager::Shutdown() {
		if (m_controllerManager) {
			m_controllerManager->release();
			m_controllerManager = nullptr;
		}

		if (m_scene) {
			m_scene->release();
			m_scene = nullptr;
		}

		if (m_eventCallback) {
			delete m_eventCallback;
			m_eventCallback = nullptr;
		}

		if (m_cpuDispatcher) {
			m_cpuDispatcher->release();
			m_cpuDispatcher = nullptr;
		}

		if (m_physics) {
			m_physics->release();
			m_physics = nullptr;
		}

		if (m_foundation) {
			m_foundation->release();
			m_foundation = nullptr;
		}
	}

	void PhysicsManager::Update(float deltaTime) {
		if (!m_scene) { return; }
		m_accumulator += deltaTime;
		while (m_accumulator >= m_fixedDeltaTime) {
			m_scene->simulate(m_fixedDeltaTime);
			m_scene->fetchResults(true);
			if (m_eventCallback) { m_eventCallback->ProcessTriggerStay(); }
			m_accumulator -= m_fixedDeltaTime;
		}
	}

	void PhysicsManager::AddForce(void* actor, const glm::vec3& force, ForceMode mode) {
		PxRigidDynamic* rb = static_cast<PxRigidDynamic*>(actor);
		if (!rb->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC)) {
			rb->addForce(PxVec3(force.x, force.y, force.z), ToPhysxForceMode(mode));
		}
	}

	void PhysicsManager::AddTorque(void* actor, const glm::vec3& torque, ForceMode mode) {
		PxRigidDynamic* rb = static_cast<PxRigidDynamic*>(actor);
		if (!rb->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC)) {
			rb->addTorque(PxVec3(torque.x, torque.y, torque.z), ToPhysxForceMode(mode));
		}
	}
}