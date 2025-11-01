/********************************************************************/
/*!
\file       RigidbodySystem.cpp
\author     Toh Yu Heng 2301294
\par        t.yuheng@digipen.edu
\date       Oct 01 2025
\brief      Implements the RigidbodySystem which manages all dynamic
            rigidbody entities in the ECS using NVIDIA PhysX.

            Responsibilities:
            - Converts static colliders to dynamic rigidbodies when required
            - Initializes and configures PhysX PxRigidDynamic actors
            - Applies physics parameters such as mass, drag, gravity, etc.
            - Synchronizes ECS transforms with PhysX simulation results
            - Supports both kinematic and non-kinematic rigidbodies

            Automatically handles collider shape recreation when moving
            from static to dynamic and keeps ECS transforms up to date.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "RigidbodySystem.h"
#include "Physics/PhysicsManager.h"
#include "Physics/PhysxUtils.h"

using namespace physics;

namespace ecs {
    void RigidbodySystem::Init() {
        onDeregister.Add([](EntityID id) {
            ECS* ecs = ECS::GetInstance();
            auto* rb = ecs->GetComponent<RigidbodyComponent>(id);
            if (!rb || !rb->actor) { return; }
            PxRigidDynamic* actor = reinterpret_cast<PxRigidDynamic*>(rb->actor);
            PxU32 nbShapes = actor->getNbShapes();
            if (nbShapes > 0) {
                std::vector<PxShape*> shapes(nbShapes);
                actor->getShapes(shapes.data(), nbShapes);
                for (PxU32 i = 0; i < nbShapes; ++i) {
                    actor->detachShape(*shapes[i]);
                    shapes[i]->release();
                }
            }
            PxScene* scene = actor->getScene();
            if (scene) { scene->removeActor(*actor); }
            actor->release();
            rb->actor = nullptr;
            auto* box = ecs->GetComponent<BoxColliderComponent>(id);
            auto* sphere = ecs->GetComponent<SphereColliderComponent>(id);
            auto* capsule = ecs->GetComponent<CapsuleColliderComponent>(id);
            if (box) { 
                box->shape = nullptr; 
                box->actor = nullptr; 
            }
            if (sphere) { 
                sphere->shape = nullptr; 
                sphere->actor = nullptr; 
            }
            if (capsule) { 
                capsule->shape = nullptr; 
                capsule->actor = nullptr; 
            }
        });
    }

    void RigidbodySystem::Update() {
        ECS* ecs = ECS::GetInstance();
        const auto& entities = m_entities.Data();

        auto pm = PhysicsManager::GetInstance();

        for (EntityID id : entities) {
            NameComponent* name = ecs->GetComponent<NameComponent>(id);
            TransformComponent* trans = ecs->GetComponent<TransformComponent>(id);
            RigidbodyComponent* rb = ecs->GetComponent<RigidbodyComponent>(id);

            if ( name->hide) { continue; }

            PxRigidDynamic* actor = reinterpret_cast<PxRigidDynamic*>(rb->actor);

            if (!rb->actor) {
                glm::quat rot{ glm::radians(trans->WorldTransformation.rotation) };
                PxTransform pos{
                    PxVec3{ trans->WorldTransformation.position.x, trans->WorldTransformation.position.y, trans->WorldTransformation.position.z },
                    PxQuat{ rot.x, rot.y, rot.z, rot.w }
                };

                actor = pm->GetPhysics()->createRigidDynamic(pos);
                rb->actor = static_cast<void*>(actor);

                auto* box = ecs->GetComponent<BoxColliderComponent>(id);
                auto* sphere = ecs->GetComponent<SphereColliderComponent>(id);
                auto* capsule = ecs->GetComponent<CapsuleColliderComponent>(id);

                if (box && box->shape) {
                    PxShape* shape = static_cast<PxShape*>(box->shape);

                    if (box->actor && box->actor != rb->actor) {
                        PxRigidActor* prevActor = static_cast<PxRigidActor*>(box->actor);
                        prevActor->detachShape(*shape);
                        if (prevActor->getNbShapes() == 0) {
                            pm->GetScene()->removeActor(*prevActor);
                            prevActor->release();
                        }
                    }

                    actor->attachShape(*shape);
                    box->actor = rb->actor;
                }

                if (sphere && sphere->shape) {
                    PxShape* shape = static_cast<PxShape*>(sphere->shape);
                    if (sphere->actor && sphere->actor != rb->actor) {
                        PxRigidActor* prevActor = static_cast<PxRigidActor*>(sphere->actor);
                        prevActor->detachShape(*shape);
                        if (prevActor->getNbShapes() == 0) {
                            pm->GetScene()->removeActor(*prevActor);
                            prevActor->release();
                        }
                    }
                    actor->attachShape(*shape);
                    sphere->actor = rb->actor;
                }

                if (capsule && capsule->shape) {
                    PxShape* shape = static_cast<PxShape*>(capsule->shape);
                    if (capsule->actor && capsule->actor != rb->actor) {
                        PxRigidActor* prevActor = static_cast<PxRigidActor*>(capsule->actor);
                        prevActor->detachShape(*shape);
                        if (prevActor->getNbShapes() == 0) {
                            pm->GetScene()->removeActor(*prevActor);
                            prevActor->release();
                        }
                    }
                    actor->attachShape(*shape);
                    capsule->actor = rb->actor;
                }
                PxRigidBodyExt::updateMassAndInertia(*actor, rb->mass);
                actor->userData = reinterpret_cast<void*>(static_cast<uintptr_t>(id));
                pm->GetScene()->addActor(*actor);
            } else {
                actor->setMass(rb->mass);
                actor->setLinearDamping(rb->drag);
                actor->setAngularDamping(rb->angularDrag);
                actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, rb->isKinematic);
                actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !rb->useGravity);
                ToPhysXContraints(actor, rb->constraints);
                ToPhysxCollisionDetectionMode(actor, rb->collisionDetection);
                ToPhysxInterpolation(actor, rb->interpolation);
            }
        }
    }
}