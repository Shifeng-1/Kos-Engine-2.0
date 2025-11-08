/******************************************************************/
/*!
\file      ECS.cpp
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      8 Nov, 2024
\brief     This header file defines the `LayerStack` class and associated
		   enums for handling layer management in the ECS framework.
		   - LayerStack: Manages named layers that contain entities.
		   - m_ChangeLayerName: Renames a specified layer.
		   - m_SwapEntityLayer: Moves an entity from one layer to another.
		   - m_RetrieveEntityID: Retrieves all entity IDs within a specified layer.

This file allows flexible layer-based organization of entities in the ECS system,
providing functionality for managing multiple layers for scene composition.



Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "Layers.h"
#include "Debugging/Logging.h"
#include "ECS/ECS.h"


namespace layer {


	void LayerStack::m_ChangeLayerName(LAYERS layer, std::string newName)
	{
		if (m_layerMap.find(layer) == m_layerMap.end()) {

			LOGGING_WARN("Layer does no exist");
			return ;
		}

		m_layerMap[layer] = newName;

	}

	bool LayerStack::m_SwapEntityLayer(LAYERS newlayer, LAYERS oldlayer, ecs::EntityID id)
	{
		// same layer, do nothing
		if (newlayer == oldlayer) return true;

		if (m_layerMap.find(newlayer) == m_layerMap.end()) {
			
			LOGGING_WARN("Layer does no exist");
			return false;
		}

		if (m_layerMap.find(oldlayer) == m_layerMap.end()) {

			LOGGING_WARN("Layer does no exist");
			return false;
		}

		// add id to new layer
		//m_layerMap[newlayer].second.push_back(id);
		//m_layerMap[oldlayer].second.erase(std::find(m_layerMap[oldlayer].second.begin(), m_layerMap[oldlayer].second.end(), id));

		//assign ecs layer
		ecs::NameComponent* nc = m_ecs.GetComponent<ecs::NameComponent>(id);
		nc->Layer = newlayer;

		return true;
	}

	void LayerStack::m_DisableLayer(LAYERS layer)
	{
		m_layerBitSet.reset(layer);

	}

	void LayerStack::m_EnableLayer(LAYERS layer)
	{
		m_layerBitSet.set(layer);
	}

	bool LayerStack::m_IsLayerVisable(int layer)
	{
		return m_layerBitSet.test((LAYERS)layer);
	}


}