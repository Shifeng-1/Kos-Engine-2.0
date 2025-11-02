/********************************************************************/
/*!
\file      SkinnedMeshRenderSystem.cpp
\author    Sean Tiu (2303398)
\par       Email: s.tiu@digipen.edu
\date      Oct 01, 2025
\brief     Defines the SkinnedMeshRenderSystem class, which handles
           rendering of animated (skinned) meshes in the ECS system.

           This system:
           - Works with SkinnedMeshRendererComponent.
           - Uploads bone transformation matrices to shaders.
           - Ensures smooth skeletal animation during rendering.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/


#include "Config/pch.h"
#include "ECS/ECS.h"

#include "SkinnedMeshRenderSystem.h"
#include "ECS/Component/SkinnedMeshRendererComponent.h"
#include "ECS/Component/TransformComponent.h"
#include "ECS/Component/NameComponent.h"
#include "Resources/ResourceManager.h"
#include "Graphics/GraphicsManager.h"

namespace ecs {

    void SkinnedMeshRenderSystem::Init()
    {
        // Initialize skinned mesh rendering resources if needed
    }

    void SkinnedMeshRenderSystem::Update()
    {
        ECS* ecs = ECS::GetInstance();
        const auto& entities = m_entities.Data();
        std::shared_ptr<GraphicsManager> gm = GraphicsManager::GetInstance();
         ResourceManager* rm = ResourceManager::GetInstance();

        for (const EntityID id : entities) {
            TransformComponent* transform = ecs->GetComponent<TransformComponent>(id);
            NameComponent* nameComp = ecs->GetComponent<NameComponent>(id);
            SkinnedMeshRendererComponent* skinnedMesh = ecs->GetComponent<SkinnedMeshRendererComponent>(id);

            // Skip entities not in this scene or hidden
            if (!ecs->layersStack.m_layerBitSet.test(nameComp->Layer) || nameComp->hide)
                continue;

            R_Model* mesh{};
            R_Animation* skeleton{};
            //if (skinnedMesh->cachedSkinnedMeshGUID != skinnedMesh->skinnedMeshGUID)
            {
                mesh = rm->GetResource<R_Model>(skinnedMesh->skinnedMeshGUID).get();
                skeleton = rm->GetResource<R_Animation>(skinnedMesh->skeletonGUID).get();
                skinnedMesh->cachedSkinnedMeshGUID = skinnedMesh->skinnedMeshGUID;
                skinnedMesh->cachedSkinnedMeshResource = static_cast<void*>(mesh);

                if (skeleton)
                {
                    // fix this
                    skeleton->m_CurrentTime += skeleton->GetTicksPerSecond() * ecs->m_GetDeltaTime();
                    float wtf = skeleton->GetDuration();
                    skeleton->m_CurrentTime = fmod(skeleton->m_CurrentTime, skeleton->GetDuration());
                }


                std::shared_ptr<R_Texture> diff = rm->GetResource<R_Texture>(skinnedMesh->diffuseMaterialGUID);
                std::shared_ptr<R_Texture> spec = rm->GetResource<R_Texture>(skinnedMesh->specularMaterialGUID);
                std::shared_ptr<R_Texture> norm = rm->GetResource<R_Texture>(skinnedMesh->normalMaterialGUID);
                std::shared_ptr<R_Texture> ao = rm->GetResource<R_Texture>(skinnedMesh->ambientOcclusionMaterialGUID);
                std::shared_ptr<R_Texture> rough = rm->GetResource<R_Texture>(skinnedMesh->roughnessMaterialGUID);

                if (mesh)
                    gm->gm_PushSkinnedMeshData(SkinnedMeshData{ mesh, skeleton, PBRMaterial{diff,spec,rough,ao,norm}, transform->transformation, 0.f,id });
            }
            //else
               // mesh = static_cast<R_Model*>(skinnedMesh->cachedSkinnedMeshResource);

            // TODO: Update bone matrices via AnimatorSystem
            // TODO: Submit skinned mesh (skinnedMesh->meshFile, skinnedMesh->materialFile) for rendering
        }
    }

}