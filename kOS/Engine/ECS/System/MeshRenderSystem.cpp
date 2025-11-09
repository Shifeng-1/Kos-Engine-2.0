/********************************************************************/
/*!
\file      MeshRenderSystem.cpp
\author    Sean Tiu (2303398)
\par       Email: s.tiu@digipen.edu
\date      Oct 01, 2025
\brief     Defines the MeshRenderSystem class, responsible for
           rendering static meshes within the ECS system.

           This system:
           - Handles entities with MeshFilterComponent and
             MeshRendererComponent.
           - Submits mesh draw calls to the rendering pipeline.
           - Supports material and texture binding for PBR rendering.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "ECS/ECS.h"

#include "MeshRenderSystem.h"
#include "ECS/Component/MeshRendererComponent.h"
#include "ECS/Component/MeshFilterComponent.h"
#include "ECS/Component/TransformComponent.h"
#include "ECS/Component/NameComponent.h"
#include "Resources/ResourceManager.h"

#include "Graphics/GraphicsManager.h"

namespace ecs {

    void MeshRenderSystem::Init()
    {
        // Initialize static mesh rendering resources if needed
    }

    void MeshRenderSystem::Update(){

        const auto& entities = m_entities.Data();

        for (const EntityID id : entities) {
            NameComponent* nameComp = m_ecs.GetComponent<NameComponent>(id);
            if (nameComp->hide)continue;;
            TransformComponent* transform = m_ecs.GetComponent<TransformComponent>(id);
            MeshFilterComponent* meshFilter = m_ecs.GetComponent<MeshFilterComponent>(id);

            if (m_ecs.HasComponent<MaterialComponent>(id)) {
                MaterialComponent* matRenderer = m_ecs.GetComponent<MaterialComponent>(id);
                //Initialize each material
                std::shared_ptr<R_Model> mesh = m_resourceManager.GetResource<R_Model>(meshFilter->meshGUID);
                std::vector<PBRMaterial>pbrTmpList;
                if (!matRenderer->materialGUID.size())continue;;
                for (utility::GUID guid : matRenderer->materialGUID) {
                    std::shared_ptr<R_Material> mat = m_resourceManager.GetResource<R_Material>(guid);
                    if (!mat)return;;
                    //Initialize all materials in the list
                    std::shared_ptr<R_Texture> diff = m_resourceManager.GetResource<R_Texture>(mat->md.diffuseMaterialGUID);
                    std::shared_ptr<R_Texture> spec = m_resourceManager.GetResource<R_Texture>(mat->md.specularMaterialGUID);
                    std::shared_ptr<R_Texture> norm = m_resourceManager.GetResource<R_Texture>(mat->md.normalMaterialGUID);
                    std::shared_ptr<R_Texture> ao = m_resourceManager.GetResource<R_Texture>(mat->md.ambientOcclusionMaterialGUID);
                    std::shared_ptr<R_Texture> rough = m_resourceManager.GetResource<R_Texture>(mat->md.roughnessMaterialGUID);
                    pbrTmpList.push_back(PBRMaterial{ diff,spec,rough,ao,norm });
                }
                if (mesh)
                    m_graphicsManager.gm_PushMeshData(MeshData{ mesh,std::make_shared<PBRMaterialList>(pbrTmpList,true), transform->transformation,id }, nameComp->Layer);

            }
            // Skip entities not in this scene or hidden
        }
    }

}