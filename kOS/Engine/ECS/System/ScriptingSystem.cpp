
#include "Config/pch.h"
#include "ECS/ECS.h"
#include "ScriptingSystem.h"
#include "Scripting/ScriptManager.h"
#include "ECS/Component/ScriptComponent.h"
#include "Resources/ResourceManager.h"
#include "Debugging/Performance.h"

namespace ecs {



	void ScriptingSystem::Init()
	{
		
		//Use The DLL
	}

	void ScriptingSystem::Update()
	{
		
		ECS* ecs = ECS::GetInstance();
		auto performance = Peformance::GetInstance();

		const auto& entities = m_entities.Data();
		ScriptManager* sm=ScriptManager::m_GetInstance();
		
		auto scriptList = sm->GetScriptList();
		for (const std::string& scriptName : scriptList) {

			std::chrono::duration<float> scriptDuration{};
			auto start = std::chrono::steady_clock::now();

			auto action = ecs->componentAction.at(scriptName);
			try {
				auto& entityList = ecs->GetComponentsEnties(scriptName);
				for (const EntityID id : entityList) {

					//TODO - find better way to go about this
					SceneData sceneData = ecs->GetSceneData(ecs->GetSceneByEntityID(id));
					if (!sceneData.isActive) continue;

					auto script = static_cast<ScriptClass*>(ecs->GetIComponent<void*>(scriptName, id));

					if (!script->isStart) {
						script->isStart = true;
						script->Start();
						
					}

					script->Update();
				}
			}
			catch (...) {
				continue;
			}
			auto end = std::chrono::steady_clock::now();
			scriptDuration = (end - start);
			performance->SetScriptValue(scriptName, scriptDuration.count());
			
		}
		
		//Get action invoker for script component


			//for (const EntityID id : entities) {
			//	//Check if its first time load
			//	//Get script component first
			//	ScriptComponent* scriptComp = ecs->GetComponent<ScriptComponent>(id);


			//	//Run if new
			//	if (!scriptComp->scriptFunctions.size()) {
			//		for (std::string sn : scriptComp->scriptNames) {
			//			if (sn.empty())continue;

			//			//Find appropriate class
			//			scriptComp->scriptFunctions.push_back(ScriptComponent::ScriptWrapper{});
			//			scriptComp->scriptFunctions.back().scriptName = sn;

			//			if (ScriptClass::ScriptClassGenerator.find(sn) == ScriptClass::ScriptClassGenerator.end()) {
			//				//Create and add new script name 
			//				sm->AddGenerator(sn);
			//			}
			//			scriptComp->scriptFunctions.back().script.reset(ScriptClass::ScriptClassGenerator[sn]());
			//			reinterpret_cast<ScriptClass*>(scriptComp->scriptFunctions.back().script.get())->entityID = id;

			//		}

			//		//Once functions are loaded, execute each function
			//		for (ScriptComponent::ScriptWrapper& sn : scriptComp->scriptFunctions) {
			//			reinterpret_cast<ScriptClass*>(sn.script.get())->Awake();
			//		}
			//		for (ScriptComponent::ScriptWrapper& sn : scriptComp->scriptFunctions) {
			//			reinterpret_cast<ScriptClass*>(sn.script.get())->Start();
			//		}
			//	}
			//	for (ScriptComponent::ScriptWrapper& sn : scriptComp->scriptFunctions) {
			//		reinterpret_cast<ScriptClass*>(sn.script.get())->Update();
			//	}
			//}
		




	}


}
