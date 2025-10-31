/********************************************************************/
/*!
\file      imgui_hierarchy_window.cpp
\author    Ng Jaz Winn, jazwinn.ng, 2301502
\par       jazwinn.ng@digipen.edu
\date      Oct 30, 2025
\brief     This file contains the implementation of the ImGui Hierarchy Window for rendering and managing game objects.
           - Displays and updates a list of game objects (entities) in the ECS framework.
           - Provides functionality for adding, deleting, and duplicating game objects.
           - Uses ImGui to create a hierarchy view, with interaction buttons for each game object.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Editor.h"

#include "ECS/ECS.h"
#include "ECS/Hierachy.h"
#include "DeSerialization/json_handler.h"
#include "Resources/ResourceManager.h"
#include "Debugging/Logging.h"
#include "AssetManager/Prefab.h"
#include "Editor/EditorCamera.h"
#include "Scene/SceneManager.h"

namespace gui {

    unsigned int ImGuiHandler::DrawHierachyWindow()
    {
        //fetch ecs
        ecs::ECS* ecs = ecs::ECS::GetInstance();
        scenes::SceneManager* scenemanager = scenes::SceneManager::m_GetInstance();
        //assetmanager::AssetManager* assetmanager = assetmanager::AssetManager::m_funcGetInstance();
        // Custom window with example widgets
        ImGui::Begin("Hierachy Window", nullptr, ImGuiWindowFlags_MenuBar);

        //std::string ObjectCountStr = "Oject Count: " + std::to_string(ecs->m_ECS_EntityMap.size());
        //ImGui::Text(ObjectCountStr.c_str());

        static std::string searchString;
        if (m_prefabSceneMode)searchString.clear();

        //menu bar for search

        if (ImGui::BeginMenuBar()) {
            ImGui::Text("Search:");
            ImGui::SameLine(); // Keep the next widget on the same line
            if (ImGui::InputText("###1234Search", &searchString)) {

            }
            ImGui::EndMenuBar(); // End menu bar
        }

        // Add Game Object Section
        if (ImGui::Button("+ Add GameObject"))
            ImGuiHandler::m_objectNameBox ? ImGuiHandler::m_objectNameBox = false : m_objectNameBox = true;

        if (m_objectNameBox)
        {
            if (ImGui::InputText("##", m_charBuffer, IM_ARRAYSIZE(m_charBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if (!m_activeScene.empty()) {
                    ecs::EntityID newEntityID = ecs->CreateEntity(m_activeScene);

                    //if in prefab mode, assign entity to upmost parent
                    if (m_prefabSceneMode) {
                        ecs::EntityID id = ecs->sceneMap.find(m_activeScene)->second.prefabID;
                        hierachy::m_SetParent(std::move(id), newEntityID);
                    }

                    m_clickedEntityId = newEntityID;

                    ecs->GetComponent<ecs::NameComponent>(newEntityID)->entityName = std::string(m_charBuffer);

                    m_charBuffer[0] = '\0';
                    m_objectNameBox = false;
                }
                else {
                    LOGGING_POPUP("No Scene Loaded");
                }
            }
        }

        for (auto& sceneentity : ecs->sceneMap) 
        {
            //when prefab mode is on, skip non prefabs, and non active prefabs
            if (m_prefabSceneMode) {
                if (sceneentity.second.isPrefab == false || sceneentity.second.isActive == false) continue;
                //create seperator
                ImGui::SeparatorText("Prefab");

                if (ImGui::Button("Back")){
                    //save "prefab"
                    scenemanager->SaveScene(m_activeScene);
                    std::string prefabName = m_activeScene;
                    //set current prefab back to inactive
                    scenemanager->SetSceneActive(m_activeScene, false);
                    
                    //set back scene's active state
                    for (const auto& [scene, sceneData] : ecs->sceneMap) {
                        if (sceneData.isPrefab == false) {
                            scenemanager->SetSceneActive(scene, m_savedSceneState.find(scene)->second);
                        }         
                    }

                    //set back active scene
                    for (auto& scene : ecs->sceneMap) {
                        if (!scene.second.isPrefab) {
                            m_activeScene = scene.first;
                            break;
                        }
                    }

                    m_prefabSceneMode = false;
                    m_clickedEntityId = -1;

                    // Instead of Updating all the time, Differences needs to be checked.
                    // Prompt when leaving prefab mode, 
                    std::vector<std::string> diffList;
                    const auto& prefabscene = ecs->sceneMap.find(prefabName);
                    prefab::RefreshComponentDifferenceList(diffList, prefabscene->second.prefabID);

                    //m_ecs->DeleteEntity(duppedID); // Dupping Somehow causes us to update all the prefab scenes?
                    //duppedID = -1;

                    for (const auto& id : ecs->GetEntitySignatureData()) {
                        ecs::NameComponent* nc = ecs->GetComponent<ecs::NameComponent>(id.first);
                        if (nc->isPrefab && (nc->prefabName == prefabName)) {
                            for (const auto& compName : diffList) {
                                if (compName == ecs::NameComponent::classname()) continue;
                                prefab::RevertToPrefab_Component(id.first, compName, prefabName);
                            }
                        }
                    }
                }
            }
            
            //skip if prefab is not active 
            if (sceneentity.second.isPrefab == true && sceneentity.second.isActive == false) continue;

            std::string headerstr = sceneentity.first.substr(0, sceneentity.first.find_last_of('.'));
            //collapsing header for scene
            bool opens{};
            if (sceneentity.second.isActive == false) {
                headerstr += " (Unloaded)";
                ImGui::CollapsingHeader(headerstr.c_str());
            }
            else {
                if (sceneentity.first == m_activeScene) {
                    headerstr += " (Active)";
                }
                opens = ImGui::CollapsingHeader(headerstr.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
            }

            if (ImGui::BeginPopupContextItem()) {
                if ((sceneentity.first != m_activeScene) && ImGui::MenuItem("Remove Scene")) {
                    scenemanager->ClearScene(sceneentity.first);

                    //break loop
                    ImGui::EndPopup();
                    break;
                }

                if ((sceneentity.first != m_activeScene) && (sceneentity.second.isActive == true) && ImGui::MenuItem("Unload Scene")) {
                    scenemanager->SetSceneActive(sceneentity.first, false);
                    m_clickedEntityId = -1;

                    if (!m_prefabSceneMode) {
                        //change scene if current active scene is unloaded
                        if (ecs->sceneMap.find(m_activeScene)->second.isActive == false) {
                            //set first loaded scene as active
                            for (auto& scene : ecs->sceneMap) {
                                if (scene.second.isActive == true && scene.second.isPrefab == false) {
                                    m_activeScene = ecs->sceneMap.begin()->first;
                                }

                            }
                        }
                    }

                    ImGui::EndPopup();
                    break;
                }

                if ((sceneentity.second.isActive == false) && ImGui::MenuItem("load Scene")) {
					scenemanager->SetSceneActive(sceneentity.first, true);
                    ImGui::EndPopup();
                    break;
                }

                if (ImGui::MenuItem("Save Scene")) {
                    onSaveAll.Invoke(sceneentity.first);
                }

                if ((sceneentity.first != m_activeScene) && ImGui::MenuItem("Set Active")) {
                    m_activeScene = sceneentity.first;

                    if (sceneentity.second.isActive == false) {
                        sceneentity.second.isActive = true;
                    }
                }

                ImGui::EndPopup();
            }

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
                {
                    IM_ASSERT(payload->DataSize == sizeof(ecs::EntityID));
                    ecs::EntityID Id = *static_cast<ecs::EntityID*>(payload->Data);
                    const auto& scene = m_ecs->GetSceneByEntityID(Id);

                    if (!scene.empty()) {
                        scenemanager->SwapScenes(scene, sceneentity.first, Id);
                    }

                    //if entity is a child, break from parent
                    const auto& parent = hierachy::GetParent(Id);
                    if (parent.has_value()) {
                        hierachy::m_RemoveParent(Id);
                    }

                    hierachy::m_UpdateChildScene(Id);
                }

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file"))
                {
                    //IM_ASSERT(payload->DataSize == sizeof(std::filesystem::path));
                    std::filesystem::path filename = static_cast<const char*>(payload->Data);

                    if (filename.filename().extension().string() == ".prefab") {                     
                        prefab::m_CreatePrefab(filename.filename().string(), sceneentity.first);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (opens) {
                for (auto entity : sceneentity.second.sceneIDs) {

                    const auto& entityMap = m_ecs->GetEntitySignatureData();
                    if (entityMap.find(m_clickedEntityId) == entityMap.end()) {
                        m_clickedEntityId = entity;
                        m_isUi = false;
                    }

                    //search bar if if string not empty, must match the entity name
                    ecs::NameComponent* nc = ecs->GetComponent<ecs::NameComponent>(entity);
                    if (!searchString.empty() && !containsSubstring(nc->entityName, searchString))continue;

                    //draw parent entity node
                    //draw entity with no parents hahaha
                    if (!hierachy::GetParent(entity).has_value()) {
                        if (DrawEntityNode(entity) == false) {
                            //delete is called
                            break;
                        }

                    }
                }
            }      
        }
          
        if (ImGui::GetContentRegionAvail().x > 0 && ImGui::GetContentRegionAvail().y > 0) {
            //std::cout << "x: " << ImGui::GetContentRegionAvail().x << std::endl;
            //std::cout << "y: " << ImGui::GetContentRegionAvail().y << std::endl;

            ImGui::InvisibleButton("#invbut", ImVec2{ ImGui::GetContentRegionAvail().x,ImGui::GetContentRegionAvail().y });
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
                {
                    IM_ASSERT(payload->DataSize == sizeof(ecs::EntityID));
                    ecs::EntityID Id = *static_cast<ecs::EntityID*>(payload->Data);

                    // if in prefab mode and parent does not have parent, reject
                    if (m_prefabSceneMode && hierachy::GetParent(Id).has_value() && (!hierachy::GetParent(hierachy::GetParent(Id).value()).has_value())) {
                        
                    }
                    if (m_prefabSceneMode) {
                        hierachy::m_SetParent(ecs->sceneMap.find(m_activeScene)->second.prefabID, Id);
                    }
                    else {
                        hierachy::m_RemoveParent(Id);
                    }
                }

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file"))
                {
                    //IM_ASSERT(payload->DataSize == sizeof(std::filesystem::path));
                    IM_ASSERT(payload->DataSize == sizeof(AssetPathGUID));
                    const AssetPathGUID* data = static_cast<const AssetPathGUID*>(payload->Data);

                    std::filesystem::path filePath = data->path;
 

                    if (!m_prefabSceneMode && filePath.filename().extension().string() == ".json") {
                        scenemanager->LoadScene(filePath);
                    }

                    if (!m_prefabSceneMode && filePath.filename().extension().string() == ".prefab") {
                       prefab::m_CreatePrefab(filePath.filename().string(), m_activeScene);
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }
        ImGui::End();
        return m_clickedEntityId;
    }


    bool ImGuiHandler::DrawEntityNode(ecs::EntityID id) {
        ecs::ECS* ecs = ecs::ECS::GetInstance();
        
        ecs::TransformComponent* transCom = ecs->GetComponent<ecs::TransformComponent>(id);
        if (transCom == NULL) return false;

        ImGuiTreeNodeFlags flag = ((static_cast<unsigned int>(m_clickedEntityId) == id) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
        if (transCom->m_childID.size() <= 0) {
            flag |= ImGuiTreeNodeFlags_Leaf;
        }
        
        ecs::NameComponent* nc = ecs->GetComponent<ecs::NameComponent>(id);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::InvisibleButton(std::string{ "##invireorderbutton" + std::to_string(id) }.c_str(), ImVec2{ ImGui::GetContentRegionAvail().x , 1.f });
        ImGui::PopStyleVar();

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
            {
                IM_ASSERT(payload->DataSize == sizeof(ecs::EntityID));
                ecs::EntityID SwapId = *static_cast<ecs::EntityID*>(payload->Data);

                const std::string swapScene = m_ecs->GetSceneByEntityID(SwapId);
                const std::string idScene = m_ecs->GetSceneByEntityID(id);

                if (SwapId != id && swapScene == idScene) {

                    auto& _scene = ecs->sceneMap.find(swapScene)->second;
                    const auto& eraseit = std::find(_scene.sceneIDs.begin(), _scene.sceneIDs.end(), SwapId);
                    
                    if (eraseit != _scene.sceneIDs.end()) {
                        _scene.sceneIDs.erase(eraseit);
                    }

                    const auto& IDit = std::find(_scene.sceneIDs.begin(), _scene.sceneIDs.end(), id);

                    if (IDit != _scene.sceneIDs.end()) {
                        _scene.sceneIDs.insert(IDit, SwapId);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        //create color if prefab
        if (nc->isPrefab) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.2f, 0.1f, 1.0f));
        }
        else if (nc->hide) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        }
        bool open = ImGui::TreeNodeEx(std::to_string(id).c_str(), flag, nc->entityName.c_str());
        if (nc->isPrefab || nc->hide) ImGui::PopStyleColor();

        if (ImGui::IsItemClicked())
        {
            m_clickedEntityId = id;
            m_isUi = false;
            //Check if it houses any ui elements
            if (ecs->HasComponent<ecs::CanvasRendererComponent>(id)
                ||( hierachy::GetParent(m_clickedEntityId).has_value()&&
                    ecs->HasComponent<ecs::CanvasRendererComponent>(hierachy::GetParent(m_clickedEntityId).value()))) {
                std::cout << "IS UI\n";
                m_isUi = true;
            }
        }

        if (ImGui::GetIO().KeysDown[ImGuiKey::ImGuiKey_F] && m_clickedEntityId == id) {
            // EditorCamera::editorCamera.position = transCom->LocalTransformation.position;
            EditorCamera::editorCamera.target = transCom->LocalTransformation.position;
            EditorCamera::editorCamera.r = glm::length(EditorCamera::editorCamera.position - EditorCamera::editorCamera.target);
            EditorCamera::editorCamera.alpha = glm::asin((EditorCamera::editorCamera.position.y - EditorCamera::editorCamera.target.y) / EditorCamera::editorCamera.r);
            EditorCamera::editorCamera.betta = std::atan2(EditorCamera::editorCamera.position.x - EditorCamera::editorCamera.target.x, EditorCamera::editorCamera.position.z - EditorCamera::editorCamera.target.z);
            EditorCamera::editorCamera.SwitchMode(true);

            // Recompute position from spherical coordinates
            EditorCamera::editorCamera.position.x = EditorCamera::editorCamera.target.x + EditorCamera::editorCamera.r * glm::cos(EditorCamera::editorCamera.alpha) * glm::sin(EditorCamera::editorCamera.betta);
            EditorCamera::editorCamera.position.y = EditorCamera::editorCamera.target.y + EditorCamera::editorCamera.r * glm::sin(EditorCamera::editorCamera.alpha);
            EditorCamera::editorCamera.position.z = EditorCamera::editorCamera.target.z + EditorCamera::editorCamera.r * glm::cos(EditorCamera::editorCamera.alpha) * glm::cos(EditorCamera::editorCamera.betta);
        }

        //draw context window
        if (ImGui::BeginPopupContextItem()) {
            //disable if the upmost prefab
            if (m_prefabSceneMode && (id == ecs->sceneMap.find(m_activeScene)->second.prefabID)) {
              
            }
            else {
                if (ImGui::MenuItem("Delete Entity")) {               
                    ecs->DeleteEntity(id);
                    m_clickedEntityId = -1;
                    ImGui::EndPopup();
                    if (open)ImGui::TreePop();
                    return false;
                }
            }
            
            if (ImGui::MenuItem("Duplicate Entity")) {
                ecs::EntityID newid = ecs->DuplicateEntity(id);

                if (m_prefabSceneMode) {

                    
                    const auto& parent = hierachy::GetParent(id);
                    //if id does not have parent, make it the parent
                    if (!parent.has_value()) {
                        hierachy::m_SetParent(id, newid);
                    }
                    else {
                        hierachy::m_SetParent(parent.value(), newid);
                    }

                   
                }

                ImGui::EndPopup();
                if (open)ImGui::TreePop();
                return false;
            }

            if (ImGui::MenuItem("Create Prefab")) {
                if (!m_prefabSceneMode) {
                    prefab::m_SaveEntitytoPrefab(id);
                }            
            }

            ImGui::EndPopup();
        }

        if (ImGui::BeginDragDropTarget())
        {           
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
            {
                IM_ASSERT(payload->DataSize == sizeof(ecs::EntityID));
                ecs::EntityID childId = *static_cast<ecs::EntityID*>(payload->Data);

                // dont allow prefabs to be dragged inside prefab
                const auto& childnc = ecs->GetComponent<ecs::NameComponent>(childId);
                const auto& parent = ecs->GetComponent<ecs::NameComponent>(id);

                if (!m_prefabSceneMode && childnc->isPrefab && (childnc->prefabName == parent->prefabName)) {

                    LOGGING_WARN("Unable to drag prefabs of same type into each other, pls go to prefab editor");
                }
                else {
                    hierachy::m_SetParent(id, childId, true);
                    LOGGING_INFO("Set Parent: %d, Child: %d", id, childId);
                    // update child's scene
                    hierachy::m_UpdateChildScene(id);

                    //return
                    ImGui::EndDragDropTarget();
                    if (open)ImGui::TreePop();
                    return false;
                }

            }

            ImGui::EndDragDropTarget();
        }
        
        //no reordering of child prefabs
        if (!transCom->m_haveParent || !ecs->GetComponent<ecs::NameComponent>(transCom->m_parentID)->isPrefab ||
            ecs->GetComponent<ecs::NameComponent>(transCom->m_parentID)->prefabName != nc->prefabName || m_prefabSceneMode) {
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                //might undefine behaviour
                ecs::EntityID index = id;
                ImGui::SetDragDropPayload("Entity", &index, sizeof(ecs::EntityID));
                ImGui::Text(nc->entityName.c_str());
                ImGui::EndDragDropSource();
            }
        }        

        if (open) {
            //recursion
            if (transCom->m_childID.size() > 0) {
                for (auto& ids : transCom->m_childID) {
                    if (!DrawEntityNode(ids)) {

                        ImGui::TreePop();
                        return false;
                    }
                }
            }
            // m_DrawEntityNode(1);
            ImGui::TreePop();
        }
        return true;
    }
}

