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

namespace ecs
{

    void SkinnedMeshRenderSystem::Init()
    {
        // Initialize skinned mesh rendering resources if needed
    }

    void SkinnedMeshRenderSystem::Update()
    {

        const auto &entities = m_entities.Data();

        for (const EntityID id : entities)
        {
            TransformComponent *transform = m_ecs.GetComponent<TransformComponent>(id);
            NameComponent *nameComp = m_ecs.GetComponent<NameComponent>(id);
            SkinnedMeshRendererComponent *skinnedMesh = m_ecs.GetComponent<SkinnedMeshRendererComponent>(id);
            AnimatorComponent *anim = m_ecs.GetComponent<AnimatorComponent>(id);
            // Skip entities not in this scene or hidden
            if (nameComp->hide)
                continue;

            R_Model *mesh{};
            R_Animation *skeleton{};
            // if (skinnedMesh->cachedSkinnedMeshGUID != skinnedMesh->skinnedMeshGUID)
            {

                skinnedMesh->cachedSkinnedMeshGUID = skinnedMesh->skinnedMeshGUID;
                skinnedMesh->cachedSkinnedMeshResource = static_cast<void *>(mesh);

                std::shared_ptr<R_Material> mat = m_resourceManager.GetResource<R_Material>(skinnedMesh->materialGUID);
                if (!mat)
                    return;
                ;
                skeleton = m_resourceManager.GetResource<R_Animation>(skinnedMesh->skeletonGUID).get();
                mesh = m_resourceManager.GetResource<R_Model>(skinnedMesh->skinnedMeshGUID).get();

                if (skeleton && anim->m_IsPlaying)
                {
                    anim->m_CurrentTime += skeleton->GetTicksPerSecond() * m_ecs.m_GetDeltaTime() * anim->m_PlaybackSpeed;
                    anim->m_CurrentTime = fmod(anim->m_CurrentTime, skeleton->GetDuration());
                }
                std::shared_ptr<R_Texture> diff = m_resourceManager.GetResource<R_Texture>(mat->md.diffuseMaterialGUID);
                std::shared_ptr<R_Texture> spec = m_resourceManager.GetResource<R_Texture>(mat->md.specularMaterialGUID);
                std::shared_ptr<R_Texture> norm = m_resourceManager.GetResource<R_Texture>(mat->md.normalMaterialGUID);
                std::shared_ptr<R_Texture> ao = m_resourceManager.GetResource<R_Texture>(mat->md.ambientOcclusionMaterialGUID);
                std::shared_ptr<R_Texture> rough = m_resourceManager.GetResource<R_Texture>(mat->md.roughnessMaterialGUID);

                // std::shared_ptr<R_Texture> diff = m_resourceManager.GetResource<R_Texture>(skinnedMesh->diffuseMaterialGUID);
                // std::shared_ptr<R_Texture> spec = m_resourceManager.GetResource<R_Texture>(skinnedMesh->specularMaterialGUID);
                // std::shared_ptr<R_Texture> norm = m_resourceManager.GetResource<R_Texture>(skinnedMesh->normalMaterialGUID);
                // std::shared_ptr<R_Texture> ao = m_resourceManager.GetResource<R_Texture>(skinnedMesh->ambientOcclusionMaterialGUID);
                // std::shared_ptr<R_Texture> rough = m_resourceManager.GetResource<R_Texture>(skinnedMesh->roughnessMaterialGUID);

                if (mesh)
                    m_graphicsManager.gm_PushSkinnedMeshData(SkinnedMeshData{mesh, skeleton, PBRMaterial{diff, spec, rough, ao, norm}, transform->transformation, anim->m_CurrentTime, id});
            }
            // else
            //  mesh = static_cast<R_Model*>(skinnedMesh->cachedSkinnedMeshResource);
        }
    }

}