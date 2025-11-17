#include "Config/pch.h"
#include "CapsuleColliderSystem.h"
#include "Physics/PhysicsManager.h"

namespace {
	constexpr float MINSIZE = 0.01f;
}

namespace ecs {
	void CapsuleColliderSystem::Init() {
		onDeregister.Add([&](EntityID id) {
			CapsuleColliderComponent* capsule = m_ecs.GetComponent<CapsuleColliderComponent>(id);
			if (!capsule || !capsule->shape) { return; }

			RigidbodyComponent* rb = m_ecs.GetComponent<RigidbodyComponent>(id);
			if (rb) { capsule->shape = nullptr; }
			else {
				PxRigidStatic* actor = capsule->actor ? static_cast<PxRigidStatic*>(capsule->actor) : nullptr;
				PxShape* shape = static_cast<PxShape*>(capsule->shape);
				if (actor) { actor->detachShape(*shape); }
				shape->release();
				capsule->shape = nullptr;
			}
		});
	}

	void CapsuleColliderSystem::Update() {
		const auto& entities = m_entities.Data();
		for (EntityID id : entities) {
			TransformComponent* trans = m_ecs.GetComponent<TransformComponent>(id);
			NameComponent* name = m_ecs.GetComponent<NameComponent>(id);
			CapsuleColliderComponent* capsule = m_ecs.GetComponent<CapsuleColliderComponent>(id);

			if (name->hide || !capsule) { continue; }

			PxFilterData filter;
			filter.word0 = name->Layer;

			glm::vec3& scale = trans->LocalTransformation.scale;
			scale.x = glm::max(scale.x, MINSIZE);
			scale.y = glm::max(scale.y, MINSIZE);
			scale.z = glm::max(scale.z, MINSIZE);

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
				case CapsuleDirection::X: rot = PxQuat{ PxHalfPi, PxVec3{ 0.0f, 0.0f, 1.0f } }; break;
				case CapsuleDirection::Z: rot = PxQuat{ PxHalfPi, PxVec3{ 1.0f, 0.0f, 0.0f } }; break;
				default: break;
			}

			glm::vec3 scaledCenter = capsule->capsule.center * scale;
			shape->setLocalPose(PxTransform{ PxVec3{ scaledCenter.x, scaledCenter.y, scaledCenter.z }, rot });
			ToPhysxIsTrigger(shape, capsule->isTrigger);
			shape->setSimulationFilterData(filter);
			shape->setQueryFilterData(filter);
		}
	}
}