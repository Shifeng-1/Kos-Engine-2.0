/******************************************************************/
/*!
\file      imgui_contentBrowser.cpp
\author    Ng Jaz Winn, jazwinn.ng, 2301502
\par       jazwinn.ng@digipen.edu
\date      Sep 30, 2025
\brief     

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/


#include "Editor.h"

#include <filesystem>
#include <string>

#include <imgui_internal.h>

#include "AssetManager/AssetManager.h"
#include "Scene/SceneManager.h"
#include "Application/ApplicationData.h"
#include "Inputs/Input.h"
#include "Configs/ConfigPath.h"
#include "AssetManager/Prefab.h"

namespace gui {

	static const char* fileIcon = "img_folderIcon.png";
	static std::string searchString;
	static float padding = 20.f;
	static float thumbnail = 100.f;

	void MoveFolder(const std::filesystem::path& newDirectory) {
		if (ImGui::BeginDragDropTarget())
		{
			//assetmanager::AssetManager* assetmanager = assetmanager::AssetManager::m_funcGetInstance();
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file"))
			{

				IM_ASSERT(payload->DataSize == sizeof(std::filesystem::path));
				std::filesystem::path* filename = static_cast<std::filesystem::path*>(payload->Data);


				if (newDirectory == *filename) return;
				std::filesystem::path destinationFile = newDirectory / filename->filename();

				std::filesystem::rename(*filename, destinationFile);

			}
			ImGui::EndDragDropTarget();
		}
	}

	void textorimage(std::string directoryString, std::string fileName) {
		//assetmanager::AssetManager* assetmanager = assetmanager::AssetManager::m_funcGetInstance();
		//if (assetmanager->m_imageManager.m_imageMap.find(fileName) != assetmanager->m_imageManager.m_imageMap.end()) {
		//	float imageRatio = static_cast<float>(assetmanager->m_imageManager.m_imageMap.find(fileName)->second.m_height) / static_cast<float>(assetmanager->m_imageManager.m_imageMap.find(fileName)->second.m_width);
		//	if (imageRatio > 1)
		//	{
		//		ImGui::ImageButton(directoryString.c_str(), (ImTextureID)(uintptr_t)assetmanager->m_imageManager.m_imageMap.find(fileName)->second.textureID, { thumbnail / imageRatio ,thumbnail }, { 0 ,1 }, { 1 ,0 }, { 0,0,0,0 });
		//	}
		//	else
		//	{
		//		ImGui::ImageButton(directoryString.c_str(), (ImTextureID)(uintptr_t)assetmanager->m_imageManager.m_imageMap.find(fileName)->second.textureID, { thumbnail ,thumbnail * imageRatio }, { 0 ,1 }, { 1 ,0 }, { 0,0,0,0 });
		//	}

		//}
		//else {
		//	ImGui::Button(directoryString.c_str(), { thumbnail ,thumbnail });
		//}

		ImGui::Button(directoryString.c_str(), { thumbnail ,thumbnail });

	};

	void ImGuiHandler::DrawContentBrowser() {

		AssetManager* assetmanager = AssetManager::GetInstance();
		scenes::SceneManager* scenemanager = scenes::SceneManager::m_GetInstance();
		ecs::ECS* ecs = ecs::ECS::GetInstance();

		static std::filesystem::path assetDirectory = assetmanager->GetAssetManagerDirectory(); // TO change
		static std::filesystem::path currentDirectory = assetDirectory;

		if (ImGui::Begin("Content Browser")) {
			if (ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.2f, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_Border)) {
				static bool isSelected{ false };
				for (auto& directoryPath : std::filesystem::directory_iterator(assetDirectory)) {
					std::string directoryString = directoryPath.path().filename().string();
					isSelected = ImGui::Selectable(directoryString.c_str());
					MoveFolder(assetDirectory / directoryPath.path().filename());

					if (std::filesystem::is_directory(directoryPath) && isSelected) {
						currentDirectory = assetDirectory / directoryPath.path().filename();
						//reset search string
						searchString.clear();
					}
				}
				ImGui::EndChild();
			}

			ImGui::SameLine();

			if (ImGui::BeginChild("ChildLa", ImVec2(0, ImGui::GetContentRegionAvail().y), 0, ImGuiWindowFlags_MenuBar)) {
				if (ImGui::BeginPopupContextWindow()) {
					if (ImGui::MenuItem("Add Folder")) {
						std::string path = "/New Folder";

						int count{ 1 };
						while (std::filesystem::exists(currentDirectory.string() + path)) {
							path = "/New Folder_" + std::to_string(count++);
						}

						if (std::filesystem::create_directories(currentDirectory.string() + path)) {

							LOGGING_INFO("Directory created successfully!");
						}
						else {
							LOGGING_ERROR("Directory already exists or failed to create!");
						}

					}
					if (ImGui::MenuItem("Reload Browser")) {
						//TODO: reload browser
					}
					ImGui::EndPopup();
				}

				//search bar
				if (m_prefabSceneMode)searchString.clear();

				//menu bar for search
				if (ImGui::BeginMenuBar()) {
					ImGui::Text("Search:");
					ImGui::SameLine(); // Keep the next widget on the same line
					ImGui::SetNextItemWidth(300);
					if (ImGui::InputText("##4312Search", &searchString)) {}
					ImGui::EndMenuBar(); // End menu bar
				}

				//back button		
				if (currentDirectory != assetDirectory) {
					bool open = ImGui::Button("Back");
					MoveFolder(currentDirectory.parent_path());
					if (open) {
						currentDirectory = currentDirectory.parent_path();
						searchString.clear();
					}

				}
				else {
					ImGui::NewLine();
				}

				float cellsize = padding + thumbnail;
				float panelwidth = ImGui::GetContentRegionAvail().x;
				int columns = (int)(panelwidth / cellsize);
				if (columns <= 0) {
					columns = 1;
				}
				ImGui::Columns(columns, 0, false);

				// Render Icons (Folder or Files)
				for (auto& directoryPath : std::filesystem::directory_iterator(currentDirectory)) {
					std::string directoryString = directoryPath.path().filename().string();

					//skip if file is not same as search and skip .meta files
					if (!searchString.empty() && !containsSubstring(directoryString, searchString) ||
						directoryPath.path().filename().extension().string() == ".meta") {
						continue;
					}

					if (directoryPath.is_directory()) {
						// if a folder
						textorimage(directoryString, fileIcon);
						MoveFolder(currentDirectory / directoryPath.path().filename());


						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
							currentDirectory /= directoryPath.path().filename();
							//reset search
							searchString.clear();
						}
					}
					else {
						//case for prefabs and scene
						if (directoryPath.path().filename().extension().string() == ".prefab") {
							std::string prefab = "";
							textorimage(std::string(directoryPath.path().filename().extension().string() + "##" + directoryPath.path().filename().string()).c_str(), prefab);

							if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
								//skip if active scene is filename
								if (m_activeScene == directoryPath.path().filename())continue;

								const auto& prefabscene = ecs->sceneMap.find(directoryPath.path().filename().string());
								if (prefabscene == ecs->sceneMap.end()) {
									LOGGING_ERROR("Prefab not loaded");
									continue;
								}

								//skip if prefab mode alraedy true
								if (!m_prefabSceneMode) {
									m_savedSceneState.clear();
									for (auto& scene : ecs->sceneMap) {
										if (scene.second.isPrefab == false) {
											//save all scenes active state
											m_savedSceneState[scene.first] = scene.second.isActive;
										}
									}

								}

								// clear save scene state

								// unload all regular scenes
								for (auto& scene : ecs->sceneMap) {
									scene.second.isActive = false;
								}

								// set prefab to active
								prefabscene->second.isActive = true;

								// Duplicate Entity and add it into original scene. Will be removed when m_prefabSceneMode is set back to false. 
								// (Duped Entity used to check if any edits has been made to prefab)
								duppedID = ecs::ECS::GetInstance()->DuplicateEntity(prefabscene->second.prefabID, m_activeScene);
								ecs::ECS::GetInstance()->GetComponent<ecs::NameComponent>(duppedID)->prefabName = prefabscene->first;

								//set prefab as active scene
								m_activeScene = directoryPath.path().filename().string();

								m_prefabSceneMode = true;

								m_clickedEntityId = -1;
							}
						}
						else if (directoryPath.path().filename().extension().string() == ".json") {
							std::string script;
							textorimage(directoryString, script);

							if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {

								scenemanager->ClearAllScene();
								scenemanager->LoadScene(directoryPath.path());
								if (!m_prefabSceneMode) {
									m_activeScene = directoryPath.path().filename().string();
								}
								else {
									ecs::ECS::GetInstance()->sceneMap.find(directoryPath.path().filename().string())->second.isActive = false;
									m_savedSceneState[directoryPath.path().filename().string()] = true;
								}

								m_clickedEntityId = -1;

							}
						}
						else {

							textorimage(directoryString, std::string());
							if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
								std::filesystem::path metaPath = directoryPath.path().string() + ".meta";
								if (std::filesystem::exists(metaPath)) {
									selectedAsset = Serialization::ReadJsonFile<AssetData>(metaPath.string());
									AssetPath = metaPath;
								}
							}
						}
					}

					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
						std::string GUID = assetmanager->GetGUIDfromFilePath(directoryPath.path());

						if (!GUID.empty()) {
							ImGui::SetDragDropPayload("file", GUID.c_str(), GUID.size() + 1);
							ImGui::Text(GUID.c_str());
						}	
						else {
							std::string filepath = directoryPath.path().string();
							ImGui::SetDragDropPayload("file", filepath.c_str(), filepath.size() + 1);
							ImGui::Text(filepath.c_str());
						}
						ImGui::EndDragDropSource();
					}

					//create context window
					static bool rename = false;
					static std::filesystem::path selectedfile{};
					if (ImGui::BeginPopupContextItem()) {
						if (ImGui::MenuItem("Rename")) {
							rename = true;
							selectedfile = directoryString;
						}
						if (ImGui::MenuItem("Delete")) {
							std::filesystem::remove_all(directoryPath.path());
						}
						if (!directoryPath.is_directory()) {

							//if (ImGui::BeginMenu("Compile"))
							//{
							//	if (ImGui::MenuItem("Compile"))
							//	{
							//		assetmanager->Compilefile(directoryPath);
							//	}
							//	ImGui::EndMenu();
							//}
							const auto& compilerMap = assetmanager->GetCompilerMap();

						if(compilerMap.find(directoryPath.path().filename().extension().string()) != compilerMap.end())
						{
							if (ImGui::BeginMenu("Reload As"))
							{
								for(const auto& comp : compilerMap.at(directoryPath.path().filename().extension().string()))
								{
									if (ImGui::MenuItem(comp.type.c_str()))
									{
										assetmanager->Compilefile(directoryPath.path(), comp.outputExtension);
									}
								}
								ImGui::EndMenu();
							}
						}


					}
					if (directoryPath.path().filename().extension().string() == ".json" && ImGui::MenuItem("Set as Startup Scene")) {


							std::filesystem::path metaPath = directoryPath.path().string() + ".meta";

							if (!std::filesystem::exists(metaPath)) {
								LOGGING_POPUP("Meta file does not exist, Compile First");
							}

							WindowSettings data = Serialization::ReadJsonFile<WindowSettings>(configpath::configFilePath);
							AssetData assetData = Serialization::ReadJsonFile<AssetData>(metaPath.string());

							data.startScene = assetData.GUID;

							Serialization::WriteJsonFile<WindowSettings>(configpath::configFilePath, &data, true);
						}


						ImGui::EndPopup();
					}


					if (rename && (selectedfile.string() == directoryString)) {
						if (ImGui::InputText("##rename", m_charBuffer, IM_ARRAYSIZE(m_charBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
							//TODO check if file has extension, keep the extension
							std::string extension{};
							if (!directoryPath.is_directory()) {
								extension = directoryPath.path().filename().extension().string();
								ImGui::SameLine();
								ImGui::Text(extension.c_str());
							}

							std::filesystem::path path = std::filesystem::current_path();
							std::string newpath = path.string() + "\\" + currentDirectory.string() + "\\" + m_charBuffer + extension;
							std::string oldpath = path.string() + "\\" + currentDirectory.string() + "\\" + directoryString;


							LOGGING_INFO("RENAME WIP");
							//assetmanager->m_RenameAsset(oldpath, newpath);

							rename = false;
							selectedfile.clear();

							//TODO edge cases,
							//Update assets if any of them are renamed
						}
					}
					else {
						ImGui::SetWindowFontScale(0.8f);
						ImGui::Text(directoryPath.path().filename().stem().string().c_str());
						ImGui::SetWindowFontScale(1.f);
					}
					ImGui::NextColumn();
				}

				//allow drag and drop on child directory
				if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
					if (!Input::InputSystem::GetInstance()->droppedFiles.empty()) {
						for (const auto& file : Input::InputSystem::GetInstance()->droppedFiles) {
							std::filesystem::path source = file;
							std::filesystem::path destination = currentDirectory;

							if (!std::filesystem::exists(source)) {
								LOGGING_WARN("Source directory does not exist ");
							}
							else {
								if (std::filesystem::is_directory(source)) {
									destination /= source.filename();
									if (std::filesystem::exists(destination)) {
										LOGGING_WARN("Directory already exists");
										continue;
									}
								}

								if (std::filesystem::exists(destination / source.filename())) {
									LOGGING_WARN("Directory already in folder");
									continue;
								}

								// Copy directory and all contents recursively
								std::filesystem::copy(source, destination, std::filesystem::copy_options::recursive);

								//load new asset
								assetmanager->RegisterAsset(destination / source.filename());
							}

						}
						// Clear the vector if you want to reset after displaying
						Input::InputSystem::GetInstance()->droppedFiles.clear();
					}
				}
				ImGui::EndChild();
			}

			// Drag and Drop Interaction for Creating New Prefabs
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
				{
					IM_ASSERT(payload->DataSize == sizeof(ecs::EntityID));
					LOGGING_DEBUG("Dropping To Save Prefab");
					ecs::EntityID id = *static_cast<ecs::EntityID*>(payload->Data);
					prefab::Prefab::m_SaveEntitytoPrefab(id);
				}
				ImGui::EndDragDropTarget();
			}
		}
		ImGui::End();
	}
}