/******************************************************************/
/*!
\file      Layers.h
\author    Jaz Winn Ng
\par       jazwinn.ng@digipen.edu
\date      Sept 28, 2025
\brief	   Layers class manages different rendering layers within the ECS framework.
			- m_ChangeLayerName: Renames a specified layer.
			- m_SwapEntityLayer: Moves an entity from one layer to another.
			- m_DisableLayer: Disables rendering for a specified layer.
			- m_EnableLayer: Enables rendering for a specified layer.
			- m_IsLayerVisable: Checks if a specified layer is currently visible.
			- m_RetrieveEntityID: Retrieves all entity IDs within a specified layer.
			- m_hideEntitywithChild: Hides an entity and all its child entities.
			- m_unhideEntitywithChild: Unhides an entity and all its child entities.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/

#ifndef LAYERS_H
#define LAYERS_H

#include "ECSList.h"
#include "ECS/ECS.h"
#include "LayerData.h"


namespace layer{

	class LayerStack {
		ecs::ECS& m_ecs;
	public:
		LayerStack(ecs::ECS& ecs) :
			m_ecs(ecs)
		{
		
			m_layerBitSet.set();// assign all bits to true

			//set max layers
			m_layerMap[DEFAULT] = "Default";
			m_layerMap[LAYER1] = "Layer 1";
			m_layerMap[LAYER2] = "Layer 2";
			m_layerMap[LAYER3] = "Layer 3";
			m_layerMap[LAYER4] = "Layer 4";
			m_layerMap[LAYER5] = "Layer 5";
			m_layerMap[LAYER6] = "Layer 6";
			m_layerMap[LAYER7] = "Layer 7";
			m_layerMap[LAYER8] = "Layer 8";

		}


		using layermap = std::unordered_map<LAYERS, std::string>;


		/******************************************************************/
		/*!
			\fn        void m_ChangeLayerName(LAYERS layer, std::string newName)
			\brief     Renames a specified layer.
			\param[in] layer The enum value of the layer to rename.
			\param[in] newName The new name for the layer.
			\details   Updates the specified layers name, if the layer exists.
		*/
		/******************************************************************/
		void m_ChangeLayerName(LAYERS layer, std::string newName);

		/******************************************************************/
		/*!
			\fn        bool m_SwapEntityLayer(LAYERS newlayer, LAYERS oldlayer, ecs::EntityID id)
			\brief     Moves an entity from one layer to another.
			\param[in] newlayer The layer to move the entity to.
			\param[in] oldlayer The layer from which to move the entity.
			\param[in] id       The ID of the entity to move.
			\return    True if the entity was successfully moved; otherwise, false.
			\details   Transfers the entity ID between layers and updates its layer attribute.
		*/
		/******************************************************************/
		bool m_SwapEntityLayer(LAYERS newlayer, LAYERS oldlayer, ecs::EntityID);

		/******************************************************************/
		/*!
		\fn      void m_DisableLayer(LAYERS layer)
		\brief   Disables rendering for the specified layer.
		\param   layer - The layer to be disabled.
		*/
		/******************************************************************/
		void m_DisableLayer(LAYERS layer);

		/******************************************************************/
		/*!
		\fn      void m_EnableLayer(LAYERS layer)
		\brief   Enables rendering for the specified layer.
		\param   layer - The layer to be enabled.
		*/
		/******************************************************************/
		void m_EnableLayer(LAYERS layer);

		/******************************************************************/
		/*!
		\fn      bool m_IsLayerVisable(int layer)
		\brief   Checks whether the specified layer is currently visible.
		\param   layer - The layer index to check.
		\return  True if the layer is visible, false otherwise.
		*/
		/******************************************************************/
		bool m_IsLayerVisable(int layer);



	public:
		layermap m_layerMap;

		// layers that are activated
		std::bitset<MAXLAYER> m_layerBitSet;
	};
}
#endif LAYERS_H