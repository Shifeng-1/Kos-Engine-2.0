
#include "Config/pch.h"
#include "ECS.h"
#include "Component/ComponentHeader.h"
#include "Debugging/Logging.h"
#include "Reflection/ReflectionInvoker.h"
#include "Reflection/Field.h"
#include "Scene/SceneManager.h"


//ECS Varaible

namespace ecs{
	std::unordered_map<std::string, std::function<std::shared_ptr<IActionInvoker>()>> ComponentTypeRegistry::actionFactories;

	void ECS::Load() {

		//Allocate memory to each component pool
		RegisterComponent<NameComponent>();
		RegisterComponent<TransformComponent>();
		RegisterComponent<SpriteComponent>();
		RegisterComponent<CameraComponent>();
		RegisterComponent<AudioComponent>();
		RegisterComponent<TextComponent>();
		RegisterComponent<MeshFilterComponent>();
		RegisterComponent<CanvasRendererComponent>();
		RegisterComponent<MeshRendererComponent>();
		RegisterComponent<MaterialComponent>();
		RegisterComponent<SkinnedMeshRendererComponent>();
		RegisterComponent<AnimatorComponent>();
		RegisterComponent<LightComponent>();
		RegisterComponent<RigidbodyComponent>();
		RegisterComponent<BoxColliderComponent>();
		RegisterComponent<CapsuleColliderComponent>();
		RegisterComponent<SphereColliderComponent>();
		RegisterComponent<CharacterControllerComponent>();
		//RegisterComponent<ScriptComponent>();
		RegisterComponent<OctreeGeneratorComponent>();
		RegisterComponent<PathfinderComponent>();
		RegisterComponent<PathfinderTargetComponent>();
		RegisterComponent<CubeRendererComponent>();
		RegisterComponent<SphereRendererComponent>();

		RegisterComponent<ParticleComponent>();

		//Allocate memory to each system
		RegisterSystem<ScriptingSystem>(RUNNING);
		RegisterSystem<TransformSystem, TransformComponent>();
		RegisterSystem<CharacterControllerSystem, TransformComponent, CharacterControllerComponent>(RUNNING);
		RegisterSystem<ColliderSystem, TransformComponent>();
		RegisterSystem<RigidbodySystem, TransformComponent, RigidbodyComponent>(RUNNING);
		RegisterSystem<PhysicsSystem, TransformComponent, RigidbodyComponent>(RUNNING);
		RegisterSystem<CameraSystem, TransformComponent, CameraComponent>();
		RegisterSystem<MeshRenderSystem, TransformComponent, MaterialComponent, MeshFilterComponent>();
		RegisterSystem<SkinnedMeshRenderSystem, TransformComponent, SkinnedMeshRendererComponent, AnimatorComponent>();
		RegisterSystem<CubeRenderSystem, TransformComponent, MaterialComponent, CubeRendererComponent>();
		RegisterSystem<SphereRenderSystem, TransformComponent, MaterialComponent, SphereRendererComponent>();

		RegisterSystem<CanvasTextRenderSystem, TransformComponent, CanvasRendererComponent>();
		RegisterSystem<CanvasSpriteRenderSystem, TransformComponent, CanvasRendererComponent>();
		RegisterSystem<AnimatorSystem, TransformComponent, AnimatorComponent>();
		RegisterSystem<LightingSystem, TransformComponent, LightComponent>();
		RegisterSystem<DebugBoxColliderRenderSystem, TransformComponent, BoxColliderComponent>();
		RegisterSystem<DebugCapsuleColliderRenderSystem, TransformComponent, CapsuleColliderComponent>();
		RegisterSystem<DebugSphereColliderRenderSystem, TransformComponent, SphereColliderComponent>();
		RegisterSystem<AudioSystem, TransformComponent, AudioComponent>();
		RegisterSystem<PathfindingSystem, TransformComponent>();
		RegisterSystem<ParticleSystem, TransformComponent, ParticleComponent>();


	}

	void ECS::Init() {
		//loops through all the system
		for (auto& System : m_systemMap) {
			System.second->Init();
		}


	}


	void ECS::Update(float DT) {

		//update deltatime
		m_deltaTime = DT;

		
		//check for gamestate
		if (m_state != m_nextState) {
			if (m_nextState == START) {
				m_state = RUNNING;
				m_nextState = RUNNING;
			}
			else {
				m_state = m_nextState;
			}

				
		}

		//retrieve all active scenes
		std::vector<decltype(sceneMap)::key_type> keys;
		for (const auto& [sceneName, sceneID] : sceneMap) {
			if (sceneID.isActive) {
				keys.push_back(sceneName);
			}
			
		}


		//loops through all the system
		for (const auto& [systemName, system] : m_systemMap) {
	
			std::chrono::duration<float> systemDuration{};
			auto start = std::chrono::steady_clock::now();
			if (system->TestState(m_state)) { //only run state system registered in
				
				for (const auto& sceneName : keys) {
					

					system->Update();

				}

			}
			auto end = std::chrono::steady_clock::now();
			systemDuration = (end - start);
			m_performance.SetSystemValue(systemName, systemDuration.count());
		}
		
	}

	void ECS::Unload() {
		m_combinedComponentPool.clear();

		//delete ecs;
	}

	void ECS::RegisterEntity(EntityID ID) {

		for (auto& system : m_systemMap) {
			if ((m_entityMap.find(ID)->second & system.second->GetSignature()) == system.second->GetSignature()) {

				system.second->RegisterSystem(ID);

			}
		}
	}

	void ECS::DeregisterEntity(EntityID ID) {

		for (auto& system : m_systemMap) {
			if ((m_entityMap.find(ID)->second & system.second->GetSignature()) == system.second->GetSignature()) {

				system.second->DeregisterSystem(ID);

			}
		}

	}

	EntityID ECS::CreateEntity(std::string scene) {

		EntityID ID = 0;
		if (m_entityCount < MaxEntity) {
			ID = m_entityCount++;
		}
		else {
			ID = m_availableEntityID.top();
			m_availableEntityID.pop();
		}

		

		// set bitflag to 0
		m_entityMap[ID] = 0;


		//assign entity to scenes
		if(sceneMap.find(scene) == sceneMap.end()){
			LOGGING_WARN("Scene does not exits");
			throw std::runtime_error("Scene does not exits");
		}

		sceneMap.find(scene)->second.sceneIDs.push_back(ID);

		//add transform component and name component as default
		AddComponent<NameComponent>(ID);
		AddComponent<TransformComponent>(ID);

		return ID;
	}

	EntityID ECS::DuplicateEntity(EntityID DuplicatesID, std::string scene) {

		if (scene.empty()) {
			const auto& result = GetSceneByEntityID(DuplicatesID);
			if (result.empty()) {
				LOGGING_ERROR("Scene does not exits");
			}
			scene = result;
		}
		else {
			if (sceneMap.find(scene) == sceneMap.end()) {
				LOGGING_ERROR("Scene does not exits");
			}
		}

		EntityID NewEntity = CreateEntity(scene);
		ComponentSignature DuplicateSignature = m_entityMap.find(DuplicatesID)->second;

		for (const auto& [ComponentName, key] : m_componentKey) {
			if (DuplicateSignature.test(key)) {
				auto& action = componentAction.at(ComponentName);
				auto* comp = action->DuplicateComponent(DuplicatesID, NewEntity);
			}
		}

		m_entityMap.find(NewEntity)->second = DuplicateSignature;
		RegisterEntity(NewEntity);
		//checks if duplicates entity has parent and assign it
		if (GetParent(DuplicatesID).has_value()) {
			//transform->m_childID.push_back(NewEntity);
			auto parent = GetParent(DuplicatesID).value();
			SetParent(parent, NewEntity);
		}

		//checks if entity has child call recursion
		if (GetChild(DuplicatesID).has_value()) {
			//clear child id of vector for new entity
			TransformComponent* transform = GetComponent<TransformComponent>(NewEntity);
			transform->m_childID.clear();

			std::vector<EntityID> childID = GetChild(DuplicatesID).value();
			for (const auto& child : childID) {
				EntityID dupChild = DuplicateEntity(child, scene);
				SetParent(NewEntity, dupChild);
			}
		}

		return NewEntity;

	}

	bool ECS::DeleteEntity(EntityID ID) {

		
		//check if id is a thing
		if (m_entityMap.find(ID) == m_entityMap.end()) {
			LOGGING_ERROR("Entity Does Not Exist");
			return false;
		}


		if (GetParent(ID).has_value()) {
			EntityID parent = GetParent(ID).value();
			// if parent id is deleted, no need to remove its child
			if (m_entityMap.find(parent) != m_entityMap.end()) {
				TransformComponent* parentTransform = GetComponent<TransformComponent>(parent);
				size_t pos{};
				for (EntityID& id : parentTransform->m_childID) {
					if (ID == id) {
						parentTransform->m_childID.erase(parentTransform->m_childID.begin() + pos);
						break;
					}
					pos++;
				}
			}
		}


		DeregisterEntity(ID);



		// remove entity from scene
		const auto& result = GetSceneByEntityID(ID);
		auto& entityList = sceneMap.find(result)->second.sceneIDs;
		auto it = std::find(entityList.begin(), entityList.end(), ID);
		sceneMap.find(result)->second.sceneIDs.erase(it);





		//get child
		if (GetChild(ID).has_value()) {
			std::vector<EntityID> childs = GetChild(ID).value();
			for (auto& x : childs) {
				DeleteEntity(x);
			}
		}

		//delete guid off map
		DeleteGUID(GetComponent<NameComponent>(ID)->entityGUID);

		// reset all components
		for (const auto& [ComponentName, key] : m_componentKey) {
			if (m_entityMap.find(ID)->second.test(key)) {
				m_combinedComponentPool[ComponentName]->Delete(ID);
			}
		}

		//delete stored entity
		m_entityMap.erase(ID);		
		m_availableEntityID.push(ID);

		return true;
	}

	



	void ECS::FreeComponentPool(const std::string& componentName) {
		if (m_combinedComponentPool.find(componentName) != m_combinedComponentPool.end()) {
			m_combinedComponentPool.erase(componentName);
		}

	}

	const std::vector<EntityID>& ECS::GetComponentsEnties(const std::string& componentName) {
		if (m_combinedComponentPool.find(componentName) != m_combinedComponentPool.end()) {
			return m_combinedComponentPool.at(componentName)->GetEntityList();
		}
		else {
			throw std::runtime_error("ecs get component entities");
		}
	}

	std::string ECS::GetSceneByEntityID(ecs::EntityID entityID) {
		for (const auto& [sceneName, entityList] : sceneMap) {
			// Check if the entityID is in the current vector of entity IDs
			if (std::find(entityList.sceneIDs.begin(), entityList.sceneIDs.end(), entityID) != entityList.sceneIDs.end()) {
				return sceneName;  // Found the matching scene name
			}
		}
		return std::string();  // No match found
	}


	void ECS::SetParent(EntityID parent, EntityID child, bool updateTransform) {


		RemoveParent(child);

		TransformComponent* parentTransform = GetComponent<TransformComponent>(parent);
		//checks if child is already in parent
		if (GetParent(child).has_value()) {
			return;
		}

		//checks if parent is getting dragged into its child
		EntityID id = parent;
		while (GetParent(id).has_value()) {
			EntityID checkParentid = GetParent(id).value();
			if (checkParentid == child) {
				LOGGING_WARN("Cannot assign parent to its own child");
				return;
			}
			id = checkParentid;

		}

		parentTransform->m_childID.push_back(child);

		TransformComponent* childTransform = GetComponent<TransformComponent>(child);
		childTransform->m_haveParent = true;
		childTransform->m_parentID = parent;
		// Recalculate Local Transform after parenting
		if (updateTransform) {
			childTransform->localTransform = glm::inverse(parentTransform->transformation) * childTransform->transformation;
			utility::DecomposeMtxIntoTRS(childTransform->localTransform, childTransform->LocalTransformation.position, childTransform->LocalTransformation.rotation, childTransform->LocalTransformation.scale);
		}
	}

	void ECS::RemoveParent(EntityID child, bool updateTransform) {
		// removes id from both the child and the parents vector

		if (!GetParent(child).has_value()) {
			// does not have parrent
			return;
		}

		EntityID parent = GetParent(child).value();
		TransformComponent* parentTransform = GetComponent<TransformComponent>(parent);
		size_t pos{};
		for (EntityID& id : parentTransform->m_childID) {
			if (child == id) {
				parentTransform->m_childID.erase(parentTransform->m_childID.begin() + pos);
				break;
			}
			pos++;
		}

		TransformComponent* childTransform = GetComponent<TransformComponent>(child);
		childTransform->m_haveParent = false;
		childTransform->m_parentID = -1;
		//Updating Transformation Mtxs
		if (updateTransform) {
			childTransform->localTransform = childTransform->transformation;
			utility::DecomposeMtxIntoTRS(childTransform->localTransform, childTransform->LocalTransformation.position, childTransform->LocalTransformation.rotation, childTransform->LocalTransformation.scale);
		}
	}

	std::optional<EntityID> ECS::GetParent(EntityID child)
	{
		TransformComponent* childTransform = GetComponent<TransformComponent>(child);
		if (!childTransform || !childTransform->m_haveParent) {
			return std::optional<EntityID>();
		}

		return childTransform->m_parentID;

	}

	std::optional<std::vector<EntityID>> ECS::GetChild(EntityID parent)
	{
		TransformComponent* parentTransform = GetComponent<TransformComponent>(parent);
		if (parentTransform->m_childID.size() <= 0) {
			return std::optional<std::vector<EntityID>>();
		}

		return parentTransform->m_childID;

	}

}



