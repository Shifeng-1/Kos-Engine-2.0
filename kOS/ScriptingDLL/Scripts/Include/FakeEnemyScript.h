#pragma once

#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ScriptAdapter/TemplateSC.h"
#include "FakePlayerHealthScript.h"

inline float Lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

class FakeEnemyScript : public TemplateSC {
public:
	float enemySpeed;
	int enemyDamage;

	float damageInterval;
	float timer;

	utility::GUID playerObject;
	ecs::EntityID playerObjectID;

	void Start() {
		playerObjectID = ecsPtr->GetEntityIDFromGUID(playerObject);
	}

	void Update() {
		// For dmg interval
		timer += ecsPtr->m_GetDeltaTime();

		// Lerp factor calc
		float t = enemySpeed * ecsPtr->m_GetDeltaTime();
		if (t > 1.0f) t = 1.0f;

		// Movement
		if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {
			// Literally lerp towards player pos, no pathfinding
			ecs::TransformComponent* ptc = ecsPtr->GetComponent<TransformComponent>(playerObjectID);
			tc->LocalTransformation.position.x = Lerp(tc->LocalTransformation.position.x, ptc->LocalTransformation.position.x, t);
			tc->LocalTransformation.position.z = Lerp(tc->LocalTransformation.position.z, ptc->LocalTransformation.position.z, t);

			// Freeze rotation so he doesn't become jesus
			tc->LocalTransformation.rotation.x = 0.0f; // Freeze X rotation
			tc->LocalTransformation.rotation.z = 0.0f; // Freeze Z rotation
		}

		// Damage
		//physicsPtr->GetEventCallback()->OnTriggerEnter.Add([this](const physics::Collision& col) {
		//	if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Player") {
		//		if (auto* healthScript = ecsPtr->GetComponent<FakePlayerHealthScript>(col.otherEntityID)) {
		//			if (timer >= damageInterval) {
		//				healthScript->healthVal -= enemyDamage;
		//				timer = 0.0f;
		//				std::cout << "Alamak kena hit ccb\n";
		//			}

		//			// Dead
		//			if (healthScript->healthVal <= 0) {
		//				// Add stuff here if free
		//				healthScript->healthVal = 0;
		//			}
		//		}
		//	}
		//});

		//physicsPtr->GetEventCallback()->OnTriggerExit.Add([this](const physics::Collision& col) {
		//	if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Player") {
		//		physicsPtr->GetEventCallback()->OnTriggerEnter.Clear();
		//	}
		//	});
	}

	REFLECTABLE(FakeEnemyScript, enemySpeed, enemyDamage, damageInterval, playerObject, playerObjectID);
};