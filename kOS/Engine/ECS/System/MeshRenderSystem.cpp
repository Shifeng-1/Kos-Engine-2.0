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

    void MeshRenderSystem::Update(const std::string& scene)
    {
        ECS* ecs = ECS::GetInstance();
        std::shared_ptr<GraphicsManager> gm = GraphicsManager::GetInstance();
        std::shared_ptr<ResourceManager> rm = ResourceManager::GetInstance();
        const auto& entities = m_entities.Data();

        for (const EntityID id : entities) {
            TransformComponent* transform = ecs->GetComponent<TransformComponent>(id);
            NameComponent* nameComp = ecs->GetComponent<NameComponent>(id);
            MaterialComponent* matRenderer = ecs->GetComponent<MaterialComponent>(id);
            MeshFilterComponent* meshFilter = ecs->GetComponent<MeshFilterComponent>(id);

            // Skip entities not in this scene or hidden
            if ((matRenderer->scene != scene) || !ecs->layersStack.m_layerBitSet.test(nameComp->Layer) || nameComp->hide)
                continue;

            // Only send data if there is a mesh to render, this is probably redundant, the ECS already forces it
            if (!ecs->HasComponent<MeshFilterComponent>(id))
                continue;

            std::shared_ptr<R_Material> mat= rm->GetResource<R_Material>(matRenderer->materialGUID);
            std::shared_ptr<R_Model> mesh = rm->GetResource<R_Model>(meshFilter->meshGUID);
            std::shared_ptr<R_Texture> diff = rm->GetResource<R_Texture>(mat->md.diffuseMaterialGUID);
            std::shared_ptr<R_Texture> spec = rm->GetResource<R_Texture>(mat->md.specularMaterialGUID);
            std::shared_ptr<R_Texture> norm = rm->GetResource<R_Texture>(mat->md.normalMaterialGUID);
            std::shared_ptr<R_Texture> ao = rm->GetResource<R_Texture>(mat->md.ambientOcclusionMaterialGUID);
            std::shared_ptr<R_Texture> rough = rm->GetResource<R_Texture>(mat->md.roughnessMaterialGUID);

            if (mesh)
                gm->gm_PushMeshData(MeshData{ mesh,PBRMaterial{diff,spec,rough,ao,norm}, transform->transformation,id});
        }
    }

}