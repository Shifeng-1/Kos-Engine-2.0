/******************************************************************/
/*!
\file      Application.h
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Oct 02, 2024
\brief     This file contains the declaration to the main update 
		   loop of the engine. It initializes the various major 
		   systems and call upon them in the update loop.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#ifndef APP_H
#define APP_H

#include "Window.h"
#include "Reflection/Reflection.h"

#include "Debugging/Logging.h"
#include "Inputs/Input.h"
#include "Graphics/GraphicsManager.h"
#include "Resources/ResourceManager.h"
#include "Scene/SceneManager.h"
#include "Physics/PhysicsManager.h"
#include "DeSerialization/json_handler.h"
#include "Reflection/Field.h"
#include "ECS/ecs.h"
#include "Audio/AudioManager.h"


#include "../Editor/ImGui Panels/Editor.h"
#include "AssetManager/AssetManager.h"
/******************************************************************/
/*!
\class     Application
\brief     The Papplciation class is responsible for the entire engine.
		   It calls onto all the systems.
*/
/******************************************************************/
namespace Application {

	class Application {

		

	public:
		Application()
			: input()
			, peformance()
			, reflectionField()
			, resourceManager()
			, physicsManager()
			, graphicsManager() 
			, ecs(peformance, graphicsManager, resourceManager, input, physicsManager, scriptManager,audioManager)
			, lvWindow(ecs, input)
			, layersManager(ecs)
			, serialization(ecs)
			, sceneManager(ecs, serialization, resourceManager)
			, scriptManager(ecs, sceneManager, input, physicsManager,resourceManager, reflectionField)
			, audioManager()
			, assetManager()
			, Editor(lvWindow, assetManager, graphicsManager, ecs, sceneManager, serialization, reflectionField, input, physicsManager, layersManager, resourceManager, scriptManager, peformance, audioManager)
		
		{
		}
		~Application() = default;

		int Init();
		int Run();
		int m_Cleanup();
		std::filesystem::path exePath;

	private:
		//all dependencies should be here
		ecs::ECS ecs;
		AppWindow lvWindow;
		Peformance peformance;
		Input::InputSystem input;
		scenes::SceneManager sceneManager;
		GraphicsManager graphicsManager;
		ResourceManager resourceManager;
		physics::PhysicsManager physicsManager;
		serialization::Serialization serialization;
		ScriptManager scriptManager;
		Fields reflectionField;
		layer::LayerStack layersManager;
		AssetManager assetManager;
		audio::AudioManager audioManager;
		gui::ImGuiHandler Editor;

		
		
	};

}


#endif APP_H


