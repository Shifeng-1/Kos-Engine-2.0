/******************************************************************/
/*!
\file      ECS.h
\author    Jaz Winn Ng
\par       jazwinn.ng@digipen.edu
\date      Sept 28, 2025
\brief	   Ecs class is the core of the Entity Component System (ECS) architecture.
		   It manages entities, components, and systems, facilitating their interactions
		   and lifecycle within the application.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/
#ifndef ECS_H
#define ECS_H

#include "Config/pch.h"

#include "Layers.h"
#include "ECSList.h"
#include "Scene/SceneData.h"

#include "ECS/Component/ComponentHeader.h"
#include "ECS/Component/Component.h"
#include "ECS/System/System.h"
#include "ECS/System/SystemHeader.h"
#include "ECS/SparseSet.h"


#include "Reflection/IReflectionInvoker.h"
#include "Reflection/DeepCopy.h"



namespace ecs {


	class ECS {

	private:

		ECS() = default;

	public:
		//singleton
		static ECS* GetInstance() {
			if (!m_InstancePtr) {
				m_InstancePtr.reset(new ECS{});
			}
			return m_InstancePtr.get();
		}

		void Load();
		void Init();
		void Update(float DeltaTime);
		void Unload();

		EntityID CreateEntity(std::string scene);
		EntityID DuplicateEntity(EntityID, std::string scene = {});
		bool DeleteEntity(EntityID);

		template<typename T>
		T* AddComponent(EntityID ID);
		template<typename T>
		void RemoveComponent(EntityID ID);
		template<typename T>
		T* GetComponent(EntityID ID);
		template<typename T>
		bool HasComponent(EntityID ID);
		template<typename T>
		T* DuplicateComponent(EntityID duplicateID, EntityID newID);
		template <typename T>
		void ResetComponent(EntityID ID);


		template <typename T>
		T GetIComponent(const std::string& componentName, EntityID ID);

		//COMPONENT DATA GETTERS
		const std::map<std::string, size_t>& GetComponentKeyData() {
			return m_componentKey;
		}

		template<typename T>
		size_t GetComponentKey(){
			return m_componentKey.at(T::classname());
		}
		size_t GetComponentKey(const std::string& className) {
			return m_componentKey.at(className);
		}

		std::unordered_set<std::string>& GetComponentsString() {
			return m_componentStrings;
		}

		//ENTITY DATA GETTERS
		ComponentSignature GetEntitySignature(EntityID ID) {
			return m_entityMap.at(ID);
		}
		const std::unordered_map<EntityID, ComponentSignature>& GetEntitySignatureData() {
			return m_entityMap;
		}

		void AddScene(const std::string& sceneName, const SceneData& sceneData) {
			sceneMap[sceneName] = sceneData;
		}

		const SceneData& GetSceneData(const std::string& sceneName) {
			return sceneMap.at(sceneName);
		}

		bool IsValidEntity(EntityID ID) {
			return m_entityMap.find(ID) != m_entityMap.end();
		}

		std::string GetSceneByEntityID(ecs::EntityID entityID);


		//SCENE DATA
		std::unordered_map<std::string, SceneData> sceneMap{};

		//LAYER DATA
		layer::LayerStack layersStack;

		//GAME STATE DATA
		GAMESTATE GetState() { return m_state; }
		GAMESTATE GetNextState() { return m_nextState; }
		void SetState(GAMESTATE state) { m_nextState = state; }

		float m_GetDeltaTime() { return m_deltaTime; }
		

		std::unordered_map<std::string, std::shared_ptr<IActionInvoker>> componentAction;

		template <typename T, typename... DependentComponent >
		void RegisterComponent();
		template<typename T, typename... Components, typename... States>
		void RegisterSystem(States... states);

		void FreeComponentPool(const std::string& componentName);
		const std::vector<EntityID>& GetComponentsEnties(const std::string& componentName);


	private:
		void RegisterEntity(EntityID);
		void DeregisterEntity(EntityID);

	private:
		//modify from set next state
		GAMESTATE m_nextState{ STOP };
		GAMESTATE m_state{ STOP };

		float m_deltaTime{};

		//COMPONENT DATA
		std::unordered_map<std::string, std::shared_ptr<ISparseSet>> m_combinedComponentPool;
		std::unordered_map<std::string, std::vector<std::string>> m_dependentComponent;
		std::map<std::string, size_t> m_componentKey;
		std::unordered_set<std::string> m_componentStrings;
		size_t totalComponents = 0;

		//SYSTEMDATA
		std::map<std::string, std::shared_ptr<ISystem>> m_systemMap;

		//ENTITY DATA
		std::unordered_map<EntityID, ComponentSignature> m_entityMap;
		EntityID m_entityCount{};
		std::stack<EntityID> m_availableEntityID;

		static std::shared_ptr<ECS> m_InstancePtr;
	};

	#include "Reflection/ReflectionInvoker.h" //this stays after ecs class and before component type registry, HEADER HELL

	class ComponentTypeRegistry {
	private:
		static std::unordered_map<std::string, std::function<std::shared_ptr<IActionInvoker>()>> actionFactories;

	public:
		template <typename T>
		static void RegisterComponentType(ECS* ecs) {
			actionFactories[T::classname()] = [ecs]() {
				return std::make_shared<ActionInvoker<T>>(ecs);
				};

		}

		static void CreateAllDrawers(std::unordered_map<std::string, std::shared_ptr<IActionInvoker>>& drawers) {
			for (const auto& [className, factory] : actionFactories) {
				drawers[className] = factory();
			}
		}
	};


	template<typename T, typename... DependentComponent>
	void ECS::RegisterComponent()
	{
		std::string classname = T::classname();
		if (m_combinedComponentPool.find(classname) != m_combinedComponentPool.end()) {
			m_combinedComponentPool.erase(classname);
		}

		// Count how many dependent components were passed
		constexpr size_t dependencyCount = sizeof...(DependentComponent);

		if constexpr (dependencyCount > 0) {
			(..., m_dependentComponent[classname].push_back(DependentComponent::classname()));
		}
		
		m_combinedComponentPool[classname] = std::make_shared<SparseSet<T>>();
		m_componentKey[classname] = ++totalComponents;
		m_componentStrings.insert(classname);

		ComponentTypeRegistry::RegisterComponentType<T>(this);
		ComponentTypeRegistry::CreateAllDrawers(componentAction);


	}

	template<typename T, typename... Components, typename... States>
	void ECS::RegisterSystem(States... states)
	{
		ComponentSignature signature;

		// reversed order expansion
		(..., signature.set(GetComponentKey(Components::classname())));

		m_systemMap[T::classname()] = std::make_shared<T>();
		m_systemMap[T::classname()]->AssignSignature(signature);

		std::bitset<GAMESTATE_COUNT> gameState;
		if constexpr (sizeof...(states) == 0) {
			gameState.set(); // all states enabled
		}
		else {
			(..., gameState.set(states));
		}

		m_systemMap[T::classname()]->SetState(gameState);

		
	}


	template <typename T>
	T* ECS::AddComponent(EntityID ID) {


		//checks if component already exist
		if (m_entityMap[ID].test(GetComponentKey<T>())) {
			LOGGING_WARN("Entity Already Has Component");
			//return already existing component
			return GetComponent<T>(ID);
		}

		T* ComponentPtr = std::static_pointer_cast<SparseSet<T>>(m_combinedComponentPool.at(T::classname()))->Set(ID, T());
		ComponentPtr->entity = ID;

		//assign component to a scene
		//check for entity scene
		const auto& scene = GetSceneByEntityID(ID);
		if (!scene.empty()) {
			static_cast<Component*>(ComponentPtr)->scene = scene;
		}
		else {
			LOGGING_ASSERT_WITH_MSG("Entity not assigned scene");
		}

		m_entityMap.find(ID)->second.set(GetComponentKey<T>());

		//checks if new component fufils any of the system requirements
		RegisterEntity(ID);

		//check if component has dependent component
		if (m_dependentComponent.find(T::classname()) != m_dependentComponent.end()) {
			for (const auto& dependentComp : m_dependentComponent[T::classname()]) {
				if (!m_entityMap[ID].test(GetComponentKey(dependentComp))) {
					LOGGING_INFO("Auto Adding Dependent Component: " + dependentComp);
					auto& action = componentAction.at(dependentComp);
					action->AddComponent(ID);
				}
			}
		}

		return ComponentPtr;
	}

	template <typename T>
	void ECS::RemoveComponent(EntityID ID) {

		//checks if component already exist
		if (!m_entityMap[ID].test(GetComponentKey<T>())) {
			LOGGING_POPUP("Entity Component has already been removed");
			return;
		}

		m_combinedComponentPool.at(T::classname())->Delete(ID);

		//deregister everthing
		DeregisterEntity(ID);

		m_entityMap.find(ID)->second.reset(GetComponentKey<T>());

		//register everything
		RegisterEntity(ID);
	}

	template<typename T>
	T* ECS::GetComponent(EntityID ID) {
		return std::static_pointer_cast<SparseSet<T>>(m_combinedComponentPool.at(T::classname()))->Get(ID);
	}

	template<typename T>
	bool ECS::HasComponent(EntityID ID) {
		return m_combinedComponentPool.at(T::classname())->ContainsEntity(ID);
	}


	template<typename T>
	T* ECS::DuplicateComponent(EntityID duplicateID, EntityID newID) {
		T* duplicateComponent = GetComponent<T>(duplicateID);

		T* NewComponent;
		if (HasComponent<T>(newID)) {
			NewComponent = GetComponent<T>(newID);
		}
		else {
			NewComponent = AddComponent<T>(newID);
		}


		DeepCopyComponents<T> duplicator;
		NewComponent->ApplyFunctionPairwise(duplicator, *duplicateComponent);

		return NewComponent;
	}

	template <typename T>
	void ECS::ResetComponent(EntityID ID)
	{
		m_combinedComponentPool.at(T::classname())->Delete(ID);
		std::static_pointer_cast<SparseSet<T>>(m_combinedComponentPool.at(T::classname()))->Set(ID, T());
	}

	template <typename T>
	T ECS::GetIComponent(const std::string& componentName, EntityID ID)
	{
		auto& action = componentAction.at(componentName);
		return static_cast<T>(m_combinedComponentPool.at(componentName)->GetBase(ID));
	}

}




#endif  ECS_H