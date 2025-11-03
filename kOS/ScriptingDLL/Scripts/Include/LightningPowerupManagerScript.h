#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "EnemyManagerScript.h"

class LightningPowerupManagerScript : public TemplateSC {
public:
	int lightningDamage = 5;
	float lingerTime;
	float range = 10.f;

	float currentTimer = 0.f;

	void Start() override {
		physicsPtr->GetEventCallback()->OnTriggerEnter.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
					enemyScript->enemyHealth -= lightningDamage;

					if (enemyScript->enemyHealth <= 0) {
						//ecsPtr->DeleteEntity(col.otherEntityID);
					}
				}
			}
			});
	}

	void Update() override {
		if (currentTimer <= lingerTime) {
			currentTimer += ecsPtr->m_GetDeltaTime();

			if (currentTimer >= lingerTime) {
				ecsPtr->DeleteEntity(entity);
			}
		}
	}


	REFLECTABLE(LightningPowerupManagerScript, lightningDamage, lingerTime, range)
};