/********************************************************************/
/*!
\file      Camera.h
\author    Gabe Ng 2301290 Sean Tiu 2303398
\par       gabe.ng@digipen.edu s.tiu@digipen.edu
\date      Oct 03 2025
\brief     This file declares the `CameraData` class,
		   which handles the creation and management of camera matrices
		   and transformations within a 3D scene.
		   - CalculateViewMtx: Calculates the camera's view matrix.
		   - CalculatePerspMtx: Calculates the perspective projection matrix.
		   - CalculateOrthoMtx: Calculates the orthographic projection matrix.
		   - CalculateUIOrthoMtx: Calculates the UI orthographic matrix.
		   - onCursor: Updates camera rotation based on cursor movement.
		   - onScroll: Updates camera zoom based on scroll input.
		   - CalculateAspectRatio: Returns the aspect ratio of the camera.

This file supports camera management by providing functions to calculate
view and projection matrices for rendering 3D scenes and UI elements.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#pragma once
#include "GraphicsReferences.h"
#include "ECS/LayerData.h"

class CameraData {
public:  

	CameraData() = default;
	CameraData(float fov, float nearPlane, float farPlane, glm::vec2 size, glm::vec3 position,glm::vec3 rot,glm::vec3 target, bool active,bool culling, layer::LAYERS layer);

	virtual glm::mat4 CalculateViewMtx();
	glm::mat4 CalculatePerspMtx();
	glm::mat4 CalculateOrthoMtx();
	glm::mat3 CalculateUIOrthoMtx();
	const glm::mat4& GetOrthoMtx() const;
	const glm::mat3& GetUIOrthoMtx() const;
	const glm::mat4& GetViewMtx() const;
	const glm::mat4& GetPerspMtx() const;
	virtual void onCursor(double xoffset, double yoffset);
	virtual void onScroll(double xoffset, double yoffset);
	float CalculateAspectRatio();


	float fov{ 45.0f };  // Field of view angle in degrees
	float nearPlane{0.05f};         // Near clipping plane
	float farPlane{200.f};          // Far clipping plane
	glm::vec2 size{ 1600.f,900.f };
	glm::vec3 position{0.f,0.f,-50.f}; // Current position of the camera in 3D space
	glm::vec3 rotation{ 0.f,0.f,0.f };
	bool active{ false };
	bool culling{ false };
	layer::LAYERS layer;
	glm::vec3 target{ 0.f };   // Target point that the camera is looking at
	float r{ glm::length(position) };
	float alpha{ glm::asin(position.y / r) };
	float betta{ std::atan2(position.x, position.z) };
protected:
	glm::mat4 viewMtx{ 1.0f };
	glm::mat4 perspMtx{ 1.0f };
	glm::mat4 orthoMtx{ 1.0f };
	glm::mat3 uiOrthoMtx{ 1.0f };
	float aspectRatio{ 1.f };
	
};

using Camera = CameraData;

