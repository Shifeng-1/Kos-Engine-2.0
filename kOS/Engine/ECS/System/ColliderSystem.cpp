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
        onDeregister.Add([](EntityID id) {
            ECS* ecs = ECS::GetInstance();
            auto* box = ecs->GetComponent<BoxColliderComponent>(id);
            auto* sphere = ecs->GetComponent<SphereColliderComponent>(id);
            auto* capsule = ecs->GetComponent<CapsuleColliderComponent>(id);
            auto* rb = ecs->GetComponent<RigidbodyComponent>(id);
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
		ECS* ecs = ECS::GetInstance();
		const auto& entities = m_entities.Data();

        auto pm = PhysicsManager::GetInstance();

        for (EntityID id : entities) {
            TransformComponent* trans = ecs->GetComponent<TransformComponent>(id);
            NameComponent* name = ecs->GetComponent<NameComponent>(id);

            if (name->hide) { continue; }
            
            RigidbodyComponent* rb = ecs->GetComponent<RigidbodyComponent>(id);
            BoxColliderComponent* box = ecs->GetComponent<BoxColliderComponent>(id);
            CapsuleColliderComponent* capsule = ecs->GetComponent<CapsuleColliderComponent>(id);
            SphereColliderComponent* sphere = ecs->GetComponent<SphereColliderComponent>(id);

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
                    shape = pm->GetPhysics()->createShape(geometry, *pm->GetDefaultMaterial(), true);
                    box->shape = shape;
                } else {
                    shape->setGeometry(geometry);
                }
                shape->setLocalPose(PxTransform{ PxVec3{ box->box.center.x, box->box.center.y, box->box.center.z } });
                ToPhysxIsTrigger(shape, box->isTrigger);
                shape->setSimulationFilterData(filter);
                shape->setQueryFilterData(filter);
                box->box.bounds.center = trans->WorldTransformation.position + box->box.center * scale;
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
                    shape = pm->GetPhysics()->createShape(geometry, *pm->GetDefaultMaterial(), true);
                    sphere->shape = shape;
                } else {
                    shape->setGeometry(geometry);
                }
                shape->setLocalPose(PxTransform{ PxVec3{ sphere->sphere.center.x, sphere->sphere.center.y, sphere->sphere.center.z } });
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
                    shape = pm->GetPhysics()->createShape(geometry, *pm->GetDefaultMaterial(), true);
                    capsule->shape = shape;
                } else {
                    shape->setGeometry(geometry);
                }
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
                shape->setLocalPose(PxTransform{ PxVec3{ capsule->capsule.center.x, capsule->capsule.center.y, capsule->capsule.center.z }, rot });
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
                    actor = pm->GetPhysics()->createRigidStatic(pxTrans);
                    actor->userData = reinterpret_cast<void*>(static_cast<uintptr_t>(id));
                    pm->GetScene()->addActor(*actor);

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