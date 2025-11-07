#pragma once
#include "ScriptAdapter/TemplateSC.h"

class EnemyManagerScript : public TemplateSC {
public:
	int enemyHealth;
	float enemyMovementSpeed;

	// REMOVE LATER
	bool isDead = false;

	void Start() override {

	}

	void Update() override {
		if (isDead) {
			ecsPtr->DeleteEntity(entity);
			return;
		}
	}

	REFLECTABLE(EnemyManagerScript, enemyHealth, enemyMovementSpeed);
};