/********************************************************************/
/*!
\file      Material.h
\author    Gabe Ng 2301290
\par       gabe.ng@digipen.edu
\date      Oct 03 2025
\brief     Contains the material type to be used for PBR

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#pragma once
#include "Shader.h"
#include "CubeMap.h"
#include "GraphicsReferences.h"
#include "Resources/R_Texture.h"

struct Material {
	glm::vec3 ambience{1.f,1.f,1.f};
	glm::vec3 diffuse{ 1.f,1.f,1.f };
	glm::vec3 specular{ 1.f,1.f,1.f };
	float shininess{100.f};

	float reflectivity;
	float refractivity;

	CubeMap* envMap{nullptr};
	void SetUniform(Shader*);
};

struct PBRMaterial {
	PBRMaterial(std::shared_ptr<R_Texture> albedoTex=nullptr,
				std::shared_ptr<R_Texture> specularTex = nullptr,
				std::shared_ptr<R_Texture> roughnessTex = nullptr,
				std::shared_ptr<R_Texture> aoTex = nullptr,
				std::shared_ptr<R_Texture> normalTex = nullptr);
	std::shared_ptr<R_Texture> albedo;
	std::shared_ptr<R_Texture> specular;
	std::shared_ptr<R_Texture> roughness;
	std::shared_ptr<R_Texture> ao;
	std::shared_ptr<R_Texture> normal;
	bool listCon=false;
};
struct PBRMaterialList :public PBRMaterial {
	PBRMaterialList(std::vector<PBRMaterial> const& pbrList, bool lc) {
		pbrMatList = pbrList;
		listCon = lc;
	}
	std::vector<PBRMaterial>pbrMatList;
};