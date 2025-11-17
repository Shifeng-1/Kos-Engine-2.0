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

	utility::GUID enemyDeathSfxGUID; //gonna remove this when stuff are fixed

	// FOR NOW UNTIL DELETE GETS IMPLEMENTED
	bool isDead = false;

	void Start() override {
		// Raymonds Stuff
		//physicsPtr->GetEventCallback()->OnTriggerEnter.Add([this](const physics::Collision& col) {
		//	//if (col.thisEntityID != this->entity) { return; }
		//	if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
		//		if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
		//			enemyScript->enemyHealth -= bulletDamage;

		//			if (enemyScript->enemyHealth <= 0) {
		//				//ecsPtr->DeleteEntity(col.otherEntityID);
		//				enemyScript->isDead = true;
		//			}

		//			isDead = true;
		//			//ecsPtr->DeleteEntity(entity);
		//			//return;
		//		}
		//	}
		//});

		//physicsPtr->GetEventCallback()->OnTriggerExit.Add([this](const physics::Collision& col) {
		//	if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
		//		physicsPtr->GetEventCallback()->OnTriggerEnter.Clear();
		//	}
		//	});

		// Rudimentary bullets
		physicsPtr->GetEventCallback()->OnTriggerEnter(entity, [this](const physics::Collision& col) {
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* ac = ecsPtr->GetComponent<ecs::AudioComponent>(entity)) {
					for (auto& af : ac->audioFiles) {
						if (af.audioGUID == enemyDeathSfxGUID && af.isSFX) {
							af.requestPlay = true;
							break;
						}
					}
				}
				ecsPtr->DeleteEntity(col.otherEntityID);
			}
		});
	}

	void Update() override {
		if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {
			tc->LocalTransformation.position += direction * bulletSpeed * ecsPtr->m_GetDeltaTime();
		}

		if (currentTimer < timeBeforeDeath) {
			currentTimer += ecsPtr->m_GetDeltaTime();

			//if (currentTimer >= timeBeforeDeath) {
			//	ecsPtr->DeleteEntity(entity);
			//}
			if (currentTimer >= timeBeforeDeath || isDead) {
				ecsPtr->DeleteEntity(entity);

			}
		}
	}

	REFLECTABLE(BulletLogic, bulletDamage, bulletSpeed, enemyDeathSfxGUID)
};