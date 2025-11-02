/******************************************************************/
/*!
\file      imgui_game_scene_window.cpp
\author    Sean Tiu
\par       s.tiu@digipen.edu
\date      8th November, 2024
\brief     This file contains the implementation of the imgui game
           window, which integrates ImGui functionality for rendering
           in game elements.

           This file focuses on rendering the game scene preview
           within a dedicated ImGui window ("Game Window") while
           maintaining the correct aspect ratio for the render.
           The method m_DrawGameSceneWindow():
           - Initializes and updates the GraphicsPipe for rendering.
           - Computes the appropriate aspect ratio for the rendered
             scene to fit within the ImGui window.
           - Centers the rendered scene within the window.
           - Adjusts camera matrices to support the editor camera view.

           This file enables seamless integration of in-game scenes
           within an editor window, making it possible for users to
           preview gameplay directly from the editor.

           Key features:
           - Dynamic aspect ratio handling for scene previews.
           - Integration with GraphicsPipe and EditorCamera for
             scene rendering.
           - Use of ImGui's AddImage for displaying textures.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Editor.h"
#include "ECS/ECS.h"
#include "Application/Application.h"
#include "Editor/EditorCamera.h"
#include "Graphics/GraphicsManager.h"

#include <imgui_internal.h>

namespace gui
{
	void ImGuiHandler::DrawGameSceneWindow()
	{
        ImGui::Begin("Game Preview");


        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 renderWindowSize = ImGui::GetContentRegionAvail();

        ImVec2 testSize(960, 540);
        //ImGui::GetWindowDrawList()->AddImage(
                //    (void*)(GLuint)GraphicsManager::GetInstance()->gm_GetGameBuffer().texID, pos,
                //    ImVec2(pos.x + testSize.x, pos.y + testSize.y),
                //    ImVec2(0, 1), ImVec2(1, 0));
        ImGui::GetWindowDrawList()->AddImage(
            reinterpret_cast<void*>(static_cast<uintptr_t>(GraphicsManager::GetInstance()->gm_GetGameBuffer().texID)),
            pos, ImVec2(pos.x + testSize.x, pos.y + testSize.y),
            ImVec2(0, 1), ImVec2(1, 0));

        gameWindowPos = ImGui::GetCursorScreenPos();
        gameWindowSize = ImGui::GetContentRegionAvail();

        ImGui::End();
	}
}