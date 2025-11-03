/********************************************************************/
/*!
\file      SceneManager.cpp
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Nov 11, 2024
\brief     This file contains the definitions for the `SceneManager` class,
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

This file supports scene management by providing functions for creating, saving,
loading, and clearing scenes within a game, allowing dynamic control of game states.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "SceneManager.h"
#include <RAPIDJSON/document.h>
#include <RAPIDJSON/writer.h>
#include <RAPIDJSON/filewritestream.h>
#include <RAPIDJSON/stringbuffer.h>
#include "ECS/ecs.h"



namespace scenes {


    bool SceneManager::CreateNewScene(const std::filesystem::path& scene)
    {
        std::ifstream checkFile(scene.string());
        //check if file name exist
        if (checkFile) {
            //if file name exist
            LOGGING_WARN("JSON file already exist, select another name");
            return false;
        }
        //create a json file

        FILE* fp = std::fopen(scene.string().c_str(), "wb");

        if (fp == nullptr) {
            LOGGING_ERROR("Fail to create new Scene");
            return false; 
        }

        fprintf(fp, "[]");
        char writeBuffer[1];  // Buffer to optimize file writing
        rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

        rapidjson::Writer<rapidjson::FileWriteStream> writer(os);

        std::fclose(fp);
        m_serialization.SaveScene(scene.string());

        // return file path
        return true;
    }

    void SceneManager::LoadScene(const std::filesystem::path& scene)
    {
        m_loadQueue.push_back(scene);
    }

    void SceneManager::ReloadScene()
    {
        // If Cached Scenes exist reload all Cached Scenes
        if (cacheScenePath.size() > 0) {
            std::vector<std::string> scenesToClear;
            for (auto& [scene, data] : m_ecs.sceneMap) {
                if (!data.isPrefab) {
                    scenesToClear.push_back(scene);
                }
            }
            for (auto const& scene : scenesToClear) {
                ImmediateClearScene(scene);
            }
            const std::string cacheExt("[Cached]");
            for (auto& filePath : cacheScenePath) {
                std::string fileName = filePath.filename().string();
                size_t pos = fileName.find(cacheExt);
                if (pos != std::string::npos) {
                    fileName.erase(pos, cacheExt.size());
                }
                ImmediateLoadScene(filePath, fileName);
            }
            // Change File Path for loadScenePath since filepath for cached scenes will contain [Cached]
            for (auto& [scene, filePath] : loadScenePath) {
                std::string path = filePath.string();
                size_t pos = path.find(cacheExt);
                if (pos != std::string::npos) {
                    path.erase(pos, cacheExt.size());
                    filePath = path;
                }
            }

            DeleteAllCacheScenes();
            cacheScenePath.clear();
        }
        else {
            //store scene path
            std::vector<std::string> scenepath;
            for (auto& scene : m_ecs.sceneMap) {
                scenepath.push_back(loadScenePath.find(scene.first)->second.string());
            }

            //clear all scenes
            //load baack previous scene
		    ClearAllScene();
            for (auto& scene : scenepath) {
                LoadScene(scene);
            }
        }
    }

    void SceneManager::ClearAllScene(bool includePrefabs) //EXCEPT PREFABS
    {
        for (auto& [scene, data] : m_ecs.sceneMap) {

            if(includePrefabs){
                ClearScene(scene);
            }
            else{
                if(!data.isPrefab){
                    ClearScene(scene);
                }
			}
            
        }
    }

    std::vector<std::filesystem::path> SceneManager::GetAllScenesPath() {
        std::vector<std::string> sce;
        for (auto& scenes : m_ecs.sceneMap) {
            sce.push_back(scenes.first);
        }

        //store scene path
        std::vector<std::filesystem::path> scenepath;
        for (auto& scene : sce) {
            scenepath.push_back(loadScenePath.find(scene)->second.string());
        }

        return scenepath;
    }

    void SceneManager::ClearAllSceneImmediate(){

        std::vector<std::string> sce;
        for (auto& scenes : m_ecs.sceneMap) {
            sce.push_back(scenes.first);

        }

        for (auto& scenes : sce) {
            ImmediateClearScene(scenes);
        }
    }

    void SceneManager::ClearScene(const std::string& scene)
    {
        m_clearQueue.push_back(scene);
    }

    void SceneManager::SaveScene(const std::string& scene)
    {
        const auto& scenepath = loadScenePath.find(scene);
        if (scenepath != loadScenePath.end()) {
            m_serialization.SaveScene(scenepath->second.string());
        }
    }

    void SceneManager::SaveAllActiveScenes(bool includeprefab)
    {
        if (m_ecs.GetState() != ecs::STOP) {
            LOGGING_WARN("Cannot save scene while in play mode");
            return;
        }

        for (auto& scenes : m_ecs.sceneMap) {
            //skip prefabs
            if (includeprefab && scenes.second.isPrefab) continue;
            SaveScene(scenes.first);
        }
    }

	void SceneManager::Update()
	{
        if (!m_clearQueue.empty()) {

			for (auto const& scene : m_clearQueue) {
				ImmediateClearScene(scene);
            }
            m_clearQueue.clear();

            //call on the asset manager garbage collection
            m_resourceManager.CollectGarbage();
        }

		if (!m_loadQueue.empty()) {

			for (auto const& scene : m_loadQueue) {
				ImmediateLoadScene(scene);
			}
            m_loadQueue.clear();
		}
	}

	bool SceneManager::ImmediateLoadScene(const std::filesystem::path& scene, const std::string forcedSceneName)
	{

		if (m_ecs.sceneMap.find(scene.filename().string()) != m_ecs.sceneMap.end()) {

			LOGGING_WARN("Scene already loaded");

			return false;
		}

		// Ensure the JSON file exists

		std::ifstream checkFile(scene.string());
		if (!checkFile) {
			if (!CreateNewScene(scene)) {
				LOGGING_ERROR("Fail to Create file");
				return false;
			}

		}
		std::string scenename = forcedSceneName.empty() ? scene.filename().string() : forcedSceneName;

		//contain scene path
		loadScenePath[scenename] = scene;

		//create new scene
		m_ecs.sceneMap[scenename];
		//check if file is prefab or scene

		// Load entities from the JSON file
		LOGGING_INFO("Loading entities from: {}", scene.string().c_str());

        m_serialization.LoadScene(scene.string(), scenename);  // Load into ECS
        onSceneLoaded.Invoke(m_ecs.sceneMap.at(scenename));

		LOGGING_INFO("Entities successfully loaded!");
        return true;
	}

	void SceneManager::ImmediateClearScene(const std::string& scene)
	{

		size_t numberOfEntityInScene = m_ecs.sceneMap.find(scene)->second.sceneIDs.size();
		for (int n{}; n < numberOfEntityInScene; n++) {
			if (m_ecs.sceneMap.find(scene)->second.sceneIDs.size() <= 0) break;
			auto entityid = m_ecs.sceneMap.find(scene)->second.sceneIDs.begin();
			if (!m_ecs.GetParent(*entityid)) {
				m_ecs.DeleteEntity(*entityid);
			}
		}


		//remove scene from activescenes
		m_ecs.sceneMap.erase(scene);
	}


	void SceneManager::SwapScenes(const std::string& oldscene, const std::string& newscene, ecs::EntityID id)
    {
        std::vector<ecs::EntityID>& vectorenityid = m_ecs.sceneMap.find(oldscene)->second.sceneIDs;
        std::vector<ecs::EntityID>::iterator it = std::find(vectorenityid.begin(), vectorenityid.end(), id);
        if (it == vectorenityid.end()) {
            LOGGING_ERROR("Entity not in old scene");
            return;
        }
        
        vectorenityid.erase(it);

        m_ecs.sceneMap.find(newscene)->second.sceneIDs.push_back(id);

    }


    void SceneManager::SetSceneActive(const std::string& scene, bool active) {
        if(active){

            auto& sceneData = m_ecs.sceneMap.at(scene);
            sceneData.isActive = true;
            
            for (const auto id : sceneData.sceneIDs) {
                m_ecs.RegisterEntity(id);
            }

        }
        else {
            auto& sceneData = m_ecs.sceneMap.at(scene);
            sceneData.isActive = false;

            for (const auto id : sceneData.sceneIDs) {
                m_ecs.DeregisterEntity(id);
            }

		}
    }

    // Files will be cached in the same location with added "filename[Cached].json" label
    void SceneManager::CacheCurrentScene(){

        for (auto [fileName, path] : loadScenePath) {
            auto iter = m_ecs.sceneMap.find(fileName);
            if (iter != m_ecs.sceneMap.end()) {
                if (iter->second.isPrefab) continue;
                std::string newPath = path.parent_path().string() + '\\' + path.stem().string() + "[Cached]" + path.extension().string();
                if (std::filesystem::exists(newPath)) {
                    std::filesystem::remove(newPath);
                    std::string metaPath = newPath + ".meta";
                    if (std::filesystem::exists(metaPath)) {
                        std::filesystem::remove(metaPath);
                    }
                }
                cacheScenePath.push_back(newPath);
                m_serialization.SaveScene(fileName, newPath);

                SetFileAttributesA(newPath.c_str(), GetFileAttributesA(newPath.c_str()) | FILE_ATTRIBUTE_HIDDEN);
            }
        }
    }

    void SceneManager::DeleteAllCacheScenes() {
        for (auto const& filePath : cacheScenePath) {
            std::filesystem::remove(filePath);
        }
    }

    void SceneManager::LoadSceneToCurrent(const std::string& currentScene, const std::filesystem::path& filepath) {
        //check if scene is loaded and file path exist

        if(m_ecs.sceneMap.find(currentScene) == m_ecs.sceneMap.end() || !std::filesystem::exists(filepath)){
            LOGGING_WARN("Scene not loaded or FilePath does not exist");
            return;
		}
		SceneData sceneData = m_ecs.sceneMap.at(currentScene);

        m_serialization.LoadScene(filepath, currentScene);

        SceneData& newSceneData = m_ecs.sceneMap.at(currentScene);
        newSceneData.sceneIDs.insert(newSceneData.sceneIDs.end(),
			sceneData.sceneIDs.begin(), sceneData.sceneIDs.end());
    }

  
    //void SceneManager::AssignEntityNewScene(const std::string& scene, m_ecs::EntityID id)
    //{
    //    m_ecs::ECS* m_ecs = m_ecs::ComponentRegistry::GetECSInstance();
    //    //assign all of entity's scene component into new scene
    //    const auto& componentKey = m_ecs.GetComponentKeyData();
    //    for (const auto& [componentName, key] : componentKey) {
    //        if (m_ecs.GetEntitySignature(id).test(key)) {
    //            m_ecs::Component* comp = m_ecs.GetIComponent<m_ecs::Component*>(componentName, id);
    //            if (comp) {
    //                comp->scene = scene;
    //            }

    //        }
    //    }

    //    //if id has children, call recurse
    //    const auto& child = m_hierachy::m_GetChild(id);
    //    if (child.has_value()) {
    //        for (auto id2 : child.value()) {
    //            AssignEntityNewScene(scene , id2);
    //        }
    //        
    //    }

    //}
}

