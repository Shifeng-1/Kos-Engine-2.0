#include "Config/pch.h"
#include "Physics/PhysicsEventCallback.h"

namespace physics {
    void PhysicsEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
        if (pairHeader.flags & (PxContactPairHeaderFlag::eREMOVED_ACTOR_0 | PxContactPairHeaderFlag::eREMOVED_ACTOR_1)) { return; }

        for (PxU32 i = 0; i < nbPairs; i++) {
            const PxContactPair& cp = pairs[i];

            if (cp.flags & (PxContactPairFlag::eREMOVED_SHAPE_0 | PxContactPairFlag::eREMOVED_SHAPE_1)) { continue; }

            CollisionPair pair{ pairHeader.actors[0], pairHeader.actors[1] };

            unsigned int entityA = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(pairHeader.actors[0]->userData));
            unsigned int entityB = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(pairHeader.actors[1]->userData));

            Collision collisionA{}, collisionB{};

            collisionA.thisEntityID = entityA;
            collisionA.otherEntityID = entityB;

            collisionB.thisEntityID = entityB;
            collisionB.otherEntityID = entityA;

            PxContactPairPoint contacts[16];
            PxU32 contactCount = cp.extractContacts(contacts, 16);

            collisionA.contacts.reserve(contactCount);
            collisionB.contacts.reserve(contactCount);

            glm::vec3 totalImpulse{ 0.0f };

            for (PxU32 j = 0; j < contactCount; ++j) {
                const PxContactPairPoint& p = contacts[j];

                ContactPoint ptA;

                ptA.point = { p.position.x, p.position.y, p.position.z };
                ptA.normal = { p.normal.x, p.normal.y, p.normal.z };
                ptA.impulse = p.impulse.magnitude();
                ptA.separation = p.separation;

                collisionA.contacts.push_back(ptA);

                ContactPoint ptB = ptA;
                ptB.normal = -ptA.normal;

                collisionB.contacts.push_back(ptB);

                totalImpulse += glm::vec3{ p.impulse.x, p.impulse.y, p.impulse.z };
            }

            collisionA.impulse = glm::length(totalImpulse);
            collisionB.impulse = glm::length(totalImpulse);

            glm::vec3 velA = GetLinearVelocity(pairHeader.actors[0]);
            glm::vec3 velB = GetLinearVelocity(pairHeader.actors[1]);
            glm::vec3 relVel = velB - velA;

            collisionA.relativeVelocity = relVel;
            collisionB.relativeVelocity = -relVel;

            if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                if (CollisionEnterList.contains(entityA)) { CollisionEnterList[entityA].Invoke(collisionA); }
                if (CollisionEnterList.contains(entityB)) { CollisionEnterList[entityB].Invoke(collisionB); }
                m_activeCollisions.insert(pair);
            }

            if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS) {
                m_activeCollisions.insert(pair);
            }

            if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST) {
                if (CollisionExitList.contains(entityA)) { CollisionExitList[entityA].Invoke(collisionA); }
                if (CollisionExitList.contains(entityB)) { CollisionExitList[entityB].Invoke(collisionB); }
                m_activeCollisions.erase(pair);
            }
        }
    }

    void PhysicsEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count) { 
        for (PxU32 i = 0; i < count; ++i) {
            const PxTriggerPair& tp = pairs[i];

            TriggerPair pair{ tp.triggerActor, tp.otherActor };

            if (tp.flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER)) { 
                m_activeTriggers.erase(pair);
                continue; 
            }

            unsigned int entityA = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(tp.triggerActor->userData));
            unsigned int entityB = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(tp.otherActor->userData));

            Collision triggerA{}, triggerB{};

            triggerA.thisEntityID = entityA;
            triggerA.otherEntityID = entityB;

            triggerB.thisEntityID = entityB;
            triggerB.otherEntityID = entityA;

            if (tp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                if (TriggerEnterList.contains(entityA)) { TriggerEnterList[entityA].Invoke(triggerA); }
                if (TriggerEnterList.contains(entityB)) { TriggerEnterList[entityB].Invoke(triggerB); }
                m_activeTriggers.insert(pair);
            }

            else if (tp.status & PxPairFlag::eNOTIFY_TOUCH_LOST) {
                if (TriggerExitList.contains(entityA)) { TriggerExitList[entityA].Invoke(triggerA); }
                if (TriggerExitList.contains(entityB)) { TriggerExitList[entityB].Invoke(triggerB); }
                m_activeTriggers.erase(pair);
            }
        }
    }

    void PhysicsEventCallback::ProcessCollisionStay() {
        for (const auto& pair : m_activeCollisions) {
            unsigned int entityA = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(pair.collision->userData));
            unsigned int entityB = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(pair.other->userData));

            Collision collisionA{}, collisionB{};

            collisionA.thisEntityID = entityA;
            collisionA.otherEntityID = entityB;

            collisionB.thisEntityID = entityB;
            collisionB.otherEntityID = entityA;

            if (CollisionStayList.contains(entityA)) { CollisionStayList[entityA].Invoke(collisionA); }
            if (CollisionStayList.contains(entityB)) { CollisionStayList[entityB].Invoke(collisionB); }
        }
    }

    void PhysicsEventCallback::ProcessTriggerStay() {
        for (const auto& pair : m_activeTriggers) {
            unsigned int entityA = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(pair.trigger->userData));
            unsigned int entityB = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(pair.other->userData));

            Collision triggerA{};
            Collision triggerB{};

            triggerA.thisEntityID = entityA;
            triggerA.otherEntityID = entityB;

            triggerB.thisEntityID = entityB;
            triggerB.otherEntityID = entityA;

            if (TriggerStayList.contains(entityA)) { TriggerStayList[entityA].Invoke(triggerA); }
            if (TriggerStayList.contains(entityB)) { TriggerStayList[entityB].Invoke(triggerB); }
        }
    }
}