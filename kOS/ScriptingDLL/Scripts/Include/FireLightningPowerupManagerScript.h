#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "EnemyManagerScript.h"

class FireLightningPowerupManagerScript : public TemplateSC {
public:
	int groundSpikesDamage = 5;
	float lingerTime;

	float currentTimer;

	void Start() override {
		physicsPtr->GetEventCallback()->OnTriggerEnter.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
					enemyScript->enemyHealth -= groundSpikesDamage;

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

			if (currentTimer <= lingerTime / 5.f) {
				for (int i = 0; i < ecsPtr->GetComponent<TransformComponent>(entity)->m_childID.size(); ++i) {
					if (auto* childTransform = ecsPtr->GetComponent<TransformComponent>(ecsPtr->GetComponent<TransformComponent>(entity)->m_childID[i])) {
						childTransform->LocalTransformation.position.y += ecsPtr->m_GetDeltaTime() * 45.f;
					}
				}
			}

			if (currentTimer >= lingerTime) {
				ecsPtr->DeleteEntity(entity);
			}
		}
	}


	REFLECTABLE(FireLightningPowerupManagerScript, groundSpikesDamage, lingerTime)
};