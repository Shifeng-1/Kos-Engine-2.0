#ifndef PHYSICSEVENTCALLBACK_H
#define PHYSICSEVENTCALLBACK_H

#include "Events/Delegate.h"
#include "PHYSX/PxPhysicsAPI.h"

using namespace physx;

namespace physics {
    struct ContactPoint {
        glm::vec3 point{ 0.0f };
        glm::vec3 normal{ 0.0f };
        float impulse{ 0.0f };
        float separation{ 0.0f }; 
    };

    struct Collision {
        unsigned int thisEntityID;
        unsigned int otherEntityID;
        std::vector<ContactPoint> contacts;
        glm::vec3 relativeVelocity{ 0.0f };
        float impulse{ 0.0f };  
    };
    
    struct CollisionPair {
        PxActor* collision;
        PxActor* other;
        bool operator==(const CollisionPair& rhs) const {
            return (collision == rhs.collision && other == rhs.other) || (collision == rhs.other && other == rhs.collision);
        }
    };

    struct TriggerPair {
        PxActor* trigger;
        PxActor* other;
        bool operator==(const TriggerPair& rhs) const {
            return trigger == rhs.trigger && other == rhs.other;
        }
    };

    struct CollisionPairHash {
        std::size_t operator()(const CollisionPair& collisionPair) const {
            auto h1 = std::hash<void*>()(collisionPair.collision);
            auto h2 = std::hash<void*>()(collisionPair.other);
            if (h1 > h2) std::swap(h1, h2);
            return h1 ^ (h2 << 1);
        }
    };

    struct TriggerPairHash {
        std::size_t operator()(const TriggerPair& triggerPair) const {
            return std::hash<void*>()(triggerPair.trigger) ^ (std::hash<void*>()(triggerPair.other) << 1);
        }
    };

    class PhysicsManager;

    class PhysicsEventCallback : public PxSimulationEventCallback {
    public:
        PhysicsEventCallback() : PxSimulationEventCallback() {}
        virtual ~PhysicsEventCallback() = default;

        std::unordered_map<unsigned int, Delegate<const Collision&>> CollisionEnterList;
        std::unordered_map<unsigned int, Delegate<const Collision&>> CollisionStayList;
        std::unordered_map<unsigned int, Delegate<const Collision&>> CollisionExitList;
        std::unordered_map<unsigned int, Delegate<const Collision&>> TriggerEnterList;
        std::unordered_map<unsigned int, Delegate<const Collision&>> TriggerStayList;
        std::unordered_map<unsigned int, Delegate<const Collision&>> TriggerExitList;

        void OnCollisionEnter(unsigned int id, Delegate<const Collision&>::Func f) { CollisionEnterList[id].Add(std::move(f)); }
        void OnCollisionStay(unsigned int id, Delegate<const Collision&>::Func f)  { CollisionStayList[id].Add(std::move(f)); }
        void OnCollisionExit(unsigned int id, Delegate<const Collision&>::Func f)  { CollisionExitList[id].Add(std::move(f)); }
        void OnTriggerEnter(unsigned int id, Delegate<const Collision&>::Func f)   { TriggerEnterList[id].Add(std::move(f)); }
        void OnTriggerStay(unsigned int id, Delegate<const Collision&>::Func f)    { TriggerStayList[id].Add(std::move(f)); }
        void OnTriggerExit(unsigned int id, Delegate<const Collision&>::Func f)    { TriggerExitList[id].Add(std::move(f)); }
        
        void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
        void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

        void ProcessCollisionStay();
        void ProcessTriggerStay();

        void onConstraintBreak(PxConstraintInfo*, PxU32) override {}
        void onWake(PxActor**, PxU32) override {}
        void onSleep(PxActor**, PxU32) override {}
        void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override {}

        std::unordered_set<CollisionPair, CollisionPairHash> m_activeCollisions;
        std::unordered_set<TriggerPair, TriggerPairHash> m_activeTriggers;
    private:
        glm::vec3 GetLinearVelocity(PxActor* actor) {
            if (auto rb = actor->is<PxRigidDynamic>()) {
                PxVec3 v = rb->getLinearVelocity();
                return glm::vec3{ v.x, v.y, v.z };
            }
            return glm::vec3{ 0.0f };
        }
    };
}

#endif