
#include "Config/pch.h"
#include "ECS/ECS.h"
#include "Scripting/ScriptManager.h"
#include "Config/ComponentRegistry.h"
#include "Scene/SceneManager.h"
#include "Scripts/Include/ScriptHeader.h"
#include "Inputs/Input.h"

static std::vector<std::string>* scriptNames;

template <typename T>
void RegisterScript() {
	ComponentTypeRegistry::RegisterComponentType<T>();
	ComponentRegistry::GetECSInstance()->RegisterComponent<T>();
	scriptNames->push_back(T::classname());
}

extern "C"  __declspec(dllexport) void UpdateStatic(StaticVariableManager* svm) {
	//ScriptManager::m_GetInstance();
	//ecs::ECS::m_in
	//Link the ECS system together
	//TemplateSC::ecsPtr = std::shared_ptr<ecs::ECS>(reinterpret_cast<ecs::ECS*>(svm->ECSSystem), [](ecs::ECS*) { /* no delete, managed elsewhere */ }).get();
	scriptNames = svm->scriptNames;
	ComponentRegistry::SetECSInstance(static_cast<ecs::ECS*>(svm->ECSSystem));
	ComponentRegistry::SetFieldInstance(static_cast<FieldSingleton*>(svm->field));
	ComponentRegistry::SetInputInstance(static_cast<Input::InputSystem*>(svm->input));
	ComponentRegistry::SetSceneInstance(static_cast<scenes::SceneManager*>(svm->scene));

	TemplateSC::ecsPtr = ComponentRegistry::GetECSInstance();
	TemplateSC::Input = ComponentRegistry::GetInputInstance();
	TemplateSC::Scenes = ComponentRegistry::GetSceneInstance();


	RegisterScript<PlayerScript>();
	RegisterScript<EnemyScripts>();
	RegisterScript<AudioScript>();

	ComponentTypeRegistry::CreateAllDrawers((static_cast<FieldSingleton*>(svm->field)->GetAction()));
}