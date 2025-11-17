#pragma once

#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ScriptAdapter/TemplateSC.h"


class EventColliderScript : public TemplateSC {
public:
	utility::GUID winUIObject;
	ecs::EntityID winUIObjectID;

	float timer;

	void Start() {
		winUIObjectID = ecsPtr->GetEntityIDFromGUID(winUIObject);
	}

	void Update() {
		timer += ecsPtr->m_GetDeltaTime();

		physicsPtr->GetEventCallback()->OnTriggerEnter(entity, [this](const physics::Collision& col) {
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Player" && timer > 10.f) {
				std::cout << "WHAT THE FUCK\n";
				ecsPtr->GetComponent<TextComponent>(winUIObjectID)->text = "LEVEL COMPLETE!";
			}
		});
	}

	REFLECTABLE(EventColliderScript, winUIObject, winUIObjectID, timer);
};