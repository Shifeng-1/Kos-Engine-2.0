#include "Config/pch.h"
#include "BoxColliderSystem.h"
#include "Physics/PhysicsManager.h"

namespace {
	constexpr float MINSIZE = 0.01f;
}

namespace ecs {
	void BoxColliderSystem::Init() {
		onDeregister.Add([&](EntityID id) {
			BoxColliderComponent* box = m_ecs.GetComponent<BoxColliderComponent>(id);
			if (!box || !box->shape) { return; }

			RigidbodyComponent* rb = m_ecs.GetComponent<RigidbodyComponent>(id);
			if (rb) { box->shape = nullptr; }
			else {
				PxRigidStatic* actor = box->actor ? static_cast<PxRigidStatic*>(box->actor) : nullptr;
				PxShape* shape = static_cast<PxShape*>(box->shape);
				if (actor) { actor->detachShape(*shape); }
				shape->release();
				box->shape = nullptr;
			}
		});
	}

	void BoxColliderSystem::Update() {
		const auto& entities = m_entities.Data();
		for (EntityID id : entities) {
			TransformComponent* trans = m_ecs.GetComponent<TransformComponent>(id);
			NameComponent* name = m_ecs.GetComponent<NameComponent>(id);
			BoxColliderComponent* box = m_ecs.GetComponent<BoxColliderComponent>(id);

			if (name->hide || !box) { continue; }

			PxFilterData filter;
			filter.word0 = name->Layer;

			glm::vec3& scale = trans->LocalTransformation.scale;
			scale.x = glm::max(scale.x, MINSIZE);
			scale.y = glm::max(scale.y, MINSIZE);
			scale.z = glm::max(scale.z, MINSIZE);

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
	}
}