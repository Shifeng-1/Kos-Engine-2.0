#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "EnemyManagerScript.h"

class FirePowerupManagerScript : public TemplateSC {
public:
	float fireballSpeed = 10.f;
	int fireballDamage = 5;
	glm::vec3 direction;

	void Start() override {
		physicsPtr->GetEventCallback()->OnTriggerEnter.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
					enemyScript->enemyHealth -= fireballDamage;

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
			tc->LocalTransformation.position += direction * fireballSpeed * ecsPtr->m_GetDeltaTime();
		}
	}


	REFLECTABLE(FirePowerupManagerScript, fireballSpeed, fireballDamage)
};