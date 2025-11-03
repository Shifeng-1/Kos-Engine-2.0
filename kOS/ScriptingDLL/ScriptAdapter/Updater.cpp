
#include "Config/pch.h"
#include "ECS/ECS.h"
#include "Scene/SceneManager.h"
#include "Scripts/Include/ScriptHeader.h"
#include "Inputs/Input.h"
#include "Physics/PhysicsManager.h"
#include "Scripting/ScriptManager.h"

static std::vector<std::string>* scriptNames;

template <typename T>
void RegisterScript(ecs::ECS* ecs) {
	FieldComponentTypeRegistry::RegisterComponentType<T>();
	ecs->RegisterComponent<T>();
	scriptNames->push_back(T::classname());
}

extern "C"  __declspec(dllexport) void UpdateStatic(StaticVariableManager* svm) {
	//ScriptManager::m_GetInstance();
	//ecs::ECS::m_in
	//Link the ECS system together
	//TemplateSC::ecsPtr = std::shared_ptr<ecs::ECS>(reinterpret_cast<ecs::ECS*>(svm->ECSSystem), [](ecs::ECS*) { /* no delete, managed elsewhere */ }).get();
	scriptNames = svm->scriptNames;


	TemplateSC::ecsPtr = static_cast<ecs::ECS*>(svm->ECSSystem);
	TemplateSC::Input = static_cast<Input::InputSystem*>(svm->input);
	TemplateSC::Scenes = static_cast<scenes::SceneManager*>(svm->scene);
	TemplateSC::physicsPtr = static_cast<physics::PhysicsManager*>(svm->physics);
	TemplateSC::resource = static_cast<ResourceManager*>(svm->resource);

	RegisterScript<BulletLogic>(TemplateSC::ecsPtr);
	RegisterScript<EnemyManagerScript>(TemplateSC::ecsPtr);
	RegisterScript<PowerupManagerScript>(TemplateSC::ecsPtr);

	RegisterScript<FirePowerupManagerScript>(TemplateSC::ecsPtr);
	RegisterScript<LightningPowerupManagerScript>(TemplateSC::ecsPtr);
	RegisterScript<AcidPowerupManagerScript>(TemplateSC::ecsPtr);
	RegisterScript<FireLightningPowerupManagerScript>(TemplateSC::ecsPtr);
	RegisterScript<FireAcidPowerupManagerScript>(TemplateSC::ecsPtr);
	RegisterScript<LightningAcidPowerupManagerScript>(TemplateSC::ecsPtr);

	RegisterScript<GroundCheckScript>(TemplateSC::ecsPtr);
	RegisterScript<PlayerManagerScript>(TemplateSC::ecsPtr);
	RegisterScript<GunScript>(TemplateSC::ecsPtr);

	RegisterScript<PlayerScript>(TemplateSC::ecsPtr);
	RegisterScript<EnemyScripts>(TemplateSC::ecsPtr);
	RegisterScript<AudioScript>(TemplateSC::ecsPtr);

	RegisterScript<BulletLogic>(TemplateSC::ecsPtr);
	RegisterScript<EnemyManagerScript>(TemplateSC::ecsPtr);
	RegisterScript<PlayerManagerScript>(TemplateSC::ecsPtr);

	FieldComponentTypeRegistry::CreateAllDrawers(static_cast<Fields*>(svm->field)->GetAction());
}