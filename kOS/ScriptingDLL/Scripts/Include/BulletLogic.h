#include "ScriptAdapter/TemplateSC.h"

class BulletLogic : public TemplateSC {
public:
	float bulletSpeed = 5.f;
	glm::vec3 direction;

	void Start() override {
		physicsPtr->OnTriggerEnter.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				std::cout << "Collided with Entity: " << col.otherEntityID << std::endl;
			}
		});
	}

	void Update() override {
		if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {
			glm::vec3 rotationInDegrees(tc->LocalTransformation.rotation);
			glm::vec3 rotationInRad = glm::radians(rotationInDegrees);
			glm::quat q = glm::quat(rotationInRad);

			glm::vec3 forward = q * glm::vec3(0.f, 0.f, 1.f);
			glm::vec3 right = q * glm::vec3(1.f, 0.f, 0.f);

			tc->LocalTransformation.position += forward * bulletSpeed * ecsPtr->m_GetDeltaTime();
		}
	}


	REFLECTABLE(BulletLogic, bulletSpeed, direction)
};