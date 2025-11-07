#include "Editor.h"
#include "config/pch.h"
#include "AssetManager/AssetManager.h"
#include "Resources/ResourceManager.h"
#include "Graphics/GraphicsManager.h"
std::string GenerateRandomGUIDBaked() {
	static std::random_device dev;
	static std::mt19937 rng(dev());

	std::uniform_int_distribution<int> dist(0, 15);

	const char* v = "0123456789abcdef";
	const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

	std::string res;
	for (int i = 0; i < 16; i++) {
		if (dash[i]) res += "-";
		res += v[dist(rng)];
		res += v[dist(rng)];
	}
	return res;
}
void gui::ImGuiHandler::DrawBakedWindow() {

	ImGui::Begin("Baked Lighting");
	if (ImGui::Button("Bake Lights"))
	{

		LOGGING_INFO("It's a piece of cake to bake a pretty cake");
		//Just bake first light first
		PointLightData* pld;
		int i = 0;

		auto sceneData =m_ecs.GetSceneData(m_ecs.GetSceneByEntityID(m_clickedEntityId));
		for (auto& lcComp: sceneData.sceneIDs)
		{
			if (!m_ecs.HasComponent<LightComponent>(lcComp))continue;;
			std::cout << lcComp << "<- LIGHT ENTITY\n";
			std::cout << "INDEX: " << i << '\n';
			//Add shadow setting later as well
			if (!m_ecs.GetComponent<ecs::LightComponent>(lcComp)->bakedLighting) {
				i++;
				continue;;
			}
			std::cout << m_clickedEntityId << "<- SELECTED LIGHT ENTITY\n\n";

			//EVENTUALLY MAKE IT DO ITS OWN DEPTH BUFFER CREATION
			//Get DCM, make a faux depth map renderer
			//Get all objects
			m_graphicsManager.gm_FillDepthCube(CameraData{}, 0, m_ecs.GetComponent<ecs::TransformComponent>(lcComp)->LocalTransformation.position);
			
			//Asset creation
			//Generate GUID
			// Attach name
			//KEEP THIS
			std::string filepath = m_assetManager.GetAssetManagerDirectory() + "/DepthMap/" + std::to_string(lcComp) + ".dcm";
			m_graphicsManager.lightRenderer.dcm[0].SaveDepthCubeMap(filepath);

			
			//Add and load asset and assign it to light component
			//Need to change entity itself
			std::cout << lcComp << '\n';
			m_ecs.GetComponent<ecs::LightComponent>(lcComp)->depthMapGUID.SetFromString( m_assetManager.RegisterAsset(filepath).GetToString());
			std::cout << "Depth map GUID " << m_ecs.GetComponent<ecs::LightComponent>(lcComp)->depthMapGUID.GetToString();
			m_assetManager.Compilefile(filepath);
			i++;

			//Retrieve GUID from file path
		}
	}
	if (ImGui::Button("Test Lights")) {
		m_graphicsManager.lightRenderer.dcm[0].LoadDepthCubeMap("D:/CJJJ2/kOS/Kos Editor/Assets/DepthMap/test.dcm");
		m_graphicsManager.lightRenderer.dcm[1].LoadDepthCubeMap("D:/CJJJ2/kOS/Kos Editor/Assets/DepthMap/test.dcm");

	}
	ImGui::End();
}