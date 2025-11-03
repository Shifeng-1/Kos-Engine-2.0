#pragma once
#include "ScriptAdapter/TemplateSC.h"

class GroundCheckScript : public TemplateSC {
public:
	bool groundCheck = false;

	void Start() override {
		physicsPtr->GetEventCallback()->OnTriggerStay.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Ground") {
				groundCheck = true;
			}
		});

		physicsPtr->GetEventCallback()->OnTriggerExit.Add([this](const physics::Collision& col) {
			//if (col.thisEntityID != this->entity) { return; }
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Ground") {
				groundCheck = false;
			}
			});
	}

	void Update() override {

	}


	REFLECTABLE(GroundCheckScript)
};