#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "EnemyManagerScript.h"

class BulletLogic : public TemplateSC {
public:
	int bulletDamage = 1;
	float bulletSpeed = 5.f;
	glm::vec3 direction;

	float timeBeforeDeath = 2.5f;
	float currentTimer = 0.f;

	void Start() override {
		physicsPtr->GetEventCallback()->OnTriggerEnter.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
					enemyScript->enemyHealth -= bulletDamage;

					if (enemyScript->enemyHealth <= 0) {
						//ecsPtr->DeleteEntity(col.otherEntityID);
					}

					//ecsPtr->DeleteEntity(entity);
				}
			}
		});
	}

	void Update() override {
		if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {
			tc->LocalTransformation.position += direction * bulletSpeed * ecsPtr->m_GetDeltaTime();
		}

		if (currentTimer < timeBeforeDeath) {
			currentTimer += ecsPtr->m_GetDeltaTime();

			if (currentTimer >= timeBeforeDeath) {
				ecsPtr->DeleteEntity(entity);
			}
		}
	}

	//void Update() override {
	//	if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {
	//		glm::vec3 rotationInDegrees(tc->LocalTransformation.rotation);
	//		glm::vec3 rotationInRad = glm::radians(rotationInDegrees);
	//		glm::quat q = glm::quat(rotationInRad);

	//		glm::vec3 forward = q * glm::vec3(0.f, 0.f, 1.f);
	//		glm::vec3 right = q * glm::vec3(1.f, 0.f, 0.f);

	//		tc->LocalTransformation.position += forward * bulletSpeed * ecsPtr->m_GetDeltaTime();
	//	}
	//}


	REFLECTABLE(BulletLogic, bulletDamage, bulletSpeed)
};