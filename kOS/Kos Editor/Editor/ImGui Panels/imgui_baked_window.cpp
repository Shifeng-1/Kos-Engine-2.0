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
	std::shared_ptr<GraphicsManager> gm = GraphicsManager::GetInstance();
	if (ImGui::Button("Bake Lights"))
	{
		ecs::ECS* ecs = ecs::ECS::GetInstance();

		LOGGING_INFO("It's a piece of cake to bake a pretty cake");
		//Just bake first light first
		PointLightData* pld;
		int i = 0;
		for (auto& lcComp: ecs->GetComponentsEnties("LightComponent"))
		{
			pld = &gm->lightRenderer.pointLightsToDraw[i];
			//Add shadow setting later as well
			if (!pld->bakedCon)continue;;
			std::cout << lcComp << "<- LIGHT ENTITY\n";

			//EVENTUALLY MAKE IT DO ITS OWN DEPTH BUFFER CREATION
			//Get DCM, make a faux depth map renderer
			//Get all objects
			gm->gm_FillDepthCube(CameraData{}, i);
			
			//Asset creation
			//Generate GUID
			// Attach name
			//KEEP THIS
			std::string filepath = AssetManager::AssetManager::GetInstance()->GetAssetManagerDirectory() + "/DepthMap/" + std::to_string(lcComp) + ".dcm";
			gm->lightRenderer.dcm[i].SaveDepthCubeMap(filepath);

			
			//Add and load asset and assign it to light component
			//Need to change entity itself
			ecs->GetComponent<ecs::LightComponent>(lcComp)->depthMapGUID=AssetManager::AssetManager::GetInstance()->RegisterAsset(filepath);
			AssetManager::AssetManager::GetInstance()->Compilefile(filepath);
			//Retrieve GUID from file path
			i++;
		}
	}
	if (ImGui::Button("Test Lights")) {
		gm->lightRenderer.dcm[0].LoadDepthCubeMap("D:/CJJJ2/kOS/Kos Editor/Assets/DepthMap/test.dcm");
		gm->lightRenderer.dcm[1].LoadDepthCubeMap("D:/CJJJ2/kOS/Kos Editor/Assets/DepthMap/test.dcm");

	}
	ImGui::End();
}