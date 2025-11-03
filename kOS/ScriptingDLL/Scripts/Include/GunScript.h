#pragma once

#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ScriptAdapter/TemplateSC.h"


class GunScript : public TemplateSC {
public:



	void Start() {

	}

	void Update() {

		if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {
			if (Input->IsKeyPressed(keys::R)) {
				tc->LocalTransformation.rotation.x = -15.f;
				std::cout << "Ayo gun\n";
			}
		}

	}

	REFLECTABLE(GunScript);
};