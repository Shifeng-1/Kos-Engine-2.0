/******************************************************************/
/*!
\file      Prefab.cpp
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Nov 11, 2024
\brief     This file contains the definations for the prefab class.
           It reads a json file and stores all its data. When the prefab
           is called in the game. It creates an entiy and copy
           the prefab data to the new entity



Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "Prefab.h"
#include "DeSerialization/json_handler.h"
#include "Debugging/Logging.h"
#include "ECS/ecs.h"
#include "ECS/ECS.h"
#include "Scene/SceneManager.h"
#include "AssetManager/AssetManager.h" // Double check if Jaz wants to do complete seperation of editor and engine
#include <RAPIDJSON/filewritestream.h>
#include <RAPIDJSON/prettywriter.h>
#include <RAPIDJSON/writer.h>
#include <RAPIDJSON/stringbuffer.h>

namespace prefab 
{
    void PrefabManager::AssignPrefabToNameComponent(ecs::EntityID parentid, std::string scenename) {
        const auto& vecChild = m_ecs.GetChild(parentid);
        if (!vecChild.has_value()) return;
        for (auto& childid : vecChild.value()) {
            ecs::NameComponent* nc = m_ecs.GetComponent<ecs::NameComponent>(childid);
            nc->isPrefab = true;
            nc->prefabName = scenename;

            if (m_ecs.GetChild(childid).has_value()) {
                AssignPrefabToNameComponent(childid, scenename);
            }
        }
    }

    // Creating Prefab Instance
    int PrefabManager::m_CreatePrefab(std::string prefabscene, std::string insertscene)
    {
        if (prefabscene == insertscene) {
            LOGGING_ERROR("Cannot load onto itself");
            return -1;
        }


        if (insertscene.empty()) {
            for (auto& scene : m_ecs.sceneMap) {

                if (scene.second.isActive && (!scene.second.isPrefab)) {
                    insertscene = scene.first;
                    break;
                }
            }      
        }

        if (m_ecs.sceneMap.find(prefabscene) == m_ecs.sceneMap.end()) {
            LOGGING_ERROR("Prefab not loaded into scene");
            return -1;
        }
       
        ecs::EntityID newId = m_ecs.CreateEntity(insertscene);

        DeepUpdatePrefab(m_ecs.sceneMap.at(prefabscene).prefabID, newId);

        AssignPrefabToNameComponent(newId, prefabscene);

        return newId;
    }

    // Recursive to align with how prefab children are also tagged as prefabs
    // Not sure if setting prefabName will affect anything
    // Currently only root parent has a prefabName
    void PrefabManager::SetPrefabStatus(ecs::EntityID id, bool status) {
        ecs::NameComponent* nc = m_ecs.GetComponent<ecs::NameComponent>(id);
        nc->isPrefab = true;
        ecs::TransformComponent* trans = m_ecs.GetComponent<ecs::TransformComponent>(id);
        for (auto child : trans->m_childID) {
            SetPrefabStatus(child, true);
        }
    }

    void PrefabManager::m_SaveEntitytoPrefab(ecs::EntityID id)
    {
        ecs::NameComponent* nc = m_ecs.GetComponent<ecs::NameComponent>(id);
        std::string m_jsonFilePath{ m_assetManager.GetAssetManagerDirectory() + "/Prefabs/" }; //TODO allow drag and drop onto content browser
        std::string filename;

        short count{};
        do {
            if (count > 0) {
                filename = nc->entityName + "_" + std::to_string(count) + ".prefab";
            }
            else {
                filename = nc->entityName + ".prefab";
            }
            count++;
        } while (m_ecs.sceneMap.find(filename) != m_ecs.sceneMap.end());

        SetPrefabStatus(id, true);
        nc->prefabName = filename;

        std::string path = m_jsonFilePath + filename;
        m_sceneManager.CreateNewScene(path);
        LOGGING_DEBUG(path.c_str());

        /*******************************SERIALIZATION START******************************************/

        serialization::JsonFileValidation(path);

        // Create JSON object to hold the updated values
        rapidjson::Document doc;
        doc.SetArray();  // Initialize as an empty array

        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

        std::unordered_set<ecs::EntityID> savedEntities;  //track saved entities

        //Start saving the entities
        m_serialization.SaveEntity(id, doc, allocator, savedEntities);

        // Write the JSON back to file
        rapidjson::StringBuffer writeBuffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(writeBuffer);
        doc.Accept(writer);

        std::ofstream outputFile(path);
        if (outputFile) {
            outputFile << writeBuffer.GetString();
            outputFile.close();
        }

        LOGGING_INFO("Save Prefab Successful");

        /*******************************SERIALIZATION END******************************************/

        // load prefab
        LoadPrefab(path);
    }

    void PrefabManager::OverwriteScenePrefab(ecs::EntityID id) {
        ecs::NameComponent* nc = m_ecs.GetComponent<ecs::NameComponent>(id);
        if (!nc->isPrefab) return;

        const std::string& prefabName = nc->prefabName;

        if (m_ecs.sceneMap.find(prefabName) != m_ecs.sceneMap.end()) {

            const auto& sceneData = m_ecs.sceneMap.at(prefabName);
            ecs::EntityID prefabID = sceneData.prefabID;

            DeepUpdatePrefab(id, prefabID);
        }
    }

    void PrefabManager::UpdateAllPrefab(const std::string& prefabSceneName) {
        if (m_ecs.sceneMap.find(prefabSceneName) == m_ecs.sceneMap.end()) return;
        const auto& prefabData = m_ecs.sceneMap.find(prefabSceneName);

        ecs::EntityID prefabID = prefabData->second.prefabID;

        for (const auto& [id, signature] : m_ecs.GetEntitySignatureData()) {
            ecs::NameComponent* nc = m_ecs.GetComponent<ecs::NameComponent>(id);
            ecs::TransformComponent* tc = m_ecs.GetComponent<ecs::TransformComponent>(id);
            if (!(tc->m_haveParent) && nc->isPrefab && (nc->prefabName == prefabSceneName)) {
                DeepUpdatePrefab(prefabID, id);
            }
        }
    }

    // A = Prefab , B = ID
    void PrefabManager::DeepUpdatePrefab(ecs::EntityID idA, ecs::EntityID idB) {
        if (idA == idB)return;

        const auto signatureA = m_ecs.GetEntitySignature(idA);
        const auto signatureB = m_ecs.GetEntitySignature(idB);

        //update components
        size_t transformKey = m_ecs.GetComponentKey(ecs::TransformComponent::classname());
        const auto& componentKey = m_ecs.GetComponentKeyData();
        for (const auto& [ComponentName, key] : componentKey) {
            auto action = m_ecs.componentAction[ComponentName];

            { //peform conditions
                //keep position unique to each object
                if (key == transformKey) {
                    auto* tcA = m_ecs.GetComponent<ecs::TransformComponent>(idA);
                    auto* tcB = m_ecs.GetComponent<ecs::TransformComponent>(idB);
                    auto deepCopy = DeepCopyComponents<ecs::TransformComponent>();
                    //skip position and roation
                    deepCopy(tcA->WorldTransformation.rotation, tcA->WorldTransformation.rotation);
                    deepCopy(tcA->WorldTransformation.scale, tcA->WorldTransformation.scale);
                    continue;
                }
            }

            if (signatureA.test(key)) {
                if (action->Compare(idA, idB) == false) { // if A != B or B does not exist, call duplicate (Assign and create(if missing) component
                    action->DuplicateComponent(idA, idB);
                }
            }
            else if (signatureB.test(key)) { //if A does not have component, B has, remove B's component
                action->RemoveComponent(idB);
            }
        }

        //Objective: Make both have the same number of children
        auto childsA = m_ecs.GetChild(idA);
        auto childsB = m_ecs.GetChild(idB);

        if (!childsA.has_value() && !childsB.has_value()) return; // both id do not have children

        int countA = 0, countB = 0; //number of children both id have

        if (childsA.has_value()) {
            countA = static_cast<int>(childsA.value().size());
        }

        if (childsB.has_value()) {
            countB = static_cast<int>(childsB.value().size());
        }

        //make A == B
        if (countA > countB) {
            int diff = countA - countB;
            const auto& scene = m_ecs.GetSceneByEntityID(idB);
            for (int n{}; n < diff; n++) {
                m_ecs.SetParent(idB, m_ecs.CreateEntity(scene));
            }
        }

        if (countA < countB) {
            int diff = countB - countA;
            std::vector<ecs::EntityID> childsVecB = childsB.value();
            for (int n{}; n < diff; n++) {
                m_ecs.DeleteEntity(childsVecB[n]);
            }
        }

        const auto childsVecA = m_ecs.GetChild(idA);
        const auto childsVecB = m_ecs.GetChild(idB);

        if (childsVecA.has_value() && childsVecB.has_value()) {
            //recurse the children
            for (int n{}; n < childsVecA.value().size(); n++) {
                DeepUpdatePrefab(childsVecA.value()[n], childsVecB.value()[n]);
            }
        }
    }

    void PrefabManager::OverwritePrefab_Component(ecs::EntityID entityID, const std::string& componentName, const std::string& prefabSceneName) {
        if (m_ecs.sceneMap.find(prefabSceneName) == m_ecs.sceneMap.end()) return;
        auto iter = m_ecs.sceneMap.find(prefabSceneName);
        if (iter == m_ecs.sceneMap.end()) return;
        ecs::EntityID prefabID = iter->second.prefabID;
        if (prefabID == entityID) return;

        auto action = m_ecs.componentAction[componentName];
        auto entitySignature = m_ecs.GetEntitySignature(entityID);
        if (entitySignature.test(m_ecs.GetComponentKey(componentName))) {
            action->RemoveComponent(prefabID);
        }
        action->DuplicateComponent(entityID, prefabID);

        // Update all Associated Prefabs;
        for (const auto& [id, signature] : m_ecs.GetEntitySignatureData()) {
            ecs::NameComponent* nc = m_ecs.GetComponent<ecs::NameComponent>(id);
            if (nc->isPrefab && (nc->prefabName == prefabSceneName)) {
                // Remove Comp in Revert will already check if it contains comp 
				//action->DuplicateComponent(prefabID, id);
                RevertToPrefab_Component(id, componentName, nc->prefabName);
            }
        }

        //save prefab
		m_sceneManager.SaveScene(prefabSceneName);
    }

    void PrefabManager::RevertToPrefab_Component(ecs::EntityID entityID, const std::string& componentName, const std::string& prefabSceneName) {

        if (m_ecs.sceneMap.find(prefabSceneName) == m_ecs.sceneMap.end()) return;
        auto iter = m_ecs.sceneMap.find(prefabSceneName);
        if (iter == m_ecs.sceneMap.end()) return;
        ecs::EntityID prefabID = iter->second.prefabID;
        if (prefabID == entityID) return;

        auto entitySignature = m_ecs.GetEntitySignature(entityID);
        auto action = m_ecs.componentAction[componentName];
        if (entitySignature.test(m_ecs.GetComponentKey(componentName))) {
            action->RemoveComponent(entityID);
        }
        action->DuplicateComponent(prefabID, entityID);
    }

    void PrefabManager::LoadPrefab(const std::filesystem::path& filepath) {

        auto scenename = filepath.filename();

        
        m_sceneManager.ImmediateLoadScene(filepath);
        auto& prefabData = m_ecs.sceneMap.at(scenename.string());
        prefabData.isPrefab = true;
        m_sceneManager.SetSceneActive(scenename.string(), false);

        //find the prefab root entity id
        for (auto& id : prefabData.sceneIDs) {
            ecs::TransformComponent* tc = m_ecs.GetComponent<ecs::TransformComponent>(id);
            if (!tc->m_haveParent) {
                m_ecs.sceneMap.find(scenename.string())->second.prefabID = id;
                ecs::NameComponent* nc = m_ecs.GetComponent<ecs::NameComponent>(id);
                nc->prefabName = scenename.string();
                break;
            }
        }
    }

    void PrefabManager::LoadAllPrefabs() {
        std::string prefabPath = m_assetManager.GetAssetManagerDirectory() + "/Prefabs/"; // Should have a better way to get file directories
        if (!std::filesystem::exists(prefabPath))return;
        for (const auto& entry : std::filesystem::directory_iterator(prefabPath)) {
			auto scenename = entry.path().filename();

            if (scenename.extension().string() == ".prefab") {
                LoadPrefab(entry.path());



            }
        }
    }

    ecs::ComponentSignature PrefabManager::ComparePrefabWithInstance(ecs::EntityID entityID) {
        std::string prefabName = m_ecs.GetComponent<ecs::NameComponent>(entityID)->prefabName;
        if (m_ecs.sceneMap.find(prefabName) == m_ecs.sceneMap.end()) return ecs::ComponentSignature();

        ecs::EntityID prefabId = m_ecs.sceneMap.find(prefabName)->second.prefabID;

        auto entitySignature = m_ecs.GetEntitySignature(entityID);
        auto prefabSignature = m_ecs.GetEntitySignature(prefabId);

        // Stores the resulting components which are different
        ecs::ComponentSignature result;

        const auto& componentKey = m_ecs.GetComponentKeyData();
        for (const auto& [ComponentName, key] : componentKey) {
            auto componentKey = m_ecs.GetComponentKey(ComponentName);

            if (entitySignature.test(componentKey) &&
                prefabSignature.test(componentKey)) {
                auto* idComp = m_ecs.GetIComponent<ecs::Component*>(ComponentName, entityID);
                auto* prefabComp = m_ecs.GetIComponent<ecs::Component*>(ComponentName, prefabId);

                auto& actionInvoker = m_ecs.componentAction[ComponentName];
                if (!actionInvoker->Compare(idComp, prefabComp)) { // 1 = same, 0 = diff
                    result.set(m_ecs.GetComponentKey(ComponentName));
                }
            }
            else if (prefabSignature.test(componentKey) != (entitySignature.test(componentKey))) {
                result.set(m_ecs.GetComponentKey(ComponentName));
            }
            // if prefab has component, but object does not
            // for reverting to prefab, component will be readded back through here.
            //if (prefabSignature.test(componentKey) && (!entitySignature.test(componentKey))) { 
            //    auto& actionInvoker = m_ecs.componentAction[ComponentName];
            //    actionInvoker->AddComponent(entityID);
            //    actionInvoker->DuplicateComponent(prefabId, entityID);
            //}
        }

        return result;
    }

    void PrefabManager::RefreshComponentDifferenceList(std::vector<std::string>& diffComp, ecs::EntityID entityID) {
        ecs::ComponentSignature sig = ComparePrefabWithInstance(entityID);
        diffComp.clear();
        const auto& componentKey = m_ecs.GetComponentKeyData();
        for (const auto& [ComponentName, key] : componentKey) {
            if (sig.test(m_ecs.GetComponentKey(ComponentName))) {
                diffComp.push_back(ComponentName);
            }
        }
    }
}

