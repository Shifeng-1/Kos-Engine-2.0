#include "ComponentRegistry.h"

ecs::ECS* ComponentRegistry::s_ecsInstance;
FieldSingleton* ComponentRegistry::s_fieldSingleton;
Input::InputSystem* ComponentRegistry::s_inputSystem;
scenes::SceneManager* ComponentRegistry::s_sceneSingleton;
//physics::PhysicsManager* ComponentRegistry::s_physicsSingleton;