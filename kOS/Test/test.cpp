/******************************************************************/
/*!
\file      test.cpp
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Oct 02, 2025
\brief     This file contains defination of the test cases for ecs components
		   and some math utility functions.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#include <gtest/gtest.h>
#include "common.h"
#include "ECS/ECS.h"
#include "Scene/SceneManager.h"
#include "Utility/MathUtility.h"
#include "glm/gtx/euler_angles.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace ecs;

SERIALIZE_DESERIALIZE_COMPARE_TEST(TransformComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(NameComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(SpriteComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(CameraComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(AudioComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(TextComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(CanvasRendererComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(MeshFilterComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(MeshRendererComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(SkinnedMeshRendererComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(AnimatorComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(LightComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(ScriptComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(BoxColliderComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(CapsuleColliderComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(SphereColliderComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(RigidbodyComponent)
SERIALIZE_DESERIALIZE_COMPARE_TEST(CharacterControllerComponent)
// Add more component tests as needed


//TEST(Scene, CreateScene) {
//    scenes::SceneManager sm;
//	EXPECT_TRUE(sm.ImmediateLoadScene("Test Scene"));
//
//	sm.ImmediateClearScene("Test Scene");
//}
//
//
//TEST(Entity, SpawnMaxEntities) {
//	auto* ecs =ComponentRegistry::GetECSInstance();
//	m_ecs.RegisterComponent<TransformComponent>();
//	m_ecs.RegisterComponent<NameComponent>();
//
//	auto* sm = scenes::SceneManager::m_GetInstance();
//	EXPECT_TRUE(sm->ImmediateLoadScene("Test Scene"));
//
//	const int numEntities = ecs::MaxEntity - 10;
//	
//	std::vector<ecs::EntityID> entityIDs;
//	entityIDs.reserve(numEntities);
//	for (int i = 0; i < numEntities; ++i) {
//		ecs::EntityID newEntity = m_ecs.CreateEntity("Test Scene");
//		entityIDs.push_back(newEntity);
//	}
//	// Verify that all entities were created successfully
//	for (const auto& id : entityIDs) {
//		EXPECT_TRUE(m_ecs.IsValidEntity(id));
//	}
//	// Clean up by destroying the created entities
//	for (const auto& id : entityIDs) {
//		m_ecs.DeleteEntity(id);
//	}
//
//	sm->ImmediateClearScene("Test Scene");
//}



TEST(Math, RandomDecomposeTRS) {
    constexpr int NUM_TESTS = 100;
    constexpr float EPS_POS = 0.0001f;
    constexpr float EPS_SCALE = 0.0001f;
    constexpr float EPS_ROT = 0.0001f;
    
	RandomizeComponents<glm::vec3> randomGen;

    for (int i = 0; i < NUM_TESTS; ++i) {
        glm::vec3 pos;
        glm::vec3 scale(1.f,2.f,5.f); // avoid zero scale
        glm::vec3 eulerRot;

		randomGen(pos);
		randomGen(eulerRot);

        // Build rotation quaternion
        glm::quat qx = glm::angleAxis(glm::radians(eulerRot.x), glm::vec3(1, 0, 0));
        glm::quat qy = glm::angleAxis(glm::radians(eulerRot.y), glm::vec3(0, 1, 0));
        glm::quat qz = glm::angleAxis(glm::radians(eulerRot.z), glm::vec3(0, 0, 1));
        glm::quat q = qz * qy * qx;

        // Build transformation matrix
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(q) * glm::scale(glm::mat4(1.0f), scale);

        // Decompose
        glm::vec3 decPos, decRot, decScale;
        utility::DecomposeMtxIntoTRS(transform, decPos, decRot, decScale);

        // Compare position
        EXPECT_TRUE(glm::all(glm::epsilonEqual(pos, decPos, EPS_POS)))
            << "Position mismatch: " << glm::to_string(pos) << " vs " << glm::to_string(decPos);

        // Compare scale
        EXPECT_TRUE(glm::all(glm::epsilonEqual(scale, decScale, EPS_SCALE)))
            << "Scale mismatch: " << glm::to_string(scale) << " vs " << glm::to_string(decScale);

        // Compare rotation via quaternions
        glm::quat decQuat = glm::quat(glm::radians(decRot));
        float dot = glm::dot(q, decQuat);
        EXPECT_NEAR(std::abs(dot), 1.0f, EPS_ROT)
            << "Rotation mismatch: " << glm::to_string(eulerRot) << " vs " << glm::to_string(decRot);
    }
}

