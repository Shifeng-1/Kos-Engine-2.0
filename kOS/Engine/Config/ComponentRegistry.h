/******************************************************************/
/*!
\file      ComponentRegistry.h
\author    Jaz Winn Ng
\par       jazwinn.ng@digipen.edu
\date      Sept 28, 2025
\brief     Stores singleton instances of ECS and FieldSingleton for global access.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/

#pragma once
#include "ECS/ecs.h"
#include "Reflection/Field.h"
#include "Inputs/Input.h"
#include "Scene/SceneManager.h"
//#include "Physics/PhysicsManager.h"

class ComponentRegistry {
private:
    static ecs::ECS* s_ecsInstance;
	static FieldSingleton* s_fieldSingleton;
    static Input::InputSystem* s_inputSystem;
    static scenes::SceneManager* s_sceneSingleton;
    //static physics::PhysicsManager* s_physicsSingleton;

public:
    static void SetECSInstance(ecs::ECS* ecs) { s_ecsInstance = ecs; }
    static ecs::ECS* GetECSInstance() { return s_ecsInstance; }

    static void SetInputInstance(Input::InputSystem* input) { s_inputSystem = input; }
    static Input::InputSystem* GetInputInstance() { return s_inputSystem; }

    static void SetSceneInstance(scenes::SceneManager* scene) { s_sceneSingleton = scene; }
    static scenes::SceneManager* GetSceneInstance() { return s_sceneSingleton; }

    //static void SetPhysicsInstance(physics::PhysicsManager* field) { s_physicsSingleton = field; }
    //static physics::PhysicsManager* GetPhysicsInstance() { return s_physicsSingleton; }

    static void SetFieldInstance(FieldSingleton* field) { s_fieldSingleton = field; }
    static FieldSingleton* GetFieldInstance() { return s_fieldSingleton; }
};