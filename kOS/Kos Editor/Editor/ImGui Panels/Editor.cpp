/********************************************************************/
/*!
\file      imgui_handler.cpp
\author    Chiu Jun Jie, junjie.c , 2301524
\par       junjie.c@digipen.edu
\date      Oct 02, 2024
\brief     This file defines functions that handle ImGui operations
		   - Initialize: Sets up ImGui with GLFW and OpenGL.
		   - NewFrame: Prepares and starts a new ImGui frame.
		   - Render: Renders ImGui elements and windows.
		   - Shutdown: Cleans up ImGui resources.
		   - DrawMainMenuBar: Renders the main menu bar UI.
		   - DrawPerformanceWindow: Displays FPS and performance metrics.
		   - DrawComponentWindow: Manages the ECS component interface.
		   - DrawLogsWindow: Displays logs.
		   - DrawInputWindow: Handles input settings.
		   - DrawRenderScreenWindow: Sets up the window for render output.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/


#include <RAPIDJSON/document.h>
#include <RAPIDJSON/writer.h>
#include <RAPIDJSON/stringbuffer.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "implot_internal.h"
#include "ImGuiStyle.h"

#include "Editor.h"
#include "Editor/EditorCamera.h"
#include "Inputs/Input.h"
#include "AssetManager/Prefab.h"
#include "AssetManager/AssetManager.h"
#include "Editor/WindowFile.h"
#include "Graphics/GraphicsManager.h"
#include "Scene/SceneManager.h"

namespace gui {

	std::unordered_map<std::string, std::function<std::shared_ptr<IEditorActionInvoker>()>>  EditorComponentTypeRegistry::s_drawerFactories;

	ImGuiHandler::ImGuiHandler(Application::AppWindow& window) :m_window(window) {

		m_ecs = ecs::ECS::GetInstance();




	} //CTORdoing 

	ImGuiHandler::~ImGuiHandler() {} //Destructor

	void ImGuiHandler::Initialize(GLFWwindow* window, const char* glsl_version, const std::string& editorTagsFile, const std::string& imguiINI)
	{
		m_imgui_layout = imguiINI;
		//set up reflection

		//REFLECTION, ADD component

		//initialize component type

		//ComponentTypeRegistry::RegisterComponentType<ecs::NameComponent>(ecs::TYPENAMECOMPONENT);
		RegisterComponent<ecs::TransformComponent>();
		RegisterComponent<ecs::SpriteComponent>();
		RegisterComponent<ecs::CameraComponent>();
		RegisterComponent<ecs::TextComponent>();
		RegisterComponent<ecs::MeshFilterComponent>();
		RegisterComponent<ecs::MeshRendererComponent>();
		RegisterComponent<ecs::SkinnedMeshRendererComponent>();
		RegisterComponent<ecs::CanvasRendererComponent>();
		RegisterComponent<ecs::AnimatorComponent>();
		RegisterComponent<ecs::LightComponent>();
		RegisterComponent<ecs::RigidbodyComponent>();
		RegisterComponent<ecs::BoxColliderComponent>();
		RegisterComponent<ecs::CapsuleColliderComponent>();
		RegisterComponent<ecs::SphereColliderComponent>();
		RegisterComponent<ecs::CharacterControllerComponent>();
		RegisterComponent<ecs::ScriptComponent>();
		RegisterComponent<ecs::AudioComponent>();
		RegisterComponent<ecs::OctreeGeneratorComponent>();
		RegisterComponent<ecs::CubeRendererComponent>();
		RegisterComponent<ecs::MaterialComponent>();
		//Allocate to map
		EditorComponentTypeRegistry::CreateAllDrawers(componentDrawers);


		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext()); // Ensure context is set

		ImGui::GetIO().IniFilename = nullptr;


		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;// Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;// Enable Multi-Viewport / Platform Windows

		SetStyle();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);


		// load current layout
		LoadLayout();

		// Load Prefabs into scenes
		prefab::Prefab::LoadAllPrefabs();

		//load scene
		openAndLoadSceneDialog();

		//set first active scene
		for (auto& scene : m_ecs->sceneMap) {
			if (!scene.second.isPrefab) {
				m_activeScene = scene.first;
				break;
			}
		}

		m_tags = filewindow::readEditorConfig(editorTagsFile);
	}

	void ImGuiHandler::NewFrame()
	{
		// Start a new ImGui frame
		ImGui_ImplOpenGL3_NewFrame(); //needs to be called every frame
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiHandler::Update()
	{



		NewFrame();

		if (Input::InputSystem::GetInstance()->IsKeyTriggered(keys::F11))
		{

		}

		if (EditorCamera::m_editorMode)
		{
			// Render ImGui

			//for gizmo - todo once camera is done
			ImGuizmo::SetOrthographic(false);

			//viewport docking
			ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

			//update while prefabmode is on
			m_UpdateOnPrefabMode();

			ecs::ECS* ecs = ecs::ECS::GetInstance();

			//check if "m_activeScene", if not find first active scene
			const auto& scene = ecs->sceneMap.find(m_activeScene);
			if (scene == ecs->sceneMap.end()) {
				for (auto& _scene : ecs->sceneMap) {
					if (!_scene.second.isPrefab) {
						m_activeScene = _scene.first;
						_scene.second.isActive = true;
					}
				}
			}

			//check if selected entityid is loaded
			if (ecs->GetEntitySignatureData().find(m_clickedEntityId) == ecs->GetEntitySignatureData().end()) {
				m_clickedEntityId = -1;
			}


			ImVec2 windowSize = ImGui::GetIO().DisplaySize;
			int windowWidth, windowHeight;
			glfwGetWindowSize(m_window.window, &windowWidth, &windowHeight);
			m_window.windowWidth = static_cast<float>(windowWidth);
			m_window.windowHeight = static_cast<float>(windowHeight);

			// only render when window is not minimize
			if ((windowSize.x > 0 && windowSize.y > 0) || !EditorCamera::m_editorMode) {
				DrawMainMenuBar();
				DrawHierachyWindow();
				DrawComponentWindow();
				DrawLogsWindow();
				DrawLayerWindow();
				DrawSceneSettingsWindow();
				DrawInputWindow();
				DrawContentBrowser();
				DrawRenderScreenWindow(static_cast<unsigned int>(m_window.windowWidth), static_cast<unsigned int>(m_window.windowHeight));
				DrawGameSceneWindow();
				DrawPerformanceWindow();
				DrawPreferencesWindow();
				DrawAnimationWindow();
				DrawAudioMixerWindow();
				DrawAssetInspector();
				DrawMaterialWindow();
			}

		}
		ImGui::EndFrame();

		EditorCamera::editorCamera.CalculatePerspMtx();
		EditorCamera::editorCamera.CalculateViewMtx();
		EditorCamera::editorCamera.CalculateUIOrthoMtx();
		GraphicsManager::GetInstance()->gm_MoveEditorCameraData(EditorCamera::editorCamera);
	}

	void ImGuiHandler::Render()
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Render();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}



	void ImGuiHandler::Shutdown()
	{
		// Shutdown ImGui
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}


	void ImGuiHandler::m_UpdateOnPrefabMode()
	{
		static bool colorChanged = false;
		static glm::vec3 originalColor = {}; // Store original color

		if (m_prefabSceneMode)
		{
			if (!colorChanged)
			{
				colorChanged = true;
			}
			//std::cout << "Gui Call for Updating Prefabs" << std::endl;
			//prefab::Prefab::m_UpdateAllPrefabEntity(m_activeScene); //?
		}
		else
		{
			if (colorChanged)
			{
				colorChanged = false;
			}
		}


	}

	void ImGuiHandler::InputUpdate()
	{
		if (!EditorCamera::m_editorMode)return;

		scenes::SceneManager* scenemanager = scenes::SceneManager::m_GetInstance();
		ImGuiIO& io = ImGui::GetIO();  // Get input/output data
		//If CTRL + S press, save
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)) && (m_ecs->GetState() != ecs::RUNNING)) {

			scenemanager->SaveAllActiveScenes();

		}

	}

	void ImGuiHandler::openAndLoadSceneDialog()
	{
		scenes::SceneManager* scenemanager = scenes::SceneManager::m_GetInstance();
		auto assetManager = AssetManager::GetInstance();

		assetManager->GetAssetWatcher()->Pause();

		//char filePath[MAX_PATH];
		std::filesystem::path path = filewindow::m_OpenfileDialog(AssetManager::GetInstance()->GetAssetManagerDirectory().c_str());
		if (!path.empty() && (path.filename().extension().string() == ".json")) {
			//clear all other scenes
			scenemanager->ClearAllScene();
			scenemanager->LoadScene(path);

			if (!m_prefabSceneMode) {
				m_activeScene = path.filename().string();
			}
			else {
				ecs::ECS::GetInstance()->sceneMap.find(path.filename().string())->second.isActive = false;
				m_savedSceneState[path.filename().string()] = true;
			}
			m_clickedEntityId = -1;
		}
		else {
			LOGGING_POPUP("No Scene loaded");
		}
		assetManager->GetAssetWatcher()->Resume();
	}

	bool containsSubstring(const std::string& x, const std::string& y) {
		// Convert both strings to lowercase
		std::string lower_x = x;
		std::string lower_y = y;

		std::transform(lower_x.begin(), lower_x.end(), lower_x.begin(), ::tolower);
		std::transform(lower_y.begin(), lower_y.end(), lower_y.begin(), ::tolower);

		// Check if the lowercase version of y is found in x
		return lower_x.find(lower_y) != std::string::npos;
	}
}

