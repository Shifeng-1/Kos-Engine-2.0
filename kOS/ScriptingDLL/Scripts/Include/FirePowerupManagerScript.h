#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "EnemyManagerScript.h"

class FirePowerupManagerScript : public TemplateSC {
public:
	float fireballSpeed = 10.f;
	int fireballDamage = 5;

	void Start() override {
		physicsPtr->OnTriggerEnter.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
					enemyScript->enemyHealth -= fireballDamage;

					if (enemyScript->enemyHealth <= 0) {
						//ecsPtr->DeleteEntity(col.otherEntityID);
					}
				}
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

			tc->LocalTransformation.position += forward * fireballSpeed * ecsPtr->m_GetDeltaTime();
		}
	}


	REFLECTABLE(FirePowerupManagerScript, fireballSpeed, fireballDamage)
};