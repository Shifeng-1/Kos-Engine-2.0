/******************************************************************/
/*!
\file      imgui_component_window.cpp
\author    Jaz Winn Ng, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Oct 02, 2025
\brief     This file implements the ImGui component window for handling
           user interactions with ECS components in the application. It
           enables the addition, display, and modification of various
           entity components within the ECS framework.

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
#include "Resources/ResourceManager.h"
#include "AssetManager/Prefab.h"
#include "ECS/Layers.h"
#include "Editor/WindowFile.h"

//scripts
#include "Reflection/Field.h"


void DrawFieldComponent(ecs::Component* component, const std::string& ComponentName, ecs::EntityID entityID);

void gui::ImGuiHandler::DrawComponentWindow()
{
    bool windowOpen = true;
    std::string Title = "Component Window";

    std::string windowTitle = Title;

    ImGui::Begin(windowTitle.c_str(), &windowOpen);

    //Add Component Window

    if (m_ecs->GetEntitySignatureData().size() > 0 && m_clickedEntityId >= 0) {

        ecs::EntityID entityID = m_clickedEntityId;
        ecs::ComponentSignature EntitySignature = m_ecs->GetEntitySignature(entityID);

        const auto& componentsString = m_ecs->GetComponentsString();
        static std::vector<const char*>componentNames = {"add Components"};
        static bool initComponentName = false;

        if (!initComponentName) {
            for (const auto& names : componentsString) {
                componentNames.push_back(names.c_str());
            }
            initComponentName = true;
        }


       

        int ComponentTypeIndex = 0;
        if (ImGui::Combo("##ADDCOMPONENT", &ComponentTypeIndex, componentNames.data(), static_cast<int>(componentNames.size()))) {
            std::string componentName = componentNames[ComponentTypeIndex];
            if (!m_ecs->GetEntitySignature(entityID).test(m_ecs->GetComponentKey(componentName))) {

                auto& action = m_ecs->componentAction.at(componentName);
				action->AddComponent(entityID);

            }
        }

        ImGui::SeparatorText("Components");

        if (EntitySignature.test(m_ecs->GetComponentKey(ecs::NameComponent::classname())))
        {
            // Retrieve the TransformComponent
            ecs::NameComponent* nc = m_ecs->GetComponent<ecs::NameComponent>(entityID);
            //Display Position
            ImGui::AlignTextToFramePadding();  // Aligns text to the same baseline as the slider
            ImGui::Text("Object Name: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100.0f);
            ImGui::InputText("##NAMETEXT##", &nc->entityName);
            ImGui::SameLine();

            bool hidden = nc->hide;
            if (ImGui::Checkbox("Hide", &hidden)) {
                if (!nc->hide) {
                    m_ecs->layersStack.m_hideEntitywithChild(entityID);
                }
                else {
                    m_ecs->layersStack.m_unhideEntitywithChild(entityID);
                }

            }
            if (nc->isPrefab && !m_prefabSceneMode) {
                auto* tc = m_ecs->GetComponent<ecs::TransformComponent>(entityID);
                if (!tc->m_haveParent) {
                    ImGui::SameLine();
                    if (ImGui::Button("Overwrite")) {
                        // Look for component of prefab and set data from me
                        try {
                            prefab::OverwriteScenePrefab(m_clickedEntityId);
                            prefab::UpdateAllPrefab(nc->prefabName);
                        }
                        catch (...) {
                            LOGGING_ERROR("Prefab overwrite, failed");
                        }

                        ImGui::End();
                        return;
                    }
                }
            }
            
            ImGui::TextDisabled(std::string( "Entity ID: " + std::to_string(entityID)).c_str());

            {
                //layer selector
                static constexpr auto enumNames = magic_enum::enum_names<layer::LAYERS>();

                // Persistent storage for null-terminated strings
                static std::array<std::string, enumNames.size()> namesStr{};
                static std::array<const char*, enumNames.size()> items{};

                static bool initialized = false;
                if (!initialized) {
                    for (size_t i = 0; i < enumNames.size(); i++) {
                        namesStr[i] = std::string(enumNames[i]);  // store the string
                        items[i] = namesStr[i].c_str();           // pointer is valid now
                    }
                    initialized = true;
                }

                int layer_current = nc->Layer;
                if (ImGui::Combo("Layers", &layer_current, items.data(), static_cast<int>(items.size()))) {
                    m_ecs->layersStack.m_SwapEntityLayer((layer::LAYERS)layer_current, nc->Layer, entityID);
                }
            }
            
            {
                // Convert vector to array of char* for ImGui
                std::vector<const char*> tag_Names(m_tags.size());
                std::transform(m_tags.begin(), m_tags.end(), tag_Names.begin(), [](const std::string& tag) {  return tag.c_str(); });

                int item{};
                const auto& it = std::find(tag_Names.begin(), tag_Names.end(), nc->entityTag);
                if (it != tag_Names.end()) {
                    item = static_cast<int>(std::distance(tag_Names.begin(), it));
                }
                else {
                    tag_Names.push_back(nc->entityTag.c_str());
                    const auto& it2 = std::find(tag_Names.begin(), tag_Names.end(), nc->entityTag);
                    item = static_cast<int>(std::distance(tag_Names.begin(), it2));
                }

                if (ImGui::Combo("Tag", &item, tag_Names.data(), static_cast<int>(tag_Names.size()))) {
                    nc->entityTag = m_tags[item];
                }
            }

            {
                // std::cout << nc->entityTag << std::endl;
                            //create overwrite button for prefab
                if (nc->isPrefab && !m_prefabSceneMode) {
                    auto* tc = m_ecs->GetComponent<ecs::TransformComponent>(entityID);
                    if (!tc->m_haveParent || !m_ecs->GetComponent<ecs::NameComponent>(tc->m_parentID)->isPrefab) {
                        static bool isHeaderOpen = false;
                        static std::vector<std::string> diffComp;
                        bool open = false;
                        std::string headerName = nc->prefabName + " [Changed]";
                        int IMGUI_ID = 0;
                        if (open = ImGui::CollapsingHeader(headerName.c_str())) {
                            for (const auto& compName : diffComp) {
                                if (compName == ecs::NameComponent::classname()) continue;
                                ImGui::TextDisabled(compName.c_str());
                                ImGui::SameLine();
                                ImGui::PushID(IMGUI_ID++);
                                if (ImGui::Button("Revert")) {
                                    // Look for component of prefab and set data into me
                                    prefab::RevertToPrefab_Component(entityID, compName, nc->prefabName);
                                    prefab::RefreshComponentDifferenceList(diffComp, entityID);
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("Overwrite")) {
                                    // Look for component of prefab and set data from me
                                    prefab::OverwritePrefab_Component(entityID, compName, nc->prefabName);
                                    prefab::RefreshComponentDifferenceList(diffComp, entityID);
                                }
                                ImGui::PopID();
                            }
                            //if (ImGui::Button("Overwrite All")) {
                            //    prefab::m_OverWriteScenePrefab(entityID);
                            //}
                            //ImGui::SameLine();
                            //if (ImGui::Checkbox("Sync", &nc->syncPrefab)) {
                            //    prefab::m_UpdateAllPrefabEntity(nc->prefabName);
                            //}
                        }

                        if (isHeaderOpen != open) { // Needed to show change in state
                            if (open) {
                                prefab::RefreshComponentDifferenceList(diffComp, entityID);
                            }
                            else {
                                diffComp.clear();
                            }
                            isHeaderOpen = open;
                        }
                    }

                }
            }




           
        }

        const auto& componentKey = m_ecs->GetComponentKeyData();
        int ImguiID = 0;
        for (auto it = componentKey.rbegin(); it != componentKey.rend(); ++it) {
            const auto& ComponentName = it->first;
            auto key = it->second;

            if (EntitySignature.test(key) && ComponentName != ecs::NameComponent::classname()) {
                auto* component = m_ecs->GetIComponent<ecs::Component*>(ComponentName, entityID);

                ImGui::PushID(ImguiID++);
                if (componentDrawers.find(ComponentName) != componentDrawers.end()) {
                    auto& editorAction = componentDrawers[ComponentName];
                    editorAction->Draw(component);
                }
                else {  
                   // auto& actionMap = GetComponentActionMap();
                    //scrpt components
                    DrawFieldComponent(component, ComponentName, entityID);
 
                }
                ImGui::PopID();


            }
        }

        //draw invinsible box
        if (ImGui::GetContentRegionAvail().x > 0 && ImGui::GetContentRegionAvail().y > 0) {

            ImGui::InvisibleButton("##Invinsible", ImVec2{ ImGui::GetContentRegionAvail().x,ImGui::GetContentRegionAvail().y });

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file"))
                {
                    IM_ASSERT(payload->DataSize == sizeof(std::filesystem::path));
                    std::filesystem::path* filename = static_cast<std::filesystem::path*>(payload->Data);
                }
                ImGui::EndDragDropTarget();
            }
        }

     }
    ImGui::End();
}   


void DrawFieldComponent(ecs::Component* component, const std::string& ComponentName, ecs::EntityID entityID) {
    auto fields = FieldSingleton::GetInstance()->GetAction();

    if (fields.find(ComponentName) == fields.end()) return;

    auto& action = fields.at(ComponentName);
    bool open = ImGui::CollapsingHeader(ComponentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

    if (ImGui::BeginPopupContextItem()) {
        auto action = ecs::ECS::GetInstance()->componentAction.at(ComponentName);
        if (ComponentName != ecs::TransformComponent::classname() && ImGui::MenuItem("Delete Component")) {
            action->RemoveComponent(entityID);
        }
        if (ImGui::MenuItem("Reset Component")) {

            action->ResetComponent(entityID);
        }

        ImGui::EndPopup();
    }

    if (open) {


        auto fieldReference = action->GetFieldReference(component);
        auto names = action->GetNames();
        DrawComponents draw(names);

        for (auto& field : fieldReference) {
            

            if (field.IsType<float>()) {
                draw(field.AsType<float>());
            }
            else if (field.IsType<int>()) {
                draw(field.AsType<int>());
            }
            else if (field.IsType<bool>()) {
                draw(field.AsType<bool>());
            }
            else if (field.IsType<std::string>()) {
                draw(field.AsType<std::string>());
            }
            else if (field.IsType<glm::vec3>()) {
                draw(field.AsType<glm::vec3>());
            }
            else if (field.IsType<glm::vec2>()) {
                draw(field.AsType<glm::vec2>());
            }
            else if (field.IsType<glm::vec4>()) {
                draw(field.AsType<glm::vec4>());
            }
            else if (field.IsType<std::vector<std::string>>()) {
                draw(field.AsType<std::vector<std::string>>());
            }
            else if (field.IsType<std::vector<glm::vec3>>()) {
                draw(field.AsType<std::vector<glm::vec3>>());
            }
            else if (field.IsType<std::vector<glm::vec2>>()) {
                draw(field.AsType<std::vector<glm::vec2>>());
            }
            else if (field.IsType<std::vector<glm::vec4>>()) {
                draw(field.AsType<std::vector<glm::vec4>>());
            }

            else {
                draw.count++;
            }

        }

    }
}
