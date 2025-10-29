#include "Editor.h"
#include "config/pch.h"
#include "AssetManager/AssetManager.h"
#include "Resources/ResourceManager.h"
#include "Graphics/GraphicsManager.h"

struct DisplayMaterial{
    std::string materialName;
    MaterialData data;

    REFLECTABLE(DisplayMaterial, materialName, data);
};
struct MaterialFB {
    FrameBuffer fb;
    bool startCon = false;
};
DisplayMaterial materialData;
MaterialFB mfb;
//Camera data

void gui::ImGuiHandler::DrawMaterialWindow() {

    if (!mfb.startCon) {
        mfb.fb.InitializeFBO(350, 350);
        mfb.startCon = true;
    }
    ImGui::Begin("Material");
    float windowWidth = ImGui::GetContentRegionAvail().x; // Available width inside window

    std::shared_ptr<ResourceManager> rm = ResourceManager::GetInstance();
    std::shared_ptr<R_Texture> diff = rm->GetResource<R_Texture>(materialData.data.diffuseMaterialGUID);
    std::shared_ptr<R_Texture> spec = rm->GetResource<R_Texture>(materialData.data.specularMaterialGUID);
    std::shared_ptr<R_Texture> norm = rm->GetResource<R_Texture>(materialData.data.normalMaterialGUID);
    std::shared_ptr<R_Texture> ao = rm->GetResource<R_Texture>(materialData.data.ambientOcclusionMaterialGUID);
    std::shared_ptr<R_Texture> rough = rm->GetResource<R_Texture>(materialData.data.roughnessMaterialGUID);
    PBRMaterial pbrMat{ diff,spec,rough,ao,norm };
    Camera cam;
    GraphicsManager::GetInstance()->gm_DrawMaterial(pbrMat, mfb.fb);

    ImGui::Image(
        reinterpret_cast<void*>(static_cast<uintptr_t>(mfb.fb.texID)),
        ImVec2(350, 350),        // Size
        ImVec2(0, 0),            // UV top-left
        ImVec2(1, 1));           // UV bottom-right
    

 
    materialData.ApplyFunction(DrawComponents{ materialData.Names() });



    float buttonWidth = 100.0f; // Width of your button

    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f); // Center horizontally
    if(ImGui::Button("Create", ImVec2(buttonWidth, 0))){
        if (!materialData.materialName.empty()) {
            std::string fileName = materialData.materialName + ".mat";
            std::string filepath = AssetManager::AssetManager::GetInstance()->GetAssetManagerDirectory() + "/Material/" + fileName;

            Serialization::WriteJsonFile(filepath, &materialData.data);
           // std::cout << "TEST CREATION\n";
            LOGGING_POPUP("Material Successfully Added");
        }
        else {
			LOGGING_WARN("Material Name cannot be empty");
        }


    }


    ImGui::End();

}