#include "Config/pch.h"
#include "PhysicsSystem.h"
#include "Physics/PhysicsManager.h"

using namespace physics;

namespace ecs {
    void PhysicsSystem::Init() {
        m_physicsManager.Init();
    }

    void PhysicsSystem::Update() {
        const auto& entities = m_entities.Data();

        for (EntityID id : entities) {
            auto* rb = m_ecs.GetComponent<RigidbodyComponent>(id);
            auto* trans = m_ecs.GetComponent<TransformComponent>(id);
            auto* name = m_ecs.GetComponent<NameComponent>(id);

            if (name->hide || !rb->actor) { continue; }

            PxRigidDynamic* actor = static_cast<PxRigidDynamic*>(rb->actor);

            glm::vec3 pos = trans->WorldTransformation.position;
            glm::quat rot = glm::quat(glm::radians(trans->WorldTransformation.rotation));
            PxTransform pxTrans{ PxVec3{ pos.x, pos.y, pos.z }, PxQuat{ rot.x, rot.y, rot.z, rot.w } };

            if (rb->isKinematic) { actor->setKinematicTarget(pxTrans); }
            else { 
                actor->setGlobalPose(pxTrans); 
                actor->setLinearVelocity(PxVec3{ rb->velocity.x, rb->velocity.y, rb->velocity.z });
                actor->setAngularVelocity(PxVec3{ rb->angularVelocity.x, rb->angularVelocity.y, rb->angularVelocity.z });
            }
        }

        m_physicsManager.Update(m_ecs.m_GetDeltaTime());

        for (EntityID id : entities) {
            NameComponent* name = m_ecs.GetComponent<NameComponent>(id);
            TransformComponent* trans = m_ecs.GetComponent<TransformComponent>(id);
            RigidbodyComponent* rb = m_ecs.GetComponent<RigidbodyComponent>(id);

            if (name->hide || !rb->actor) { continue; }

            PxRigidDynamic* actor = static_cast<PxRigidDynamic*>(rb->actor);

            if (!rb->isKinematic) {
                PxVec3 vel = actor->getLinearVelocity();
                PxVec3 aVel = actor->getAngularVelocity();
                rb->velocity = glm::vec3{ vel.x, vel.y, vel.z };
                rb->angularVelocity = glm::vec3{ aVel.x, aVel.y, aVel.z };
                rb->speed = glm::length(rb->velocity);
            } else {
                rb->velocity = glm::vec3{ 0.0f };
                rb->angularVelocity = glm::vec3{ 0.0f };
            }
        }

        SyncTransforms();
    }

    void PhysicsSystem::SyncTransforms() {
        const auto& entities = m_entities.Data();

        for (EntityID id : entities) {
            NameComponent* name = m_ecs.GetComponent<NameComponent>(id);
            TransformComponent* trans = m_ecs.GetComponent<TransformComponent>(id);
            RigidbodyComponent* rb = m_ecs.GetComponent<RigidbodyComponent>(id);

            if (name->hide || !rb->actor) { continue; }

            PxRigidDynamic* actor = static_cast<PxRigidDynamic*>(rb->actor);

            if (rb->isKinematic) { continue; }

            PxTransform pxTrans = actor->getGlobalPose();
            glm::vec3 currPos{ pxTrans.p.x, pxTrans.p.y, pxTrans.p.z };
            glm::quat currRot{ pxTrans.q.w, pxTrans.q.x, pxTrans.q.y, pxTrans.q.z };

            glm::vec3 nextPos;
            glm::quat nextRot;

            switch (rb->interpolation) {
            case InterpolationMode::None:
                nextPos = currPos;
                nextRot = currRot;
                break;
            case InterpolationMode::Interpolate:
                nextPos = glm::mix(rb->prevPosition, currPos, m_physicsManager.InterpolationAlpha());
                nextRot = glm::slerp(glm::quat{ glm::radians(rb->prevRotation) }, currRot, m_physicsManager.InterpolationAlpha());
                break;
            case InterpolationMode::Extrapolate:
                if (actor->getRigidBodyFlags().isSet(PxRigidBodyFlag::eENABLE_POSE_INTEGRATION_PREVIEW)) {
                    PxVec3 vel = actor->getLinearVelocity();
                    PxVec3 aVel = actor->getAngularVelocity();
                    float extrapolationTime = m_physicsManager.InterpolationAlpha() * m_physicsManager.FixedDeltaTime();
                    nextPos = currPos + glm::vec3{ vel.x, vel.y, vel.z } * extrapolationTime;
                    
                    glm::vec3 a{ aVel.x, aVel.y, aVel.z };
                    float angularSpeed = glm::length(a);
                    if (angularSpeed > 0.001f) {
                        glm::vec3 axis = glm::normalize(a);
                        glm::quat delta = glm::angleAxis(angularSpeed * extrapolationTime, axis);
                        nextRot = delta * currRot;
                    } else { nextRot = currRot; }
                } else {
                    nextPos = currPos;
                    nextRot = currRot;
                }
                break;
            }

            TransformSystem::SetImmediateWorldPosition(m_ecs, trans, glm::vec3{ nextPos.x, nextPos.y, nextPos.z });
            TransformSystem::SetImmediateWorldRotation(m_ecs, trans, glm::degrees(glm::eulerAngles(nextRot)));

            rb->prevPosition = currPos;
            rb->prevRotation = glm::degrees(glm::eulerAngles(currRot));
        }
    }
}