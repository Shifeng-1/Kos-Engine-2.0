#pragma once

#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ScriptAdapter/TemplateSC.h"


class FakePlayerHealthScript : public TemplateSC {
public:
	int healthVal;

	void Start() {
	}

	void Update() {
		if (auto* tc = ecsPtr->GetComponent<ecs::TextComponent>(entity)) {
			tc->text = std::to_string(healthVal);
		}
	}

	REFLECTABLE(FakePlayerHealthScript, healthVal);
};