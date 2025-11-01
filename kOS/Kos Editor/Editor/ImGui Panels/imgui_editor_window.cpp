/******************************************************************/
/*!
\file      imgui_editor_window.cpp
\author    Sean Tiu
\par       s.tiu@digipen.edu
\date      8th November, 2024
\brief     This file implements the editor window interface for the
           application, using ImGui for the graphical user interface.

           The editor window provides an interactive interface for the
           user to manipulate and view objects in the application's
           scene, such as entities, cameras, and textures. It includes
           functionality for rendering the scene in a window, zooming,
           dragging, and resetting the camera view. Additionally, it
           supports drag-and-drop of assets (such as .png, .ttf, and
           .prefab files) into the editor window, automatically creating
           entities and setting their properties based on the file type.

           Key features of this file:
           - Draws the editor window and render screen.
           - Handles camera zoom, drag, and reset controls.
           - Manages drag-and-drop functionality for asset files.
           - Updates camera matrices and view transformations.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/



#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Editor.h"
#include "imgui_internal.h"


#include "ECS/ECS.h"
#include "Application/Application.h"


#include "Editor/EditorCamera.h"
#include "Resources/ResourceManager.h"
#include "AssetManager/Prefab.h"

#include "ECS/Hierachy.h"

#include "Graphics/GraphicsManager.h"


void gui::ImGuiHandler::DrawRenderScreenWindow(unsigned int windowWidth, unsigned int windowHeight)
{
    ecs::ECS* ecs = ecs::ECS::GetInstance();

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_MenuBar;
    bool open = true;

    ImGui::Begin("Editor Window", &open, window_flags);

    DrawPlayPauseBar();

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 renderWindowSize = ImGui::GetContentRegionAvail();

    float textureAspectRatio = (float)windowWidth / (float)windowHeight;
    float renderWindowAspectRatio = renderWindowSize.x / renderWindowSize.y;

    ImVec2 imageSize;
    imageSize.x = windowWidth / 2.f;
    imageSize.y = windowHeight / 2.f;

    //Dynamic Window Resizing
    if (renderWindowAspectRatio > textureAspectRatio)
    {
        imageSize.y = renderWindowSize.y;
        imageSize.x = imageSize.y * textureAspectRatio;
    }
    else
    {
        imageSize.x = renderWindowSize.x;
        imageSize.y = imageSize.x / textureAspectRatio;
    }

    if (imageSize.x <= renderWindowSize.x)
    {
        pos.x += (renderWindowSize.x - imageSize.x) / 2;
    }

    if (imageSize.y <= renderWindowSize.y)
    {
        pos.y += (renderWindowSize.y - imageSize.y) / 2;
    }


    //pipe->m_renderFinalPassWithDebug();
    ImVec2 pMax(pos.x + imageSize.x, pos.y + imageSize.y);

    // std::cout << imageSize.x << ' ' << imageSize.y;
    //ImGui::GetWindowDrawList()->AddImage(
    //    (void*)(GLuint)GraphicsManager::GetInstance()->gm_GetEditorBuffer().texID,
    //    pos, pMax,
    //    ImVec2(0, 1), ImVec2(1, 0));
    const FrameBuffer* fbAdd = &GraphicsManager::GetInstance()->gm_GetEditorBuffer();
    ImGui::GetWindowDrawList()->AddImage(
        reinterpret_cast<void*>(static_cast<uintptr_t>(fbAdd->texID)),
        pos, pMax,
        ImVec2(0, 1), ImVec2(1, 0));
    //Get mouse position
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGuizmo::IsOver()) {
        // Mouse click relative to image
        float relX = (mousePos.x - pos.x) / (pMax.x - pos.x);
        float relY = (mousePos.y - pos.y) / (pMax.y - pos.y);
        //Clamp
        relX = std::clamp(relX, 0.0f, 1.0f);
        relY = std::clamp(relY, 0.0f, 1.0f);

        int pixelX = static_cast<int>(relX * fbAdd->width);
        int pixelY = -(static_cast<int>(relY * fbAdd->height) - fbAdd->height);

        //Get texture data
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        // Bind your texture to the FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GraphicsManager::GetInstance()->gm_GetFBM()->gBuffer.gMaterial, 0);
        // Read just one pixel
        float pixelVal;
        glReadPixels(pixelX, pixelY, 1, 1, GL_ALPHA, GL_FLOAT, &pixelVal);
        
        //std::cout << "Clicked pixerl val is " << --pixelVal << '\n';
        --pixelVal;
        m_clickedEntityId =pixelVal>=0? pixelVal: m_clickedEntityId;
        std::cout << "PixelVal is " << pixelVal << '\n';
        if (ecs->HasComponent<ecs::CanvasRendererComponent>(pixelVal)
            || (hierachy::GetParent(m_clickedEntityId).has_value() &&
                ecs->HasComponent<ecs::CanvasRendererComponent>(hierachy::GetParent(m_clickedEntityId).value()))) {
            std::cout << "IS UI\n";
            m_isUi = true;
        }
        else {
            m_isUi = false;
        }
        //Get texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
    }
    //Test clicking
    //EditorCamera::m_editorWindowPosition.x = pos.x;
    //EditorCamera::m_editorWindowPosition.y = pos.y;
    //EditorCamera::m_editorWindowDimensions.x = imageSize.x;
    //EditorCamera::m_editorWindowDimensions.y = imageSize.y;

    //auto calculateworld = [pos, imageSize]()-> glm::vec3 {
    //    float screencordX = ImGui::GetMousePos().x - pos.x;
    //    float screencordY = ImGui::GetMousePos().y - pos.y;

    //    //TODO calculate mouse pos correctly
    //    float cordX = (screencordX - imageSize.x / 2.f) / (imageSize.x / 2.f);
    //    float cordY = (std::abs(screencordY) - imageSize.y / 2.f) / (imageSize.y / 2.f);

    //    glm::vec3 translate = { cordX , -cordY, 0.f };
    //    translate.x *= EditorCamera::m_editorCameraMatrix[0][0];
    //    translate.y *= EditorCamera::m_editorCameraMatrix[1][1];
    //    translate.x *= 1.f / graphicpipe::GraphicsCamera::m_aspectRatio;
    //    translate.x += EditorCamera::m_editorCameraMatrix[2][0];
    //    translate.y += EditorCamera::m_editorCameraMatrix[2][1];

    //    return translate;
    //    };

    //EditorCamera::calculateLevelEditorCamera();
    //EditorCamera::calculateLevelEditorView();
    //EditorCamera::calculateLevelEditorOrtho();

    //graphicpipe::GraphicsCamera::m_currCameraMatrix = { EditorCamera::m_editorCameraMatrix[0][0], EditorCamera::m_editorCameraMatrix[0][1] ,EditorCamera::m_editorCameraMatrix[0][2] ,
    //                                                       EditorCamera::m_editorCameraMatrix[1][0] ,EditorCamera::m_editorCameraMatrix[1][1] ,EditorCamera::m_editorCameraMatrix[1][2] ,
    //                                                       EditorCamera::m_editorCameraMatrix[2][0] ,EditorCamera::m_editorCameraMatrix[2][1] ,EditorCamera::m_editorCameraMatrix[2][2] };
    //graphicpipe::GraphicsCamera::m_currViewMatrix = { EditorCamera::m_editorViewMatrix[0][0], EditorCamera::m_editorViewMatrix[0][1], EditorCamera::m_editorViewMatrix[0][2],
    //                                                  EditorCamera::m_editorViewMatrix[1][0] ,EditorCamera::m_editorViewMatrix[1][1] ,EditorCamera::m_editorViewMatrix[1][2] ,
    //                                                  EditorCamera::m_editorViewMatrix[2][0] ,EditorCamera::m_editorViewMatrix[2][1] ,EditorCamera::m_editorViewMatrix[2][2] };

    ////If no Camera, Set Editor Camera as Game Camera
    //if (graphicpipe::GraphicsCamera::m_cameras.size() == 0)
    //{
    //    graphicpipe::GraphicsCamera::m_currCameraScaleX = EditorCamera::m_editorCamera.zoom.x;
    //    graphicpipe::GraphicsCamera::m_currCameraScaleY = EditorCamera::m_editorCamera.zoom.y;
    //    graphicpipe::GraphicsCamera::m_currCameraTranslateX = EditorCamera::m_editorCamera.m_coordinates.x;
    //    graphicpipe::GraphicsCamera::m_currCameraTranslateY = EditorCamera::m_editorCamera.m_coordinates.y;
    //    graphicpipe::GraphicsCamera::m_currCameraRotate = 0.f;
    //}
    //

    DrawGizmo(pos.x, pos.y, imageSize.x, imageSize.y);


    ImGuiIO& io = ImGui::GetIO();
    float scrollInput = io.MouseWheel; // Positive for zoom in, negative for zoom out

    //Zoom In/Out Camera
    if (ImGui::IsWindowHovered() && scrollInput)
    {
        // EditorCamera::m_editorCamera.zoom.x -= scrollInput * EditorCamera::m_editorCameraZoomSensitivity * EditorCamera::m_editorCamera.zoom.x;
        // EditorCamera::m_editorCamera.zoom.y -= scrollInput * EditorCamera::m_editorCameraZoomSensitivity * EditorCamera::m_editorCamera.zoom.y;

       // EditorCamera::m_editorCamera.zoom.x = std::clamp(EditorCamera::m_editorCamera.zoom.x, 0.1f, 100.f);
       // EditorCamera::m_editorCamera.zoom.y = std::clamp(EditorCamera::m_editorCamera.zoom.y, 0.1f, 100.f);
       // EditorCamera::editorCamera.onScroll(0, scrollInput);

        EditorCamera::editorCamera.onScroll(0, scrollInput);
    }
    //For camera stuff
    static bool mouseCon = false;
    static ImVec2 lastMousePos = ImVec2(0, 0);
    static bool firstMouseInput = true;

    if ((ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) && ImGui::IsWindowHovered())
    {
        mouseCon = true;
        firstMouseInput = true;
        lastMousePos = ImGui::GetMousePos();
    }
    else if ((ImGui::IsMouseReleased(ImGuiMouseButton_Right) || ImGui::IsMouseReleased(ImGuiMouseButton_Middle)))
    {
        mouseCon = false;
    }

    if (mouseCon)
    {
        ImVec2 currentMousePos = ImGui::GetMousePos();
        const float cameraSpeed = 0.1; // adjust accordingly
        float sprintMultiplier = 1.f;
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
            sprintMultiplier = 2.5f;
        }
        else {
            sprintMultiplier = 1.f;
        }

        if (ImGui::IsKeyDown(ImGuiKey_W)) {
            if (EditorCamera::editorCamera.orbitMode) EditorCamera::editorCamera.SwitchMode(false);
            EditorCamera::editorCamera.position += sprintMultiplier * cameraSpeed * EditorCamera::editorCamera.direction;
        }
        if (ImGui::IsKeyDown(ImGuiKey_S)) {
            if (EditorCamera::editorCamera.orbitMode) EditorCamera::editorCamera.SwitchMode(false);
            EditorCamera::editorCamera.position -= sprintMultiplier * cameraSpeed * EditorCamera::editorCamera.direction;
        }
        if (ImGui::IsKeyDown(ImGuiKey_A)) {
            if (EditorCamera::editorCamera.orbitMode) EditorCamera::editorCamera.SwitchMode(false);
            EditorCamera::editorCamera.position -= sprintMultiplier * cameraSpeed * glm::normalize(glm::cross(EditorCamera::editorCamera.direction, glm::vec3{ 0.0f, 1.0f, 0.0f }));
        }
        if (ImGui::IsKeyDown(ImGuiKey_D)) {
            if (EditorCamera::editorCamera.orbitMode)EditorCamera::editorCamera.SwitchMode(false);
            EditorCamera::editorCamera.position += sprintMultiplier * cameraSpeed * glm::normalize(glm::cross(EditorCamera::editorCamera.direction, glm::vec3{ 0.0f, 1.0f, 0.0f }));
        }
        if (!firstMouseInput)
        {
            float deltaX = currentMousePos.x - lastMousePos.x;
            float deltaY = currentMousePos.y - lastMousePos.y;

            // Call your camera function with the delta
            if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                EditorCamera::editorCamera.onCursor(deltaX, deltaY);
            }
            else if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) { // Needs to be fixed
                if (std::abs(deltaX) > 0.1f && std::abs(deltaY) > 0.1f) {
                    if (EditorCamera::editorCamera.orbitMode) EditorCamera::editorCamera.SwitchMode(false);
                    glm::vec3 right = glm::normalize(glm::cross(EditorCamera::editorCamera.direction, EditorCamera::editorCamera.up));
                    glm::vec3 up = glm::normalize(glm::cross(right, EditorCamera::editorCamera.direction));
                    EditorCamera::editorCamera.position += (deltaX * right - deltaY * up) * cameraSpeed;
                }
            }
        }

        lastMousePos = currentMousePos;
        firstMouseInput = false;
    }
    //Move Camera Around
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
    {
        //ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);

        //glm::vec2 delta = glm::vec2(mouseDelta.x, mouseDelta.y) * EditorCamera::m_editorCameraDragSensitivity * EditorCamera::m_editorCamera.zoom.x;

        //// Update the camera position
        //EditorCamera::m_editorCamera.m_coordinates.x -= delta.x;
        //EditorCamera::m_editorCamera.m_coordinates.y += delta.y;

        //ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
    }

    //Reset Camera To Center
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_R))
    {
        //EditorCamera::m_editorCamera.m_coordinates.x = 0.f;
        //EditorCamera::m_editorCamera.m_coordinates.y = 0.f;
        //EditorCamera::m_editorCamera.zoom.x = 1.f;
        //EditorCamera::m_editorCamera.zoom.y = 1.f;
    }

    static unsigned int lastEntity{};

    // Clean up behaviours when switching entities
    if (static_cast<int>(lastEntity) != m_clickedEntityId) {
        lastEntity = m_clickedEntityId;
        m_collisionSetterMode = false;
    }

    if (ImGui::IsWindowHovered() && !ImGuizmo::IsUsing() && ImGui::IsMouseClicked(0)) {
        //If cursor selects object, object is selected
        //auto transform = calculateworld();
        //ImVec2 WorldMouse = ImVec2{ transform.x, transform.y };
        ////calculate AABB of each object (active scenes)
        //for (auto& sceneentity : ecs->sceneMap) {

        //    if (!sceneentity.second.isActive) continue;

        //    for (auto& entity : sceneentity.second.sceneIDs) {
        //        //calculate AABB
        //        auto* tc = ecs->GetComponent<ecs::TransformComponent>(entity);
        //        const glm::mat3x3& transformation = tc->transformation;

        //       /* glm::vec2 min, max;

        //        glm::vec2 translation, scale;
        //        float rotation;

        //        mat3x3::Mat3Decompose(transformation, translation, scale, rotation);

        //        max = glm::vec2{ float(translation.x + scale.x * 0.5), float(translation.y + scale.y * 0.5) };
        //        min = glm::vec2{ float(translation.x - scale.x * 0.5), float(translation.y - scale.y * 0.5) };
        //        if ((min.x <= WorldMouse.x && WorldMouse.x <= max.x) &&
        //            (min.y <= WorldMouse.y && WorldMouse.y <= max.y)) {
        //            m_clickedEntityId = entity;
        //            break;
        //        }*/
        //    }
        //}

        //if pos is within any of the object, set that object as active.
    }


    //delete entity
    if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        if (m_clickedEntityId >= 0) {
            ecs::ECS::GetInstance()->DeleteEntity(m_clickedEntityId);
            m_clickedEntityId = -1;
        }
    }

    //For Dragging Assets Into Editor Window
    ImGui::Dummy(renderWindowSize);
    if (ImGui::BeginDragDropTarget()) {
        //if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file")) {
        //    ecs::ECS* ecs = ecs::ECS::GetInstance();

        //    IM_ASSERT(payload->DataSize == sizeof(std::filesystem::path));
        //    std::filesystem::path* filename = static_cast<std::filesystem::path*>(payload->Data);

        //    // glm::vec3 translate = calculateworld();
        //    glm::vec3 translate{};

        //    if (filename->filename().extension().string() == ".png" || filename->filename().extension().string() == ".jpg") {
        //        ecs::EntityID id = ecs->CreateEntity(m_activeScene); //assign to active scene
        //        ecs::TransformComponent* transCom = ecs->GetComponent<ecs::TransformComponent>(id);
        //        transCom->WorldTransformation.position = translate;
        //        // Insert matrix
        //        ecs::NameComponent* nameCom = ecs->GetComponent<ecs::NameComponent>(id);
        //        nameCom->entityName = filename->filename().stem().string();

        //        ecs::SpriteComponent* spriteCom = ecs->AddComponent<ecs::SpriteComponent>(id);

        //        if (m_prefabSceneMode) {
        //            hierachy::m_SetParent(ecs->sceneMap.find(m_activeScene)->second.prefabID, id);
        //        }

        //        m_clickedEntityId = id;
        //    }
        //    if (filename->filename().extension().string() == ".ttf") {

        //        ecs::EntityID id = ecs->CreateEntity(m_activeScene); //assign to active scene
        //        ecs::TransformComponent* transCom = ecs->GetComponent<ecs::TransformComponent>(id);
        //        transCom->WorldTransformation.position = translate;
        //        // Insert matrix
        //        ecs::NameComponent* nameCom = ecs->GetComponent<ecs::NameComponent>(id);
        //        nameCom->entityName = filename->filename().stem().string();

        //        //ADD logic here

        //        if (m_prefabSceneMode) {
        //            hierachy::m_SetParent(ecs->sceneMap.find(m_activeScene)->second.prefabID, id);
        //        }
        //        m_clickedEntityId = id;
        //    }

        //    if (filename->filename().extension().string() == ".mpg" || filename->filename().extension().string() == ".mpeg") {
        //        ecs::EntityID id = ecs->CreateEntity(m_activeScene); //assign to active scene
        //        ecs::TransformComponent* transCom = ecs->GetComponent<ecs::TransformComponent>(id);
        //        transCom->WorldTransformation.position = translate;
        //        // Insert matrix
        //        ecs::NameComponent* nameCom = ecs->GetComponent<ecs::NameComponent>(id);
        //        nameCom->entityName = filename->filename().stem().string();

        //        //ADD logic here

        //        if (m_prefabSceneMode) {
        //            hierachy::m_SetParent(ecs->sceneMap.find(m_activeScene)->second.prefabID, id);
        //        }

        //        m_clickedEntityId = id;
        //    }

        //    if (!m_prefabSceneMode && filename->filename().extension().string() == ".prefab") {//dont allow adding of prefab in prefab 
        //        try {
        //            //check to see if prefab is even loaded
        //            if (ecs->sceneMap.find(filename->filename().string()) != ecs->sceneMap.end()) {
        //                ecs::EntityID id = prefab::m_CreatePrefab(filename->filename().string(), m_activeScene);
        //                ecs::TransformComponent* transCom = ecs->GetComponent<ecs::TransformComponent>(id);
        //                transCom->WorldTransformation.position = translate;
        //            }
        //            else {
        //                LOGGING_ERROR("Prefab not loaded");
        //            }
        //        }
        //        catch (...) {
        //            LOGGING_ERROR("Prefab fail to load");
        //        }
        //    }
        //}
        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}
