#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "EnemyManagerScript.h"

class LightningAcidPowerupManagerScript : public TemplateSC {
public:
	float starfallDamage = 5.f;
	float starfallForce;
	float lingerTime;

	glm::vec3 direction;

	float currentTimer = 0.f;

	// REMOVE LATER
	bool isDead = false;

	void Start() override {
		physicsPtr->GetEventCallback()->OnTriggerEnter.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
					enemyScript->enemyHealth -= static_cast<int>(starfallDamage);

					if (enemyScript->enemyHealth <= 0) {
						//ecsPtr->DeleteEntity(col.otherEntityID);
						enemyScript->isDead = true;
					}

					isDead = true;
				}
			}
		});

		direction.y += 1.f;
		direction = glm::normalize(direction) * starfallForce;
		physicsPtr->AddForce(ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor, direction, ForceMode::Impulse);
	}

	void Update() override {
		if (currentTimer <= lingerTime) {
			currentTimer += ecsPtr->m_GetDeltaTime();

			if (currentTimer >= lingerTime || isDead) {
				ecsPtr->DeleteEntity(entity);
			}
		}
	}


	REFLECTABLE(LightningAcidPowerupManagerScript, starfallDamage, starfallForce, lingerTime)
};