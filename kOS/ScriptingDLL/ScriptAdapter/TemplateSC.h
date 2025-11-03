#pragma once

#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ECS/ECS.h"
#include "Scripting/Script.h"
#include "Inputs/Input.h"
#include "Inputs/Keycodes.h"
#include "Scene/SceneManager.h"
#include "Physics/PhysicsManager.h"
#include "Resources/ResourceManager.h"

class TemplateSC :public ecs::Component, public ScriptClass {
public:
	//use raw, shared ptr will destruct exe ecs
	static ecs::ECS* ecsPtr;
	static Input::InputSystem* Input;
	static scenes::SceneManager* Scenes;
	static physics::PhysicsManager* physicsPtr;
	static ResourceManager* resource;
private:
};


template <typename T>
int DuplicatePrefabIntoScene(const std::string& scene, const utility::GUID guid) {

	std::string path = TemplateSC::resource->GetResourcePath<T>(guid);

	if (path.empty() || scene.empty()) {
		LOGGING_ERROR("Prefab file path is empty, cannot duplicate prefab into scene.");
		return -1;
	}

	auto* sm = TemplateSC::Scenes;
	sm->LoadSceneToCurrent(scene, path);
	auto* ecs = TemplateSC::ecsPtr;
	const auto& sceneData = ecs->GetSceneData(scene);
	ecs::EntityID newID;

	//retrieve the new id that just spawned (the one without parent) - TODO: improve this later
	for ( auto entityID : sceneData.sceneIDs) {
		auto* tc = ecs->GetComponent<ecs::TransformComponent>(entityID);
		if (tc->m_haveParent == false) {
			return static_cast<int>(entityID);
		}
	}

	return -1;
}
