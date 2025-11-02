/******************************************************************/
/*!
\file      Editor.h
\author    Jaz Winn Ng
\par       jazwinn.ng@digipen.edu
\date      Sept 28, 2025
\brief     Editor class that manages ImGui integration and rendering for the Kos Editor.
            - Initialize: Sets up ImGui context, configures backends, and applies styles.
            - NewFrame: Prepares a new ImGui frame for rendering.
            - Update: Manages the main rendering loop, including various editor windows.
            - Render: Draws all ImGui elements to the screen.
			- Shutdown: Cleans up ImGui resources and contexts.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/
#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "ImGuizmo.h"
#include "ECS/ECS.h"
#include "Editor/EditorReflection.h"
#include "Application/Window.h"
#include "AssetManager/AssetDatabase.h"
#include "Config/ComponentRegistry.h"
#include "Editor/Payload.h"


namespace gui {
    

    class ImGuiHandler
    {
    public:
        /******************************************************************/
        /*!
        \fn      gui::ImGuiHandler::ImGuiHandler()
        \brief   Default constructor for the ImGuiHandler class.
        \details Sets up initial values for ImGui operations and prepares the context for UI rendering.
        */
        /******************************************************************/
        ImGuiHandler(Application::AppWindow& window);

        /******************************************************************/
        /*!
        \fn      gui::ImGuiHandler::~ImGuiHandler()
        \brief   Destructor for the ImGuiHandler class.
        \details Cleans up and releases any resources used by ImGui, ensuring a proper shutdown.
        */
        /******************************************************************/
        ~ImGuiHandler();


        /******************************************************************/
        /*!
            \fn        gui::ImGuiHandler::m_Initialize(GLFWwindow* window, const char* glsl_version)
            \brief     Initializes ImGui with GLFW and OpenGL settings.
            \param[in] window       The window handle used by GLFW.
            \param[in] glsl_version The OpenGL GLSL version string.
            \details   Sets up the ImGui context, configures platform and renderer bindings,
                       enables various ImGui features (such as keyboard, gamepad support, and
                       multi-viewport windows), and applies the dark style theme.
        */
        /******************************************************************/
        void Initialize(GLFWwindow* window, const char* glsl_version, const std::string& editorTagsFile, const std::string& imguiINI);

        /******************************************************************/
        /*!
            \fn        gui::ImGuiHandler::m_NewFrame()
            \brief     Prepares and starts a new ImGui frame.
            \details   This function is responsible for starting a new frame in ImGui by
                        initializing both OpenGL and GLFW backends and preparing for ImGui rendering.
        */
        /******************************************************************/
        void NewFrame();

        /******************************************************************/
        /*!
            \fn        gui::ImGuiHandler::m_Render()
            \brief     Renders ImGui elements and windows.
            \details   This function manages the main rendering loop for ImGui, handling the
                        menu bar, performance window, hierarchy, component, and log windows.
                        It also manages ImGui's viewport and platform-specific rendering.
        */
        /******************************************************************/
        void Update();

        /******************************************************************/
        /*!
        \fn      void m_InputUpdate(GLFWwindow* window)
        \brief   Updates input states based on the current GLFW window context.
        \param   window - Pointer to the GLFW window used to poll input events.
        */
        /******************************************************************/
        void InputUpdate();

        /******************************************************************/
        /*!
        \fn      void gui::ImGuiHandler::m_render()
        \brief   Renders the ImGui UI elements.
        \details Draws all registered ImGui widgets and panels to the screen, handling the rendering logic.
        */
        /******************************************************************/
        void Render();


        /******************************************************************/
        /*!
            \fn        gui::ImGuiHandler::m_Shutdown()
            \brief     Cleans up and shuts down ImGui.
            \details   This function handles shutting down ImGui by cleaning up the OpenGL
                       and GLFW resources, destroying the ImGui and ImPlot contexts.
        */
        /******************************************************************/
        void Shutdown();

    private:

        template <typename T>
        void RegisterComponent();
        void DrawMainMenuBar();
        void DrawPerformanceWindow();
        void DrawComponentWindow();
        void DrawInputWindow();
        unsigned int DrawHierachyWindow();
        bool DrawEntityNode(ecs::EntityID entity);
        void DrawLogsWindow();
        void DrawPlayPauseBar();
        void DrawRenderScreenWindow(unsigned int windowWidth, unsigned int windowHeight);
        void DrawGizmo(float renderPosX, float renderPosY, float renderWidth, float renderHeight);
        void DrawLayerWindow();
        void DrawSceneSettingsWindow();
        void DrawGameSceneWindow();
        void DrawContentBrowser();
        void DrawAssetInspector();
		void DrawMaterialWindow();
        void DrawBakedWindow();

        void m_UpdateOnPrefabMode();
        void openAndLoadSceneDialog();

        void DeserializeProfile();
        void DrawPreferencesWindow();

        void DrawAnimationWindow();
        void DrawAudioMixerWindow();

        /***********PreferenceTab*************/
        bool openPreferencesTab = false;
        /******************************/

        /***********Assets*************/
        std::filesystem::path AssetPath{};
        AssetData selectedAsset{};
        /******************************/

        std::unordered_map<std::string, std::shared_ptr<IEditorActionInvoker>> componentDrawers;

        int m_clickedEntityId{ -1 };
        bool m_isUi{ false };

        std::string m_activeScene{};

        /***********PREFAB*************/
        bool m_prefabSceneMode{ false };
        //ecs::EntityID duppedID = -1;    //Used to check if prefab has been changed.
        bool m_collisionSetterMode{ false };
        std::unordered_map<std::string, bool> m_savedSceneState;
        /******************************/

        bool m_objectNameBox = false;
        char m_charBuffer[50] = "";  // Char Buffer

        std::vector<std::string>m_tags;

        ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        
        /***********Window Names*************/
        ImVec2 gameWindowPos;
        ImVec2 gameWindowSize;
        /************************************/

        /***********Delegates****************/
        Delegate<const std::string&> onSaveAll;
        void RegisterCallBack();
        /************************************/


        std::string m_imgui_layout;

        inline void SaveLayout() {

            if (!ImGui::GetCurrentContext()) {
                std::cout << "Warning: No ImGui context found. Skipping LoadLayout()." << std::endl;
                return;
            }
            std::ifstream file(m_imgui_layout);
            if (!file.good()) {
                std::cout << "Layout file not found: " << m_imgui_layout << std::endl;
                return;
            }

            ImGui::SaveIniSettingsToDisk(m_imgui_layout.c_str());
        }

        inline void LoadLayout()
        {
            if (!ImGui::GetCurrentContext()) {
                std::cout << "Warning: No ImGui context found. Skipping LoadLayout()." << std::endl;
                return;
            }
            std::ifstream file(m_imgui_layout);
            if (!file.good()) {
                std::cout << "Layout file not found: " << m_imgui_layout << std::endl;
                return;
            }

            ImGui::LoadIniSettingsFromDisk(m_imgui_layout.c_str());
        }

        

        private:

			ecs::ECS* m_ecs;
            Application::AppWindow& m_window;
        
    };


    class EditorComponentTypeRegistry {
    private:
        static std::unordered_map<std::string, std::function<std::shared_ptr<IEditorActionInvoker>()>> s_drawerFactories;

    public:
        template <typename T>
        static void RegisterComponentType() {
            s_drawerFactories[T::classname()] = []() {
                return std::make_shared<EditorActionInvoker<T>>();
                };
        }

        static void CreateAllDrawers(std::unordered_map<std::string, std::shared_ptr<IEditorActionInvoker>>& drawers) {
            for (const auto& [className, factory] : s_drawerFactories) {
                drawers[className] = factory();
            }
        }
    };




    bool containsSubstring(const std::string& x, const std::string& y);


    template<typename T>
    inline void ImGuiHandler::RegisterComponent()
    {
        EditorComponentTypeRegistry::RegisterComponentType<T>();
    }
}