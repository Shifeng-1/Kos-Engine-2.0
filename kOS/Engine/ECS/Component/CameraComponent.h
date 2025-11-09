/********************************************************************/
/*!
\file      CameraComponent.h
\author    Sean Tiu (2303398)
\par       Email: s.tiu@digipen.edu
\date      Oct 01, 2025
\brief     Declares the CameraComponent class, which defines the
           properties and behavior of a camera within the ECS system.

           The CameraComponent stores data required for rendering
           from a specific viewpoint, including:
           - Camera type (Perspective or Orthographic)
           - Field of view (FOV)
           - Near and far clipping planes
           - Viewport size
           - Target direction vector
           - Active state flag

           This component allows entities to represent cameras that
           can be used for scene rendering, editor previews, or
           gameplay perspectives.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#ifndef CAMCOM_H
#define CAMCOM_H

#include "Component.h"

namespace ecs {

    class CameraComponent : public Component {
    public:

		enum CameraPerspectiveType
		{
			PERSPECTIVE,
			ORTHOGRAPHIC
		};

		CameraPerspectiveType cameraType{ PERSPECTIVE };
		float fov{ 45.0f };  // Field of view angle in degrees
		float nearPlane{ 0.05f };         // Near clipping plane
		float farPlane{ 200.f };          // Far clipping plane
		glm::vec2 size{ 1600.f,900.f };
		glm::vec3 target{ 0.f, 0.f, -1.f };   // Target point that the camera is looking 
		bool active{ false };
        bool culling{ false };
        layer::LAYERS layer;
		REFLECTABLE(CameraComponent, cameraType, fov, nearPlane, farPlane, size, target, active,culling,layer);
    };

}

#endif // CAMCOM_H
