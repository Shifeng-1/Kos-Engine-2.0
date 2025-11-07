/********************************************************************/
/*!
\file      Camera.cpp
\author    Gabe Ng 2301290 Sean Tiu 2303398
\par       gabe.ng@digipen.edu s.tiu@digipen.edu
\date      Oct 03 2025
\brief     This file provides the definitions for the `CameraData` class,
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

#include "Config/pch.h"
#include "Camera.h"

CameraData::CameraData(float fieldOfView, float nearP, float farP, glm::vec2 cameraSize, glm::vec3 pos,glm::vec3 rot ,glm::vec3 targ, bool cameraActive, bool cull, layer::LAYERS lay)
    : fov(fieldOfView), nearPlane(nearP), farPlane(farP), size(cameraSize), position(pos),rotation(rot), target(targ), active(cameraActive),culling(cull),layer(lay)
{
    rotation.y = fmod(rotation.y, 360.0f);
    rotation.x = glm::clamp(rotation.x, -89.0f, 89.0f);     
    CalculateViewMtx();
    CalculatePerspMtx();
    CalculateUIOrthoMtx();
}

glm::mat4 CameraData::CalculateViewMtx() {

    //Get right and up vectors
    glm::vec3 direction,right,up;
    direction.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
    direction.y = sin(glm::radians(rotation.x));
    direction.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
    direction = glm::normalize(direction);
    right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, direction));

    //Compute final look at direction 
    viewMtx = glm::lookAt(position, position +direction, up);
    return viewMtx;
}

const glm::mat4& CameraData::GetViewMtx() const { return viewMtx; }

glm::mat4 CameraData::CalculatePerspMtx() {
    //Note: base these values for shadow mapping... thinking
    perspMtx = glm::perspective(glm::radians(fov), size.x / size.y, nearPlane, farPlane);
    return perspMtx;
}

const glm::mat4& CameraData::GetPerspMtx() const { return perspMtx; }

glm::mat4 CameraData::CalculateOrthoMtx()
{
    orthoMtx = glm::ortho(
        0.0f, size.x,   // left, right
        0.0f, size.y,   // bottom, top
        -1.0f, 1.0f     // near, far
    );
    return orthoMtx;
}

glm::mat3 CameraData::CalculateUIOrthoMtx()
{
    CalculateAspectRatio();
    float left = -1.f * (1.f / aspectRatio);
    float right = 1.f * (1.f / aspectRatio);
    float bottom = -1.f;
    float top = 1.f;
    uiOrthoMtx[0][0] = (2.0f / (right - left));
    uiOrthoMtx[1][1] = 2.0f / (top - bottom);
    uiOrthoMtx[2][0] = -(right + left) / (right - left);
    uiOrthoMtx[2][1] = -(top + bottom) / (top - bottom);
    uiOrthoMtx[2][2] = 1;
    return uiOrthoMtx;
}

const glm::mat4& CameraData::GetOrthoMtx() const
{
    return orthoMtx;
}

const glm::mat3& CameraData::GetUIOrthoMtx() const
{
    return uiOrthoMtx;
}

// Handles cursor movement to adjust camera orientation
// Pure virtual function
void CameraData::onCursor(double xoffset, double yoffset)
{
  
}

void CameraData::onScroll(double xoffset, double yoffset)
{
        r += yoffset > 0.0f ? -1.0f : 1.0f; // Zoom in or out
        if (r < 1.0f) r = 1.0f; // Clamp minimum distance
        position.x = target.x + r * glm::cos(alpha) * glm::sin(betta);
        position.y = target.y+ r * glm::sin(alpha);
        position.z = target.z + r * glm::cos(alpha) * glm::cos(betta);
}

float CameraData::CalculateAspectRatio()
{
    aspectRatio = size.y / size.x;
    return aspectRatio;
}
