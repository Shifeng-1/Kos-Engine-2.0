/********************************************************************/
/*!
\file      CanvasSpriteRenderSystem.cpp
\author    Sean Tiu (2303398)
\par       Email: s.tiu@digipen.edu
\date      Oct 01, 2025
\brief     Defines the CanvasSpriteRenderSystem class, which handles
           the rendering of 2D sprites in UI canvases within the ECS
           system.

           This system:
           - Renders screen-space attached to
             entities with CanvasRendererComponents.
           - Interfaces with the rendering pipeline to draw UI sprites
             efficiently.
           - Runs every frame to update visual elements in the user
             interface.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "ECS/ECS.h"

#include "CanvasSpriteRenderSystem.h"
#include "Resources/ResourceManager.h"
#include "ECS/Hierachy.h"
#include "Graphics/GraphicsManager.h"

namespace ecs {

    void CanvasSpriteRenderSystem::Init()
    {
        // Initialize UI rendering resources if needed
    }

    void CanvasSpriteRenderSystem::Update()
    {
        ECS* ecs = ECS::GetInstance();
        const auto& entities = m_entities.Data();

        for (const EntityID id : entities) {
            TransformComponent* transform = ecs->GetComponent<TransformComponent>(id);
            NameComponent* nameComp = ecs->GetComponent<NameComponent>(id);
            CanvasRendererComponent* canvas = ecs->GetComponent<CanvasRendererComponent>(id);

            // Skip entities not in this scene or hidden
            if (!ecs->layersStack.m_layerBitSet.test(nameComp->Layer) || nameComp->hide)
                continue;

            std::optional<std::vector<EntityID>> childEntities = hierachy::m_GetChild(id);
            if (!childEntities.has_value()) continue;

            for (EntityID childID : childEntities.value())
            {
                std::shared_ptr<ResourceManager> rm = ResourceManager::GetInstance();
                std::shared_ptr<GraphicsManager> gm = GraphicsManager::GetInstance();
                if (ecs->HasComponent<SpriteComponent>(childID))
                {
                    SpriteComponent* spriteComp = ecs->GetComponent<SpriteComponent>(childID);
                    TransformComponent* childTransform = ecs->GetComponent<TransformComponent>(childID);
                    if (!spriteComp->spriteGUID.empty())
                    {
                        std::shared_ptr<R_Texture> fontResource = rm->GetResource<R_Texture>(spriteComp->spriteGUID);
                        gm->gm_PushScreenSpriteData(ScreenSpriteData{ childTransform->WorldTransformation.position,
                                                           glm::vec2{ childTransform->WorldTransformation.scale.x, 
                                                                      childTransform->WorldTransformation.scale.y},
                                                                      -childTransform->WorldTransformation.rotation.x, spriteComp->color,
                                                                      fontResource.get(), 0, 0, 0,childID }); /// Temporarily all 0
                    }
                }

            }
        }
    }

}