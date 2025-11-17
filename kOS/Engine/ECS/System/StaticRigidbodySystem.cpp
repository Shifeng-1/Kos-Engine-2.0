#include "Config/pch.h"
#include "StaticRigidbodySystem.h"
#include "Physics/PhysicsManager.h"

namespace ecs {
	void StaticRigidbodySystem::Init() {
		onDeregister.Add([&](EntityID id) {
            RigidbodyComponent* rb = m_ecs.GetComponent<RigidbodyComponent>(id);
            if (rb) { return; }

            BoxColliderComponent* box = m_ecs.GetComponent<BoxColliderComponent>(id);
            SphereColliderComponent* sphere = m_ecs.GetComponent<SphereColliderComponent>(id);
            CapsuleColliderComponent* capsule = m_ecs.GetComponent<CapsuleColliderComponent>(id);

            PxRigidStatic* actor = nullptr;
            if (box && box->actor) { actor = static_cast<PxRigidStatic*>(box->actor); }
            else if (sphere && sphere->actor) { actor = static_cast<PxRigidStatic*>(sphere->actor); }
            else if (capsule && capsule->actor) { actor = static_cast<PxRigidStatic*>(capsule->actor); }

            if (actor) {
                PxScene* scene = actor->getScene();
                if (scene) { scene->removeActor(*actor); }
                actor->release();
                if (box) { box->actor = nullptr; }
                if (sphere) { sphere->actor = nullptr; }
                if (capsule) { capsule->actor = nullptr; }
            }
		});
	}

	void StaticRigidbodySystem::Update() {
        const auto& entities = m_entities.Data();

        for (EntityID id : entities) {
            TransformComponent* trans = m_ecs.GetComponent<TransformComponent>(id);
            NameComponent* name = m_ecs.GetComponent<NameComponent>(id);
            RigidbodyComponent* rb = m_ecs.GetComponent<RigidbodyComponent>(id);

            if (name->hide || rb) { continue; }

            BoxColliderComponent* box = m_ecs.GetComponent<BoxColliderComponent>(id);
            SphereColliderComponent* sphere = m_ecs.GetComponent<SphereColliderComponent>(id);
            CapsuleColliderComponent* capsule = m_ecs.GetComponent<CapsuleColliderComponent>(id);

            if (!box && !sphere && !capsule) { continue; }

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

            if (box && box->shape) {
                PxShape* shape = static_cast<PxShape*>(box->shape);
                if (!IsShapeAttachedToActor(actor, shape)) { actor->attachShape(*shape); }
            }

            if (sphere && sphere->shape) {
                PxShape* shape = static_cast<PxShape*>(sphere->shape);
                if (!IsShapeAttachedToActor(actor, shape)) { actor->attachShape(*shape); }
            }

            if (capsule && capsule->shape) {
                PxShape* shape = static_cast<PxShape*>(capsule->shape);
                if (!IsShapeAttachedToActor(actor, shape)) { actor->attachShape(*shape); }
            }
        }
	}
    
    bool StaticRigidbodySystem::IsShapeAttachedToActor(PxRigidActor* actor, PxShape* shape) {
        if (!shape) { return false; }
        PxU32 nbShapes = actor->getNbShapes();
        std::vector<PxShape*> shapes(nbShapes);
        actor->getShapes(shapes.data(), nbShapes);
        for (PxU32 i = 0; i < nbShapes; ++i) { 
            if (shapes[i] == shape) {
                return true; 
            } 
        }
        return false;
    }
}