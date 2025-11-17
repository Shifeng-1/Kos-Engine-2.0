#include "Config/pch.h"
#include "SphereColliderSystem.h"
#include "Physics/PhysicsManager.h"

namespace {
	constexpr float MINSIZE = 0.01f;
}

namespace ecs {
	void SphereColliderSystem::Init() {
		onDeregister.Add([&](EntityID id) {
			SphereColliderComponent* sphere = m_ecs.GetComponent<SphereColliderComponent>(id);
			if (!sphere || !sphere->shape) { return; }

			RigidbodyComponent* rb = m_ecs.GetComponent<RigidbodyComponent>(id);
			if (rb) { sphere->shape = nullptr; }
			else {
				PxRigidStatic* actor = sphere->actor ? static_cast<PxRigidStatic*>(sphere->actor) : nullptr;
				PxShape* shape = static_cast<PxShape*>(sphere->shape);
				if (actor) { actor->detachShape(*shape); }
				shape->release();
				sphere->shape = nullptr;
			}
		});
	}

	void SphereColliderSystem::Update() {
		const auto& entities = m_entities.Data();
		for (EntityID id : entities) {
			TransformComponent* trans = m_ecs.GetComponent<TransformComponent>(id);
			NameComponent* name = m_ecs.GetComponent<NameComponent>(id);
			SphereColliderComponent* sphere = m_ecs.GetComponent<SphereColliderComponent>(id);

			if (name->hide || !sphere) { continue; }

			PxFilterData filter;
			filter.word0 = name->Layer;

			glm::vec3& scale = trans->LocalTransformation.scale;
			scale.x = glm::max(scale.x, MINSIZE);
			scale.y = glm::max(scale.y, MINSIZE);
			scale.z = glm::max(scale.z, MINSIZE);

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
	}
}