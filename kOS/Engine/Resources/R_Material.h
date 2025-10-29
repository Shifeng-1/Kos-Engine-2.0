#pragma once
#include "Resource.h"
#include "Config/pch.h"

struct MaterialData
{

    std::string diffuseMaterialGUID{};   // Path or ID for material asset
    std::string specularMaterialGUID{};
    std::string normalMaterialGUID{};
    std::string ambientOcclusionMaterialGUID{};
    std::string roughnessMaterialGUID{};

    REFLECTABLE(MaterialData,
        diffuseMaterialGUID,
        specularMaterialGUID,
        normalMaterialGUID,
        ambientOcclusionMaterialGUID,
        roughnessMaterialGUID
	);
};

class R_Material : public Resource {
public:
    using Resource::Resource;

    void Load() override;
    void Unload() override;


    MaterialData md;
   
    REFLECTABLE(R_Material);

private:
};