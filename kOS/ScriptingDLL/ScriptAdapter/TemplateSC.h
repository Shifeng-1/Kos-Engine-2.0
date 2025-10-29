#pragma once

#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ECS/ECS.h"
#include "Scripting/Script.h"
#include "Inputs/Input.h"
#include "Inputs/Keycodes.h"
#include "Scene/SceneManager.h"


class TemplateSC :public ecs::Component,  public ScriptClass {
public:
	//use raw, shared ptr will destruct exe ecs
	static ecs::ECS* ecsPtr;
	static Input::InputSystem* Input;
	static scenes::SceneManager* Scenes;
	
private:
};


