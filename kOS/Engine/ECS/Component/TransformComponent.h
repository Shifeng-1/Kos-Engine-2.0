/******************************************************************/
/*!
\file      TransformComponent.h
\author    Ng Jaz winn, jazwinn.ng , 2301502
\par       jazwinn.ng@digipen.edu
\date      Oct 02, 2024
\brief     This file contains data for the trnasformation of the object


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#ifndef TRANSCOM_H
#define TRANSCOM_H

#include "Component.h"


namespace ecs {
	/******************************************************************/
	/*!
	\class     TransformComponent
	\brief     Contains data required for transformation
	*/
	/******************************************************************/
	struct Transformation {
		glm::vec3 position{ 0,0,0 };
		glm::vec3 rotation{ 0,0,0 };
		glm::vec3 scale{ 1,1,1 };

		REFLECTABLE(Transformation, position, rotation, scale)
	};



	class TransformComponent : public Component {

	public:

		Transformation LocalTransformation;
		Transformation WorldTransformation;

		// TRS for entity (World Transform)
		glm::mat4 transformation = glm::mat4(1.0f);
		// Transformation Mtx with the parent as its origin 
		glm::mat4 localTransform = glm::mat4(1.0f); // parentTransform * localTransform = transformation

		//for Entities with child
		std::vector<EntityID> m_childID{};
		
		//for entities with parents
		EntityID m_parentID{ -1 };
		
		bool m_haveParent{false};

		REFLECTABLE(TransformComponent, WorldTransformation, LocalTransformation)
	};

}



#endif TRANSCOM_H