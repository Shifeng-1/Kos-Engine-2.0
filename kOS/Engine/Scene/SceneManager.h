/********************************************************************/
/*!
\file      SceneManager.h
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Sep 11, 2025
\brief     This file declares the definitions for the `SceneManager` class,
		   which handles the creation, loading, saving, clearing, and management
		   of scenes within the ECS framework.
		   - m_CreateNewScene: Creates a new JSON file for a scene.
		   - m_LoadScene: Loads entities from a JSON file into the ECS system.
		   - m_ReloadScene: Reloads all active scenes.
		   - m_ClearAllScene: Clears all non-prefab scenes.
		   - m_ClearScene: Removes all entities from a specified scene.
		   - m_SaveScene: Saves the current state of a specified scene to a JSON file.
		   - m_SaveAllActiveScenes: Saves all active scenes.
		   - m_SwapScenes: Moves an entity from one scene to another.
		   - GetSceneByEntityID: Finds the scene that contains a specified entity.

This file supports camera management by providing functions to calculate
view and projection matrices for rendering 3D scenes and UI elements.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#ifndef SCENE_H
#define SCENE_H

#include "Config/pch.h"
#include "Resources/ResourceManager.h"
#include "DeSerialization/json_handler.h"
#include "Events/Delegate.h"
#include "SceneData.h"
#include "ECS/ECS.h"

namespace scenes {

	struct PathActive {
		std::filesystem::path path;
		bool isActive;
	};

	class SceneManager {
		ecs::ECS& m_ecs;
		serialization::Serialization& m_serialization;
		ResourceManager& m_resourceManager;

	public:
		SceneManager(ecs::ECS& ecs, serialization::Serialization& slm, ResourceManager& rm):
			m_ecs(ecs),
			m_serialization(slm),
			m_resourceManager(rm)
		{};

		void EndFrame();
		bool CreateNewScene(const std::filesystem::path& scenepath);

		void LoadScene(const std::filesystem::path& scenepath);
		void ReloadScene();

		void ClearAllScene(bool includePrefabs = false);
		void ClearScene(const std::string& scene);
		void SaveScene(const std::string& scene);

		// first is path, second is isActive
		std::vector<PathActive> GetAllScenesPath();

		void ClearAllSceneImmediate();

		//Do not call this in the script
		bool ImmediateLoadScene(const std::filesystem::path& scenepath, const std::string forcedSceneName = "");
		//Do not call this in the script
		void ImmediateClearScene(const std::string& scene);

		void SaveAllActiveScenes(bool includeprefab = false);
		void SwapScenes(const std::string& oldscene, const std::string& newscene , ecs::EntityID id);

		void SetSceneActive(const std::string& scene, bool active);

		void CacheCurrentScene();
		void DeleteAllCacheScenes();
		bool LoadCacheSceneImmediate(const std::filesystem::path& scene, std::string originalName);

		void LoadSceneToCurrent(const std::string& currentScene, const std::filesystem::path& filepath);

		//void AssignEntityNewScene(const std::string& scene, ecs::EntityID id);
		//EVENTS

		Delegate<const SceneData&> onSceneLoaded;




	private:



		std::vector<std::filesystem::path> m_loadQueue;
		std::vector<std::string> m_clearQueue;


		std::unordered_map<std::string, std::filesystem::path> unloadScenePath;
		std::unordered_map<std::string, std::filesystem::path> loadScenePath;
		std::vector<std::filesystem::path> cacheScenePath;




	};
}
#endif SCENE_H
