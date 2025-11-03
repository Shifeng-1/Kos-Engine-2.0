#pragma once
#include "ScriptAdapter/TemplateSC.h"

class PowerupManagerScript : public TemplateSC {
public:
	std::string powerupType;

	void Start() override {

	}

	void Update() override {
		if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {

		}
	}

	REFLECTABLE(PowerupManagerScript, powerupType);
};