/******************************************************************/
/*!
\file      imgui_play_pauce.cpp
\author    Jaz Winn Ng
\par       jazwinn.ng@digipen.edu
\date      Sept 28, 2025
\brief     Displays the play, pause, and stop buttons in the editor using ImGui.
			- DrawPlayPauseBar: Renders the play, pause, and stop buttons, managing game state transitions and audio control.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Editor.h"
#include "Resources/ResourceManager.h"
#include "Scene/SceneManager.h"

#include "Windows.h"
#include <thread>
#include "Scripting/ScriptManager.h"
#include "Audio/AudioManager.h"

namespace gui {

    void DrawLoading() {

    }

    void CloseLoadingWindow(HWND g_hwnd) {
        if (g_hwnd) {
            PostMessage(g_hwnd, WM_CLOSE, 0, 0);
        }
    }
	
    void ImGuiHandler::DrawPlayPauseBar() {

        if (ImGui::BeginMenuBar()) {

            if (m_ecs.GetState() == ecs::STOP || m_ecs.GetState() == ecs::WAIT) {

                if(ImGui::Button("Play")) {
                    if (m_ecs.GetState() == ecs::STOP) {
                        m_sceneManager.CacheCurrentScene();
                    }
                    m_ecs.SetState(ecs::START);
                    m_audioManager.SetPaused(false);
				}
            }

            if (m_ecs.GetState() == ecs::RUNNING){
                if (ImGui::Button("Pause")) {
                    m_ecs.SetState(ecs::WAIT);
                    m_audioManager.SetPaused(true);
                }
            }

            if (m_ecs.GetState() == ecs::WAIT || m_ecs.GetState() == ecs::RUNNING) {
                if (ImGui::Button("Stop")) {
                    m_ecs.SetState(ecs::STOP);

                    m_audioManager.StopAll();
                    m_audioManager.SetPaused(false);

                    //stop all scene states
                    std::unordered_map<std::string, bool> saveStateScene;
                    for (const auto& scene : m_ecs.sceneMap) {
                        saveStateScene[scene.first] = scene.second.isActive;
                    }
                    //TODO load back scene

                    m_sceneManager.ReloadScene();
                    m_clickedEntityId = -1;
                    //set back all scene to its active/inactive state
                    for (auto& scene : m_ecs.sceneMap) {
                        const auto& saveState = saveStateScene.find(scene.first);
                        if (saveState == saveStateScene.end())continue;
                        scene.second.isActive = saveState->second;
                    }
                }
            }
        }
        ImGui::EndMenuBar();
    }
}