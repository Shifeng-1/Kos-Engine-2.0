#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "EnemyManagerScript.h"

class FireAcidPowerupManagerScript : public TemplateSC {
public:
	int flamethrowerDamage = 1;
	float lingerTime;

	float currentTimer = 0.f;

	// REMOVE ALTER
	bool isDead = false;

	void Start() override {
		physicsPtr->GetEventCallback()->OnTriggerEnter.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
					enemyScript->enemyHealth -= flamethrowerDamage;

					if (enemyScript->enemyHealth <= 0) {
						//ecsPtr->DeleteEntity(col.otherEntityID);
						enemyScript->isDead = true;
					}

					isDead = true;
				}
			}
			});
	}

	void Update() override {
		if (currentTimer <= lingerTime) {
			currentTimer += ecsPtr->m_GetDeltaTime();

			if (currentTimer >= lingerTime || isDead) {
				ecsPtr->DeleteEntity(entity);
			}
		}
	}


	REFLECTABLE(FireAcidPowerupManagerScript)
};