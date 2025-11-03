/********************************************************************/
/*!
\file       ColliderSystem.cpp
\author     Toh Yu Heng 2301294, Rayner Tan Wei Chen 2301449
\par        t.yuheng@digipen.edu, raynerweichen.tan@digipen.edu
\date       Oct 01 2025
\brief      Implements the ColliderSystem, responsible for creating and
            synchronizing static collision shapes (Box, Sphere, Capsule)
            within the ECS world using NVIDIA PhysX.

            This system:
            - Iterates over all entities with collider components
            - Generates corresponding PhysX PxShape instances if missing
            - Updates shape transforms based on entity TransformComponents
            - Creates and attaches PxRigidStatic actors for static colliders
            - Updates existing actor poses each frame

            Rigidbodies and CharacterControllers are skipped to avoid
            conflicts with dynamic or controlled physics objects.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "ColliderSystem.h"
#include "Physics/PhysicsManager.h"

using namespace physics;

namespace {
    constexpr float MINSIZE = 0.01f;
}

namespace ecs {
	void ColliderSystem::Init() {
        onDeregister.Add([&](EntityID id) {
            auto* box = m_ecs.GetComponent<BoxColliderComponent>(id);
            auto* sphere = m_ecs.GetComponent<SphereColliderComponent>(id);
            auto* capsule = m_ecs.GetComponent<CapsuleColliderComponent>(id);
            auto* rb = m_ecs.GetComponent<RigidbodyComponent>(id);
            if (rb) {
                if (box) { box->shape = nullptr; }
                if (sphere) { sphere->shape = nullptr; }
                if (capsule) { capsule->shape = nullptr; }
                return;
            }
            PxRigidStatic* actor = nullptr;
            if (box && box->actor) { actor = static_cast<PxRigidStatic*>(box->actor); }
            else if (sphere && sphere->actor) { actor = static_cast<PxRigidStatic*>(sphere->actor); }
            else if (capsule && capsule->actor) { actor = static_cast<PxRigidStatic*>(capsule->actor); }
            if (actor) {
                if (box && box->shape) {
                    PxShape* shape = static_cast<PxShape*>(box->shape);
                    actor->detachShape(*shape);
                    shape->release();
                    box->shape = nullptr;
                }
                if (sphere && sphere->shape) {
                    PxShape* shape = static_cast<PxShape*>(sphere->shape);
                    actor->detachShape(*shape);
                    shape->release();
                    sphere->shape = nullptr;
                }
                if (capsule && capsule->shape) {
                    PxShape* shape = static_cast<PxShape*>(capsule->shape);
                    actor->detachShape(*shape);
                    capsule->shape = nullptr;
                }
                PxScene* scene = actor->getScene();
                if (scene) { scene->removeActor(*actor); }
                actor->release();
                if (box) { box->actor = nullptr; }
                if (sphere) { sphere->actor = nullptr; }
                if (capsule) { capsule->actor = nullptr; }
            } else {
                if (box && box->shape) {
                    static_cast<PxShape*>(box->shape)->release();
                    box->shape = nullptr;
                }
                if (sphere && sphere->shape) {
                    static_cast<PxShape*>(sphere->shape)->release();
                    sphere->shape = nullptr;
                }
                if (capsule && capsule->shape) {
                    static_cast<PxShape*>(capsule->shape)->release();
                    capsule->shape = nullptr;
                }
            }
        });
    }

	void ColliderSystem::Update() {
		const auto& entities = m_entities.Data();

        for (EntityID id : entities) {
            TransformComponent* trans = m_ecs.GetComponent<TransformComponent>(id);
            NameComponent* name = m_ecs.GetComponent<NameComponent>(id);

            if (name->hide) { continue; }
            
            RigidbodyComponent* rb = m_ecs.GetComponent<RigidbodyComponent>(id);
            BoxColliderComponent* box = m_ecs.GetComponent<BoxColliderComponent>(id);
            CapsuleColliderComponent* capsule = m_ecs.GetComponent<CapsuleColliderComponent>(id);
            SphereColliderComponent* sphere = m_ecs.GetComponent<SphereColliderComponent>(id);

            if (!box && !sphere && !capsule) continue;

            PxFilterData filter;
            filter.word0 = name->Layer;

            glm::vec3& scale = trans->LocalTransformation.scale;
            scale.x = glm::max(scale.x, MINSIZE);
            scale.y = glm::max(scale.y, MINSIZE);
            scale.z = glm::max(scale.z, MINSIZE);

            if (box) {
                glm::vec3 halfExtents = box->box.size * scale * 0.5f;
                PxShape* shape = static_cast<PxShape*>(box->shape);
                PxBoxGeometry geometry{ halfExtents.x, halfExtents.y, halfExtents.z };
                if (!shape) {
                    shape = m_physicsManager.GetPhysics()->createShape(geometry, *m_physicsManager.GetDefaultMaterial(), true);
                    box->shape = shape;
                } 
                shape->setGeometry(geometry);
                glm::vec3 scaledCenter = box->box.center * scale;
                shape->setLocalPose(PxTransform{ PxVec3{ scaledCenter.x, scaledCenter.y, scaledCenter.z } });
                ToPhysxIsTrigger(shape, box->isTrigger);
                shape->setSimulationFilterData(filter);
                shape->setQueryFilterData(filter);
                box->box.bounds.center = trans->WorldTransformation.position + scaledCenter;
                box->box.bounds.extents = halfExtents;
                box->box.bounds.size = box->box.size * scale;
                box->box.bounds.min = box->box.bounds.center - box->box.bounds.extents;
                box->box.bounds.max = box->box.bounds.center + box->box.bounds.extents;
            }

            if (sphere) {
                float radius = sphere->sphere.radius * glm::max(scale.x, glm::max(scale.y, scale.z));
                PxShape* shape = static_cast<PxShape*>(sphere->shape);
                PxSphereGeometry geometry{ radius };
                if (!shape) {
                    shape = m_physicsManager.GetPhysics()->createShape(geometry, *m_physicsManager.GetDefaultMaterial(), true);
                    sphere->shape = shape;
                } 
                shape->setGeometry(geometry);
                glm::vec3 scaledCenter = sphere->sphere.center * scale;
                shape->setLocalPose(PxTransform{ PxVec3{ scaledCenter.x, scaledCenter.y, scaledCenter.z } });
                ToPhysxIsTrigger(shape, sphere->isTrigger);
                shape->setSimulationFilterData(filter);
                shape->setQueryFilterData(filter);
            }

            if (capsule) {
                float radius = capsule->capsule.radius * glm::max(scale.x, scale.z);
                float halfHeight = capsule->capsule.height * scale.y * 0.5f;
                PxShape* shape = static_cast<PxShape*>(capsule->shape);
                PxCapsuleGeometry geometry{ radius, halfHeight };
                if (!shape) {
                    shape = m_physicsManager.GetPhysics()->createShape(geometry, *m_physicsManager.GetDefaultMaterial(), true);
                    capsule->shape = shape;
                } 
                shape->setGeometry(geometry);
                PxQuat rot{ PxIdentity };
                switch (capsule->capsule.capsuleDirection) {
                    case CapsuleDirection::X:
                        rot = PxQuat{ PxHalfPi, PxVec3{ 0.0f, 0.0f, 1.0f } };
                        break;
                    case CapsuleDirection::Z:
                        rot = PxQuat{ PxHalfPi, PxVec3{ 1.0f, 0.0f, 0.0f } };
                        break;
                    default:
                        break;
                }
                glm::vec3 scaledCenter = capsule->capsule.center * scale;
                shape->setLocalPose(PxTransform{ PxVec3{ scaledCenter.x, scaledCenter.y, scaledCenter.z }, rot });
                ToPhysxIsTrigger(shape, capsule->isTrigger);
                shape->setSimulationFilterData(filter);
                shape->setQueryFilterData(filter);
            }

            if (!rb) {
                PxRigidStatic* actor = nullptr;
                if (box && box->actor) { actor = static_cast<PxRigidStatic*>(box->actor); }
                else if (sphere && sphere->actor) { actor = static_cast<PxRigidStatic*>(sphere->actor); }
                else if (capsule && capsule->actor) { actor = static_cast<PxRigidStatic*>(capsule->actor); }

                glm::vec3 pos = trans->WorldTransformation.position;
                glm::quat rot{ glm::radians(trans->WorldTransformation.rotation) };
                PxTransform pxTrans{ PxVec3{ pos.x, pos.y, pos.z }, PxQuat{ rot.x, rot.y, rot.z, rot.w } };

                if (!actor) {
                    actor = m_physicsManager.GetPhysics()->createRigidStatic(pxTrans);
                    actor->userData = reinterpret_cast<void*>(static_cast<uintptr_t>(id));
                    m_physicsManager.GetScene()->addActor(*actor);

                    if (box) { box->actor = actor; }
                    if (sphere) { sphere->actor = actor; }
                    if (capsule) { capsule->actor = actor; }
                } else {
                    actor->setGlobalPose(pxTrans);
                }

                auto HasShape = [](PxRigidActor* actor, PxShape* shape) -> bool {
                    PxU32 nbShapes = actor->getNbShapes();
                    std::vector<PxShape*> shapes(nbShapes);
                    actor->getShapes(shapes.data(), nbShapes);
                    for (PxU32 i = 0; i < nbShapes; ++i) {
                        if (shapes[i] == shape) { return true; }
                    }
                    return false;
                };

                if (box && box->shape) {
                    PxShape* shape = static_cast<PxShape*>(box->shape);
                    if (!HasShape(actor, shape)) { actor->attachShape(*shape); }
                }

                if (sphere && sphere->shape) {
                    PxShape* shape = static_cast<PxShape*>(sphere->shape);
                    if (!HasShape(actor, shape)) { actor->attachShape(*shape); }
                }

                if (capsule && capsule->shape) {
                    PxShape* shape = static_cast<PxShape*>(capsule->shape);
                    if (!HasShape(actor, shape)) { actor->attachShape(*shape); }
                }
            }
        }
	}
}