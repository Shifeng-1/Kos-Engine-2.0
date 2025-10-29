#pragma once

/********************************************************************/
/*!
\file      ScriptManager.h
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
#include "Script.h"


struct StaticVariableManager {
	void* ECSSystem;
	void* field;
	void* input;
	void* scene;
	void* physics;
	std::vector<std::string>* scriptNames;
};

class ScriptManager {
public: 

	static ScriptManager* m_GetInstance() {
		if (!m_ScriptManagerPtr) {
			m_ScriptManagerPtr.reset(new ScriptManager{});
		}
		return m_ScriptManagerPtr.get();
	}
	void Init(const std::string& scriptdllPath);


	void LoadDLL();
	void LoadScripts();
	void ReloadDLL();
	void RunDLL();
	void PeformHotReload();
	void UnloadDLL();
   
	void AddGenerator(std::string const& id);

	
	const std::vector<std::string>& GetScriptList() const { return scriptList; }
	//List of script pointers
private:
	static std::shared_ptr<ScriptManager> m_ScriptManagerPtr;
	
	std::vector<std::string> scriptList;
	std::string m_dllPath;
	HINSTANCE hInstDLL;
	StaticVariableManager svm{};
};
