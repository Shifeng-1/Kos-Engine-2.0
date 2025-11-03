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
#include "ECS/ecs.h"
#include "DeSerialization/json_handler.h"
#include "Resources/ResourceManager.h"
#include "Debugging/Logging.h"
#include "AssetManager/Prefab.h"
#include "Editor/EditorCamera.h"
#include "Scene/SceneManager.h"

namespace gui
{

    unsigned int ImGuiHandler::DrawHierachyWindow()
    {
        std::function<void(EntityID)> updateChildScene = [&](EntityID parent)
        {
            std::string parentscene = m_ecs.GetSceneByEntityID(parent);
            const auto &child = m_ecs.GetChild(parent);

            if (child.has_value())
            {
                auto &children = child.value();
                for (auto &childid : children)
                {
                    // If child scene does not belong to parent scene, swap it
                    std::string childscene = m_ecs.GetSceneByEntityID(childid);
                    if (parentscene != childscene)
                    {
                        m_sceneManager.SwapScenes(childscene, parentscene, childid);
                    }

                    // Recursive call
                    if (m_ecs.GetChild(childid).has_value())
                    {
                        updateChildScene(childid);
                    }
                }
            }
        };

        // assetmanager::AssetManager* assetmanager = assetmanager::AssetManager::m_funcGetInstance();
        //  Custom window with example widgets
        ImGui::Begin("Hierachy Window", nullptr, ImGuiWindowFlags_MenuBar);

        // std::string ObjectCountStr = "Oject Count: " + std::to_string(m_ecs_EntityMap.size());
        // ImGui::Text(ObjectCountStr.c_str());

        static std::string searchString;
        if (m_prefabSceneMode)
            searchString.clear();

        // menu bar for search

        if (ImGui::BeginMenuBar())
        {
            ImGui::Text("Search:");
            ImGui::SameLine(); // Keep the next widget on the same line
            if (ImGui::InputText("###1234Search", &searchString))
            {
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
                if (!m_activeScene.empty())
                {
                    ecs::EntityID newEntityID = m_ecs.CreateEntity(m_activeScene);

                    // if in prefab mode, assign entity to upmost parent
                    if (m_prefabSceneMode)
                    {
                        ecs::EntityID id = m_ecs.sceneMap.find(m_activeScene)->second.prefabID;
                        m_ecs.SetParent(std::move(id), newEntityID);
                    }

                    m_clickedEntityId = newEntityID;

                    m_ecs.GetComponent<ecs::NameComponent>(newEntityID)->entityName = std::string(m_charBuffer);

                    m_charBuffer[0] = '\0';
                    m_objectNameBox = false;
                }
                else
                {
                    LOGGING_POPUP("No Scene Loaded");
                }
            }
        }

        int imguiId = 0;
        static ImVec2 headerPos, headerEnd;
        for (auto &[sceneName, sceneentity] : m_ecs.sceneMap)
        {
            // when prefab mode is on, skip non prefabs, and non active prefabs
            if (m_prefabSceneMode)
            {
                if (sceneentity.isPrefab == false || sceneentity.isActive == false)
                    continue;
                // create seperator
                ImGui::SeparatorText("Prefab");

                if (ImGui::Button("Back"))
                {
                    // save "prefab"
                    m_sceneManager.SaveScene(m_activeScene);
                    std::string prefabName = m_activeScene;
                    // set current prefab back to inactive
                    m_sceneManager.SetSceneActive(m_activeScene, false);

                    // set back scene's active state
                    for (const auto &[scene, sceneData] : m_ecs.sceneMap)
                    {
                        if (sceneData.isPrefab == false)
                        {
                            m_sceneManager.SetSceneActive(scene, m_savedSceneState.find(scene)->second);
                        }
                    }

                    // set back active scene
                    for (auto &scene : m_ecs.sceneMap)
                    {
                        if (!scene.second.isPrefab)
                        {
                            m_activeScene = scene.first;
                            break;
                        }
                    }

                    m_prefabSceneMode = false;
                    m_clickedEntityId = -1;

                    // Instead of Updating all the time, Differences needs to be checked.
                    // Prompt when leaving prefab mode,
                    /*std::map<EntityID, std::vector<std::string>> allDiffList;
                    const auto &prefabscene = m_ecs.sceneMap.find(prefabName);
                    for (const auto &id : m_ecs.GetEntitySignatureData())
                    {
                        ecs::NameComponent *nc = m_ecs.GetComponent<ecs::NameComponent>(id.first);
                        if (nc->isPrefab && (nc->prefabName == prefabName))
                        {
                            std::vector<std::string> diffList;
                            m_prefabManager.RefreshComponentDifferenceList(diffList, id.first);
                            if (diffList.size())
                                allDiffList.emplace(id.first, diffList);
                        }
                    }

                    for (auto &[id, diffList] : allDiffList)
                    {
                        for (const auto &compName : diffList)
                        {
                            if (compName == ecs::NameComponent::classname() || compName == ecs::TransformComponent::classname())
                                continue;
                            m_prefabManager.RevertToPrefab_Component(id, compName, prefabName);
                        }
                    }*/
                    // m_ecs.DeleteEntity(duppedID); // Dupping Somehow causes us to update all the prefab scenes?
                    // duppedID = -1;

                    // for (const auto& id : m_ecs.GetEntitySignatureData()) {
                    //     ecs::NameComponent* nc = m_ecs.GetComponent<ecs::NameComponent>(id.first);
                    //     if (nc->isPrefab && (nc->prefabName == prefabName)) {
                    //         for (const auto& compName : diffList) {
                    //             if (compName == ecs::NameComponent::classname()) continue;
                    //             prefab::RevertToPrefab_Component(id.first, compName, prefabName);
                    //         }
                    //     }
                    // }
                }
            }

            // skip if prefab is not active
            if (sceneentity.isPrefab == true && sceneentity.isActive == false)
                continue;
            headerPos = ImGui::GetCursorPos();
            std::string headerstr = sceneName.substr(0, sceneName.find_last_of('.'));
            // collapsing header for scene
            bool opens{};
            if (sceneentity.isActive == false)
            {
                headerstr += " (Unloaded)";
                ImGui::CollapsingHeader(headerstr.c_str());
            }
            else
            {
                if (sceneName == m_activeScene)
                {
                    headerstr += " (Active)";
                }
                opens = ImGui::CollapsingHeader(headerstr.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
            }

            // Right Click Section
            if (ImGui::BeginPopupContextItem())
            {
                if ((sceneName != m_activeScene) && ImGui::MenuItem("Remove Scene"))
                {
                    m_sceneManager.ClearScene(sceneName);

                    // break loop
                    ImGui::EndPopup();
                    break;
                }

                if ((sceneName != m_activeScene) && (sceneentity.isActive == true) && ImGui::MenuItem("Unload Scene"))
                {
                    m_sceneManager.SetSceneActive(sceneName, false);
                    m_clickedEntityId = -1;

                    if (!m_prefabSceneMode)
                    {
                        // change scene if current active scene is unloaded
                        if (m_ecs.sceneMap.find(m_activeScene)->second.isActive == false)
                        {
                            // set first loaded scene as active
                            for (auto &scene : m_ecs.sceneMap)
                            {
                                if (scene.second.isActive == true && scene.second.isPrefab == false)
                                {
                                    m_activeScene = m_ecs.sceneMap.begin()->first;
                                }
                            }
                        }
                    }

                    ImGui::EndPopup();
                    break;
                }

                if ((sceneentity.isActive == false) && ImGui::MenuItem("load Scene"))
                {
                    m_sceneManager.SetSceneActive(sceneName, true);
                    ImGui::EndPopup();
                    break;
                }

                if (ImGui::MenuItem("Save Scene"))
                {
                    onSaveAll.Invoke(sceneName);
                }

                if ((sceneName != m_activeScene) && ImGui::MenuItem("Set Active"))
                {
                    m_activeScene = sceneName;

                    if (sceneentity.isActive == false)
                    {
                        sceneentity.isActive = true;
                    }
                }

                ImGui::EndPopup();
            }

            // Covers the Collapsing Header Space
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Entity"))
                {
                    IM_ASSERT(payload->DataSize == sizeof(ecs::EntityID));
                    ecs::EntityID Id = *static_cast<ecs::EntityID *>(payload->Data);
                    const auto &scene = m_ecs.GetSceneByEntityID(Id);

                    if (!scene.empty())
                    {
                        m_sceneManager.SwapScenes(scene, sceneName, Id);
                    }

                    // if entity is a child, break from parent
                    const auto &parent = m_ecs.GetParent(Id);
                    if (parent.has_value())
                    {
                        m_ecs.RemoveParent(Id);
                    }

                    updateChildScene(Id);
                }

                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("file"))
                {
                    // IM_ASSERT(payload->DataSize == sizeof(std::filesystem::path));
                    std::filesystem::path filename = static_cast<const char *>(payload->Data);

                    if (filename.filename().extension().string() == ".prefab")
                    {
                        m_prefabManager.m_CreatePrefab(filename.filename().string(), sceneName);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (opens)
            {
                for (auto entity : sceneentity.sceneIDs)
                {

                    const auto &entityMap = m_ecs.GetEntitySignatureData();
                    if (entityMap.find(m_clickedEntityId) == entityMap.end())
                    {
                        m_clickedEntityId = entity;
                        m_isUi = false;
                    }

                    // search bar if if string not empty, must match the entity name
                    ecs::NameComponent *nc = m_ecs.GetComponent<ecs::NameComponent>(entity);
                    if (!searchString.empty() && !containsSubstring(nc->entityName, searchString))
                        continue;

                    // draw parent entity node
                    // draw entity with no parents hahaha
                    if (!m_ecs.GetParent(entity).has_value())
                    {
                        if (DrawEntityNode(entity) == false)
                        {
                            // delete is called
                            break;
                        }
                    }
                }
                headerEnd = ImGui::GetCursorPos();

                // Covers the Scene TreeNodes + Collapsing Header
                ImGui::SetCursorPos(headerPos);
                ImGui::PushID(imguiId++);
                ImGui::InvisibleButton("#invbut", ImVec2{ImGui::GetContentRegionMax().x, headerEnd.y - headerPos.y});
                ImGui::PopID();
                ImGui::SetCursorPos(headerEnd);
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("EntityPayload"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(EntityPayload));
                        ecs::EntityID Id = static_cast<EntityPayload *>(payload->Data)->id;

                        // if in prefab mode and parent does not have parent, reject
                        if (m_prefabSceneMode && m_ecs.GetParent(Id).has_value() && (!m_ecs.GetParent(m_ecs.GetParent(Id).value()).has_value()))
                        {
                        }
                        if (m_prefabSceneMode)
                        {
                            m_ecs.SetParent(m_ecs.sceneMap.find(m_activeScene)->second.prefabID, Id);
                        }
                        else
                        {
                            m_ecs.RemoveParent(Id, true);
                        }
                    }

                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("file"))
                    {
                        // IM_ASSERT(payload->DataSize == sizeof(std::filesystem::path));
                        IM_ASSERT(payload->DataSize == sizeof(AssetPayload));
                        const AssetPayload *data = static_cast<const AssetPayload *>(payload->Data);

                        std::filesystem::path filePath = data->path;

                        if (!m_prefabSceneMode && filePath.filename().extension().string() == ".json")
                        {
                            m_sceneManager.LoadScene(filePath);
                        }

                        if (!m_prefabSceneMode && filePath.filename().extension().string() == ".prefab")
                        {
                            m_prefabManager.m_CreatePrefab(filePath.filename().string(), sceneName);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
            }
        }
        ImGui::End();
        return m_clickedEntityId;
    }

    bool ImGuiHandler::DrawEntityNode(ecs::EntityID id)
    {

        std::function<void(EntityID)> updateChildScene = [&](EntityID parent)
        {
            std::string parentscene = m_ecs.GetSceneByEntityID(parent);
            const auto &child = m_ecs.GetChild(parent);

            if (child.has_value())
            {
                auto &children = child.value();
                for (auto &childid : children)
                {
                    // If child scene does not belong to parent scene, swap it
                    std::string childscene = m_ecs.GetSceneByEntityID(childid);
                    if (parentscene != childscene)
                    {
                        m_sceneManager.SwapScenes(childscene, parentscene, childid);
                    }

                    // Recursive call
                    if (m_ecs.GetChild(childid).has_value())
                    {
                        updateChildScene(childid);
                    }
                }
            }
        };

        ecs::TransformComponent *transCom = m_ecs.GetComponent<ecs::TransformComponent>(id);
        if (transCom == NULL)
            return false;

        ImGuiTreeNodeFlags flag = ((static_cast<unsigned int>(m_clickedEntityId) == id) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
        if (transCom->m_childID.size() <= 0)
        {
            flag |= ImGuiTreeNodeFlags_Leaf;
        }

        ecs::NameComponent *nc = m_ecs.GetComponent<ecs::NameComponent>(id);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::InvisibleButton(std::string{"##invireorderbutton" + std::to_string(id)}.c_str(), ImVec2{ImGui::GetContentRegionAvail().x, 1.f});
        ImGui::PopStyleVar();

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("EntityPayload"))
            {
                IM_ASSERT(payload->DataSize == sizeof(EntityPayload));
                ecs::EntityID SwapId = static_cast<EntityPayload *>(payload->Data)->id;

                const std::string swapScene = m_ecs.GetSceneByEntityID(SwapId);
                const std::string idScene = m_ecs.GetSceneByEntityID(id);

                if (SwapId != id && swapScene == idScene)
                {

                    auto &_scene = m_ecs.sceneMap.find(swapScene)->second;
                    const auto &eraseit = std::find(_scene.sceneIDs.begin(), _scene.sceneIDs.end(), SwapId);

                    if (eraseit != _scene.sceneIDs.end())
                    {
                        _scene.sceneIDs.erase(eraseit);
                    }

                    const auto &IDit = std::find(_scene.sceneIDs.begin(), _scene.sceneIDs.end(), id);

                    if (IDit != _scene.sceneIDs.end())
                    {
                        _scene.sceneIDs.insert(IDit, SwapId);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        // create color if prefab
        if (nc->isPrefab)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.2f, 0.1f, 1.0f));
        }
        else if (nc->hide)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        }
        bool open = ImGui::TreeNodeEx(std::to_string(id).c_str(), flag, nc->entityName.c_str());
        if (nc->isPrefab || nc->hide)
            ImGui::PopStyleColor();

        if (ImGui::IsItemClicked())
        {
            m_clickedEntityId = id;
            m_isUi = false;
            // Check if it houses any ui elements
            if (m_ecs.HasComponent<ecs::CanvasRendererComponent>(id) || (m_ecs.GetParent(m_clickedEntityId).has_value() &&
                                                                         m_ecs.HasComponent<ecs::CanvasRendererComponent>(m_ecs.GetParent(m_clickedEntityId).value())))
            {
                // std::cout << "IS UI\n";
                m_isUi = true;
            }
        }

        if (ImGui::GetIO().KeysDown[ImGuiKey::ImGuiKey_F] && m_clickedEntityId == id)
        {
            // EditorCamera::editorCamera.position = transCom->LocalTransformation.position;
            EditorCamera::editorCamera.target = transCom->WorldTransformation.position;
            EditorCamera::editorCamera.r = glm::length(EditorCamera::editorCamera.position - EditorCamera::editorCamera.target);
            EditorCamera::editorCamera.alpha = glm::asin((EditorCamera::editorCamera.position.y - EditorCamera::editorCamera.target.y) / EditorCamera::editorCamera.r);
            EditorCamera::editorCamera.betta = std::atan2(EditorCamera::editorCamera.position.x - EditorCamera::editorCamera.target.x, EditorCamera::editorCamera.position.z - EditorCamera::editorCamera.target.z);
            EditorCamera::editorCamera.SwitchMode(true);

            // Recompute position from spherical coordinates
            EditorCamera::editorCamera.position.x = EditorCamera::editorCamera.target.x + EditorCamera::editorCamera.r * glm::cos(EditorCamera::editorCamera.alpha) * glm::sin(EditorCamera::editorCamera.betta);
            EditorCamera::editorCamera.position.y = EditorCamera::editorCamera.target.y + EditorCamera::editorCamera.r * glm::sin(EditorCamera::editorCamera.alpha);
            EditorCamera::editorCamera.position.z = EditorCamera::editorCamera.target.z + EditorCamera::editorCamera.r * glm::cos(EditorCamera::editorCamera.alpha) * glm::cos(EditorCamera::editorCamera.betta);
        }

        // draw context window
        if (ImGui::BeginPopupContextItem())
        {
            // disable if the upmost prefab
            if (m_prefabSceneMode && (id == m_ecs.sceneMap.find(m_activeScene)->second.prefabID))
            {
            }
            else
            {
                if (ImGui::MenuItem("Delete Entity"))
                {
                    m_ecs.DeleteEntity(id);
                    m_clickedEntityId = -1;
                    ImGui::EndPopup();
                    if (open)
                        ImGui::TreePop();
                    return false;
                }
            }

            if (ImGui::MenuItem("Duplicate Entity"))
            {
                ecs::EntityID newid = m_ecs.DuplicateEntity(id);

                if (m_prefabSceneMode)
                {
                    const auto &parent = m_ecs.GetParent(id);
                    // if id does not have parent, make it the parent
                    if (!parent.has_value())
                    {
                        m_ecs.SetParent(id, newid);
                    }
                    else
                    {
                        m_ecs.SetParent(parent.value(), newid);
                    }
                }

                ImGui::EndPopup();
                if (open)
                    ImGui::TreePop();
                return false;
            }

            if (ImGui::MenuItem("Create Prefab"))
            {
                if (!m_prefabSceneMode)
                {
                    m_prefabManager.m_SaveEntitytoPrefab(id);
                }
            }

            ImGui::EndPopup();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("EntityPayload"))
            {
                IM_ASSERT(payload->DataSize == sizeof(EntityPayload));
                ecs::EntityID childId = static_cast<EntityPayload *>(payload->Data)->id;

                // dont allow prefabs to be dragged inside prefab
                const auto &childnc = m_ecs.GetComponent<ecs::NameComponent>(childId);
                const auto &parent = m_ecs.GetComponent<ecs::NameComponent>(id);

                if (!m_prefabSceneMode && childnc->isPrefab && (childnc->prefabName == parent->prefabName))
                {

                    LOGGING_WARN("Unable to drag prefabs of same type into each other, pls go to prefab editor");
                }
                else
                {
                    m_ecs.SetParent(id, childId, true);
                    LOGGING_INFO("Set Parent: %d, Child: %d", id, childId);
                    // update child's scene
                    updateChildScene(id);

                    // return
                    ImGui::EndDragDropTarget();
                    if (open)
                        ImGui::TreePop();
                    return false;
                }
            }

            ImGui::EndDragDropTarget();
        }

        // no reordering of child prefabs
        if (!transCom->m_haveParent || !m_ecs.GetComponent<ecs::NameComponent>(transCom->m_parentID)->isPrefab ||
            m_ecs.GetComponent<ecs::NameComponent>(transCom->m_parentID)->prefabName != nc->prefabName || m_prefabSceneMode)
        {
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                auto *nc = m_ecs.GetComponent<ecs::NameComponent>(id);
                if (nc)
                {
                    EntityPayload payload{id, nc->entityGUID};

                    ImGui::SetDragDropPayload("EntityPayload", &payload, sizeof(EntityPayload));
                    ImGui::Text("%s", nc->entityName.c_str());
                    if (!nc->entityGUID.Empty())
                        ImGui::Text("%s", nc->entityGUID.GetToString().c_str());
                }
                ImGui::EndDragDropSource();
            }
        }

        if (open)
        {
            // recursion
            if (transCom->m_childID.size() > 0)
            {
                for (auto &ids : transCom->m_childID)
                {
                    if (!DrawEntityNode(ids))
                    {

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
