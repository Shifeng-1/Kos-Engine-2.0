#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "EnemyManagerScript.h"

class LightningAcidPowerupManagerScript : public TemplateSC {
public:
	float starfallDamage = 5.f;
	float starfallForce;

	glm::vec3 direction;

	void Start() override {
		physicsPtr->GetEventCallback()->OnTriggerEnter.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
					enemyScript->enemyHealth -= starfallDamage;

					if (enemyScript->enemyHealth <= 0) {
						//ecsPtr->DeleteEntity(col.otherEntityID);
					}
				}
			}
		});

		direction.y *= 2.f;
		direction = glm::normalize(direction) * starfallForce;
		physicsPtr->AddForce(ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor, direction, ForceMode::Impulse);
	}

	void Update() override {

	}


	REFLECTABLE(LightningAcidPowerupManagerScript, starfallDamage, starfallForce)
};