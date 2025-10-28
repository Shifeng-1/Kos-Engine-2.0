
/********************************************************************/
/*!
\file      ScriptManager.cpp
\author    Gabe Ng 2301290
\par       gabe.ng@digipen.edu
\date      Oct 03 2025
\brief     Handles the loading of DLL and manipulation of DLL elements.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#include "Config/pch.h"
#include "ScriptManager.h"


//Systems to be shared with DLL
#include "Debugging/Logging.h"
#include "Config/ComponentRegistry.h"
#include "ECS/ECS.h"
#include "Inputs/Input.h"

std::shared_ptr<ScriptManager> ScriptManager::m_ScriptManagerPtr{ new ScriptManager{} };

//LoadDLL

std::string scriptPath = "../ScriptingDLL/";
std::string resourcePath = "../Resource/R_Scripts/R_Script.txt";

typedef void (*DLLUpdateStatic)(StaticVariableManager*);

void ScriptManager::Init(const std::string& scriptdllPath)
{
	m_dllPath = scriptdllPath + "\\script.dll";
	RunDLL();
}

void ScriptManager::LoadDLL() {
	ScriptClass::ScriptClassGenerator.clear();
	//Load DLL from path
	hInstDLL = LoadLibraryA(m_dllPath.c_str());
	if (!hInstDLL) { LOGGING_ERROR("Failed to load DLL library"); return; }
	//Create scripts to be added

	std::string cPrefix{ "c_Create" };

	//Update systems 
	DLLUpdateStatic updateFunc = (DLLUpdateStatic)GetProcAddress(hInstDLL, "UpdateStatic");
	updateFunc(&svm);

	//Create a file
	std::ofstream outfile(resourcePath);
	if (!outfile.is_open()) {
		LOGGING_ERROR("Failed to create R_Script at file path"); return;
	}
	//Find scripts inr esourfces and load them
	for (std::filesystem::directory_entry const& filePath : std::filesystem::directory_iterator(scriptPath)) {
		if (filePath.path().extension() == ".h") {
			std::string fileName{ filePath.path().string() };
			fileName = fileName.substr(scriptPath.size());
			fileName = fileName.substr(0, fileName.size() - filePath.path().extension().string().size());
			
			//Add editable variables
			LOGGING_INFO("Reading file {}", fileName);
			
			//Create function name
			std::string functionName{ cPrefix + fileName };
			//Add create function to the map
			LOGGING_INFO("Adding function {}", functionName);
			
			//Add file name to the script txt
			//For debug purposes
			GenerateScriptClass procAddress =(GenerateScriptClass)GetProcAddress(hInstDLL, functionName.c_str());
			if (procAddress) {
				outfile << fileName << '\n';
				//	ScriptClass::ScriptClassGenerator[fileName] = procAddress;
			//	ScriptWrapper sw;
			//	//sw.scriptName = fileName;
			//	//sw.GenereateSC();
			//	//sw.script->Awake();
			}
		}
	}
	outfile.close();
	FreeLibrary(hInstDLL);

}
void ScriptManager::LoadScripts() {
	//Load from script path
}
void ScriptManager::ReloadDLL() {
	FreeLibrary(hInstDLL);

}


//To be called before you press play in the scenee
void ScriptManager::RunDLL() {
	if (!hInstDLL) {
		hInstDLL = LoadLibraryA(m_dllPath.c_str());
		if (!hInstDLL) { LOGGING_ERROR("Failed to load DLL library"); return; }
		svm.ECSSystem = ecs::ECS::GetInstance();
		svm.field = FieldSingleton::GetInstance();
		svm.input = Input::InputSystem::GetInstance();
		svm.scriptNames = &scriptList;
		DLLUpdateStatic updateFunc = (DLLUpdateStatic)GetProcAddress(hInstDLL, "UpdateStatic");
		updateFunc(&svm);
		updateFunc = nullptr;
	}

}
void ScriptManager::PeformHotReload()
{
	if (hInstDLL) {
		UnloadDLL();
	}

	//











	RunDLL();

}
void AssetDLLTMP(std::string const&) { }

void ScriptManager::UnloadDLL() {
	//Loop through each script and free them
	//ScriptList.clear();

	if (hInstDLL) {
		for (auto a : ScriptClass::ScriptClassGenerator) {
			a.second = nullptr;
		}
		
		
		ScriptClass::ScriptClassGenerator.clear();
		FieldSingleton::GetInstance()->GetAction().clear();
		auto ecs = ecs::ECS::GetInstance();
		//clear ecs componentpool
		for (const auto& scripts : scriptList) {
			ecs->FreeComponentPool(scripts);
			ecs->componentAction.erase(scripts);
			ecs->GetComponentsString().erase(scripts);
		}
		scriptList.clear();

		FreeLibrary(hInstDLL);
		hInstDLL = nullptr;
	}
	}


void ScriptManager::AddGenerator(std::string const& id) {
	//Create function name
	std::string functionName{ "c_Create" + id };
	//Add create function to the map
	LOGGING_INFO("Adding function {}", functionName);

	//Add file name to the script txt
	//For debug purposes
	GenerateScriptClass procAddress = (GenerateScriptClass)GetProcAddress(hInstDLL, functionName.c_str());
	ScriptClass::ScriptClassGenerator[id] = procAddress;
}
