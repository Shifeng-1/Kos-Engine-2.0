/********************************************************************/
/*!
\file      LightingSystem.cpp
\author    Sean Tiu (2303398)
\par       Email: s.tiu@digipen.edu
\date      Oct 01, 2025
\brief     Defines the LightingSystem class, which manages light
		   components and updates lighting data in the ECS system.

		   This system:
		   - Updates and uploads light data (directional, point,
			 and spot lights) to GPU shaders.
		   - Interfaces with deferred and forward rendering passes.
		   - Ensures lighting information remains synchronized
			 across the rendering pipeline.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "ECS/ECS.h"

#include "LightingSystem.h"
#include "ECS/Component/LightComponent.h"
#include "Graphics/GraphicsManager.h"

namespace ecs {



	void LightingSystem::Init()
	{

	}

	void LightingSystem::Update()
	{
		const auto& entities = m_entities.Data();

		for (const EntityID id : entities) {
			TransformComponent* transform = m_ecs.GetComponent<TransformComponent>(id);
			NameComponent* nameComp = m_ecs.GetComponent<NameComponent>(id);
			LightComponent* light = m_ecs.GetComponent<LightComponent>(id);

			//skip component not of the scene
			if (nameComp->hide) continue;

			switch (light->lightType)
			{
			case LightComponent::LightType::POINTLIGHT: {
				m_graphicsManager.gm_PushPointLightData(PointLightData{ transform->LocalTransformation.position, light->color, light->diffuseStrength,
										light->specularStrength,light->linear,light->quadratic,light->intesnity,light->shadowCast,light->bakedLighting,light->depthMapGUID });
				std::shared_ptr<R_DepthMapCube> ptr = m_resourceManager.GetResource<R_DepthMapCube>(light->depthMapGUID);;
				if (ptr)m_graphicsManager.lightRenderer.dcm[m_graphicsManager.lightRenderer.pointLightsToDraw.size() - 1] = ptr->dcm;
			}
				break;
			case LightComponent::LightType::DIRECTIONAL:
				m_graphicsManager.gm_PushDirectionalLightData(DirectionalLightData{ transform->LocalTransformation.position, light->color, light->diffuseStrength,
														light->specularStrength,light->linear,light->quadratic, light->intesnity,light->direction });
				break;
			case LightComponent::LightType::SPOTLIGHT:
				m_graphicsManager.gm_PushSpotLightData(SpotLightData{ transform->LocalTransformation.position, light->color, light->diffuseStrength,
														light->specularStrength,light->linear,light->quadratic,light->intesnity,light->direction, light->cutOff,
														light->outerCutOff});
				break;
			default:
				break;
			}

		}

	}


}

