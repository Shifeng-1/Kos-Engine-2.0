#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "EnemyManagerScript.h"

class AcidPowerupManagerScript : public TemplateSC {
public:
	int acidDamage = 5;
	float lingerTime;
	float growthRate;
	float acidBlastSpeed;

	float currentTimer = 0.f;
	glm::vec3 direction;

	void Start() override {
		physicsPtr->GetEventCallback()->OnTriggerEnter.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
					enemyScript->enemyHealth -= acidDamage;

					if (enemyScript->enemyHealth <= 0) {
						//ecsPtr->DeleteEntity(col.otherEntityID);
					}
				}
			}
			});
	}

	void Update() override {
		if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {
			tc->LocalTransformation.position += direction * acidBlastSpeed * ecsPtr->m_GetDeltaTime();
		}

		if (currentTimer <= lingerTime) {
			currentTimer += ecsPtr->m_GetDeltaTime();
			ecsPtr->GetComponent<TransformComponent>(entity)->LocalTransformation.scale *= growthRate;

			if (currentTimer >= lingerTime) {
				ecsPtr->DeleteEntity(entity);
			}
		}
	}


	REFLECTABLE(AcidPowerupManagerScript, acidDamage, lingerTime, growthRate, acidBlastSpeed)
};