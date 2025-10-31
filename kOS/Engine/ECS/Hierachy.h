/********************************************************************/
/*!
\file      Hierachy.h
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Nov 11, 2025
\brief     This header file declares the `Hierachy` class, which manages
		   the parent-child relationships between entities within the ECS framework.
		   - m_SetParent: Sets a parent-child relationship between two entities.
		   - m_RemoveParent: Removes an entity’s parent relationship.
		   - m_GetParent: Retrieves the parent of a specified entity.
		   - m_GetChild: Retrieves the children of a specified entity.
		   - m_UpdateChildScene: Updates the scene of all child entities recursively.

This file enables hierarchical organization of entities, allowing for complex
scene structures and efficient management of entity relationships.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#ifndef HIERACHY_H

#include "ECS.h"

using namespace ecs;

namespace hierachy {

	void m_SetParent(EntityID parent, EntityID child, bool updateTransform = false);

	void m_RemoveParent(EntityID child);

	std::optional<EntityID> GetParent(EntityID child);

	std::optional<std::vector<EntityID>> m_GetChild(EntityID parent);

	void m_UpdateChildScene(EntityID parent);

}

#define HIERACHY_H
#endif HIREACHY_H

