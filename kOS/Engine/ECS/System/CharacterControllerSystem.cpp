/********************************************************************/
/*!
\file       CharacterControllerSystem.cpp
\author		Toh Yu Heng 2301294
\par		t.yuheng@digipen.edu
\date		Oct 01 2025
\brief		Implements the CharacterControllerSystem which manages
            all character controller entities in the ECS using 
            NVIDIA PhysX.

            Responsibilities:
            - Initializes PhysX character controllers for entities 
              with CharacterControllerComponent
            - Handles movement input from the InputSystem (WASD, jump)
            - Applies gravity and jump forces to controllers
            - Updates ECS TransformComponent positions based on 
              controller movement
            - Maintains character grounding and velocity state

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "CharacterControllerSystem.h"
#include "Inputs/Input.h"
#include "Physics/PhysicsManager.h"

using namespace physics;

namespace ecs {
	void CharacterControllerSystem::Init() {
        onDeregister.Add([this](EntityID id) {
            ECS* ecs = ECS::GetInstance();
            if (CharacterControllerComponent* charctrl = ecs->GetComponent<CharacterControllerComponent>(id)) {
                if (PxController* ctrl = static_cast<PxController*>(charctrl->controller)) {
                    PxControllerManager* mgr = PhysicsManager::GetInstance()->GetControllerManager();
                    if (mgr) { mgr->purgeControllers(); }
                    charctrl->controller = nullptr;
                }
            }
        });
    }

	void CharacterControllerSystem::Update(const std::string& scene) {
        ECS* ecs = ECS::GetInstance();
        const auto& entities = m_entities.Data();

        for (EntityID id : entities) {
            TransformComponent* trans = ecs->GetComponent<TransformComponent>(id);
            NameComponent* name = ecs->GetComponent<NameComponent>(id);
            CharacterControllerComponent* charctrl = ecs->GetComponent<CharacterControllerComponent>(id);

            if (!trans || !charctrl || trans->scene != scene || name->hide) { continue; }

            PxController* ctrl = static_cast<PxController*>(charctrl->controller);

            if (!ctrl) {
                if (ecs->HasComponent<CapsuleColliderComponent>(id)) {
                    CapsuleColliderComponent* capsule = ecs->GetComponent<CapsuleColliderComponent>(id);
                    PxCapsuleControllerDesc desc;
                    desc.height = capsule->capsule.height;
                    desc.radius = capsule->capsule.radius;
                    desc.position = PxExtendedVec3{ trans->LocalTransformation.position.x, trans->LocalTransformation.position.y, trans->LocalTransformation.position.z };
                    desc.slopeLimit = cosf(glm::radians(charctrl->slopeLimit));
                    desc.stepOffset = charctrl->stepOffset;
                    desc.contactOffset = charctrl->skinWidth;
                    desc.material = PhysicsManager::GetInstance()->GetDefaultMaterial();
                    desc.density = 10.0f;
                    ctrl = PhysicsManager::GetInstance()->GetControllerManager()->createController(desc);
                }
                else if (ecs->HasComponent<BoxColliderComponent>(id)) {
                    BoxColliderComponent* box = ecs->GetComponent<BoxColliderComponent>(id);
                    PxBoxControllerDesc desc;
                    desc.halfHeight = box->box.size.y * 0.5f;
                    desc.halfSideExtent = box->box.size.x * 0.5f;
                    desc.halfForwardExtent = box->box.size.z * 0.5f;
                    desc.position = PxExtendedVec3{ trans->LocalTransformation.position.x, trans->LocalTransformation.position.y, trans->LocalTransformation.position.z };
                    desc.slopeLimit = cosf(glm::radians(charctrl->slopeLimit));
                    desc.stepOffset = charctrl->stepOffset;
                    desc.contactOffset = charctrl->skinWidth;
                    desc.material = PhysicsManager::GetInstance()->GetDefaultMaterial();
                    desc.density = 10.0f;
                    ctrl = PhysicsManager::GetInstance()->GetControllerManager()->createController(desc);
                }

                if (!ctrl) { continue; }
                ctrl->getActor()->userData = reinterpret_cast<void*>(static_cast<std::uintptr_t>(id));
                charctrl->controller = ctrl;
                charctrl->prevPos = trans->LocalTransformation.position;
                charctrl->isChanged = true;
            }

            const float posThreshold = 0.001f;
            bool transformChanged = false;

            if (charctrl->isChanged) {
                bool posChanged = glm::length(trans->LocalTransformation.position - charctrl->prevPos) > posThreshold;
                transformChanged = posChanged;
            }

            if (transformChanged) {
                PxExtendedVec3 newPos{ trans->LocalTransformation.position.x, trans->LocalTransformation.position.y, trans->LocalTransformation.position.z };
                ctrl->setPosition(newPos);
                charctrl->yVelocity = 0.0f;
                charctrl->isGrounded = false;
            }

            glm::vec3 moveVec{ 0.0f };
            if (Input::InputSystem::GetInstance()->IsKeyPressed(keys::W)) { moveVec.z -= 1.0f; }
            if (Input::InputSystem::GetInstance()->IsKeyPressed(keys::S)) { moveVec.z += 1.0f; }
            if (Input::InputSystem::GetInstance()->IsKeyPressed(keys::A)) { moveVec.x -= 1.0f; }
            if (Input::InputSystem::GetInstance()->IsKeyPressed(keys::D)) { moveVec.x += 1.0f; }

            if (glm::length(moveVec) > 0.0f) { moveVec = glm::normalize(moveVec); }

            const float speed = 5.0f;
            PxVec3 displacement{ moveVec.x * speed * ecs->m_GetDeltaTime(), 0.0f, moveVec.z * speed * ecs->m_GetDeltaTime()};

            const float gravity = -9.81f;
            const float jumpStrength = 10.0f;

            charctrl->yVelocity += gravity * ecs->m_GetDeltaTime();

            if (Input::InputSystem::GetInstance()->IsKeyTriggered(keys::SPACE) && charctrl->isGrounded) {
                charctrl->yVelocity = jumpStrength;
                charctrl->isGrounded = false;
            }

            displacement.y = charctrl->yVelocity * ecs->m_GetDeltaTime();

            PxControllerCollisionFlags flags = ctrl->move(displacement, charctrl->minMoveDistance, ecs->m_GetDeltaTime(), PxControllerFilters());

            if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN) {
                charctrl->isGrounded = true;
                charctrl->yVelocity = 0.0f;
            }
            
            PxExtendedVec3 pos = ctrl->getPosition();
            glm::vec3 currPos{ static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z) };

            trans->LocalTransformation.position = currPos;

            charctrl->prevPos = currPos;
            charctrl->isChanged = true;
        }
    }
}