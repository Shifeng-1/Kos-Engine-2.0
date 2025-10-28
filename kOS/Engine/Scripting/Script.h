#pragma once

/********************************************************************/
/*!
\file      Script.h
\author    Gabe Ng 2301290 
\par       gabe.ng@digipen.edu
\date      Oct 03 2025
\brief     Contains basic definitions of script class to be used in dll scripting

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#include "Config/pch.h"
class Script {
public:

	typedef void (*DLLFunction)(std::string const&);
	void Awake() { this->DLLAwake(EntityID); };
	void Start() { this->DLLStart(EntityID); };
	void Update() { this->DLLUpdate(EntityID); };
	void LateUpdate() { this->DLLLateUpdate(EntityID); };
	void FixedUpdate() { this->DLLFixedUpdate(EntityID); };

	//Script contains data of each function
	DLLFunction DLLAwake{};
	DLLFunction DLLStart{};
	DLLFunction DLLUpdate{};
	DLLFunction DLLLateUpdate{};
	DLLFunction DLLFixedUpdate{};

	std::string EntityID;
};

class ScriptClass;
typedef ScriptClass* (*GenerateScriptClass)();
class ScriptClass {
public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();
	bool isStart = false;
	static std::map<std::string, GenerateScriptClass> ScriptClassGenerator;
};

