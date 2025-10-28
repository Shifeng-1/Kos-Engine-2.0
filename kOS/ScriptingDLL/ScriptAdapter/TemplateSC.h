#pragma once

#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ECS/ECS.h"
#include "Scripting/Script.h"
#include "Inputs/Input.h"


class TemplateSC :public ecs::Component,  public ScriptClass {
public:
	//use raw, shared ptr will destruct exe ecs
	
	static ecs::ECS* ecsPtr;
	static Input::InputSystem* Input;
private:
};


