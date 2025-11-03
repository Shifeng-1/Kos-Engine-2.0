/********************************************************************/
/*!
\file      Renderer.h
\author    Sean Tiu (2303398)
\par       Emails: s.tiu@digipen.edu
\date      Oct 03, 2025
\brief     Declares various renderer structures used in the graphics
		   system, each responsible for rendering a specific type of
		   object within the scene.

		   This file defines several rendering modules that inherit
		   from the BasicRenderer interface:
		   - MeshRenderer: Renders 3D meshes.
		   - TextRenderer: Handles on-screen text rendering.
		   - SpriteRenderer: Draws 2D sprites and UI elements.
		   - LightRenderer: Renders different types of scene lights
			 (point, directional, and spot).
		   - DebugRenderer: Visualizes debug primitives such as
			 cubes, frustums, and light gizmos.

		   Each renderer maintains its own render data and provides
		   initialization, rendering, and clearing functions used
		   during the graphics pipeline.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#pragma once
#include"GraphicsReferences.h"
#include "Resources/ResourceHeader.h"
#include "Mesh.h"
#include "Light.h"
#include "UIElements.h"
#include "Model.h"
#include "Camera.h"
#include "Particle.h"
#include <vector>
#include "CubeMap.h"

struct BasicRenderer
{
	virtual void Clear() = 0;
};

struct MeshRenderer : BasicRenderer
{
	void Render(const CameraData& camera, Shader& shader);
	void Clear() override;
	std::vector<MeshData> meshesToDraw{};
};

struct SkinnedMeshRenderer : BasicRenderer
{
	void Render(const CameraData& camera, Shader& shader);
	void Clear() override;
	std::vector<SkinnedMeshData> skinnedMeshesToDraw{};
	std::unordered_map<unsigned int, SkinnedMeshData*> skinnedMeshLookup{}; //Entity ID, Skinned Mesh Pointer
};

struct CubeRenderer : BasicRenderer
{
	struct CubeData {
		PBRMaterial meshMaterial;
		glm::mat4 transformation{ 1.f };
		int entityID{ -1 };
	};
	void Render(const CameraData& camera, Shader& shader, Cube* cubePtr);
	void Clear();
	std::vector<CubeData> cubesToDraw{};
};
struct SphereRenderer:BasicRenderer
{
	struct SphereData {
		PBRMaterial meshMaterial;
		glm::mat4 transformation{ 1.f };
		int entityID{ -1 };
	};
	void Render(const CameraData& camera, Shader& shader, Sphere* cubePtr);
	void Clear();
	std::vector<SphereData> spheresToDraw{};

};
struct TextRenderer : BasicRenderer
{
	void InitializeTextRendererMeshes();
	void RenderScreenFonts(const CameraData& camera, Shader& shader);
	void Clear() override;
	std::vector<ScreenTextData> screenTextToDraw{};
	///std::vector<WorldTextData> worldTextToDraw{};
	TextMesh screenTextMesh;
};

struct SpriteRenderer : BasicRenderer
{
	void InitializeSpriteRendererMeshes();
	void RenderScreenSprites(const CameraData& camera, Shader& shader);
	void Clear() override;
	std::vector<ScreenSpriteData> screenSpritesToDraw{};

private:
	ScreenSpriteMesh screenSpriteMesh;
};

struct LightRenderer : BasicRenderer
{
	void InitializeLightRenderer();
	void UpdateDCM();
	void RenderAllLights(const CameraData& camera, Shader& shader);
	void DebugRender(const CameraData& camera, Shader& shader);
	void Clear() override;
	std::vector<PointLightData> pointLightsToDraw{};
	std::vector<DirectionalLightData> directionLightsToDraw{};
	std::vector<SpotLightData> spotLightsToDraw{};
	DepthCubeMap dcm[16];
	DepthCubeMap testDCM;
};
struct DebugRenderer : BasicRenderer {

	void InitializeDebugRendererMeshes();
	void Render(const CameraData& camera, Shader& shader);
	void RenderPointLightDebug(const CameraData& camera, Shader& shader, std::vector<PointLightData> pointLights);
	void RenderDebugFrustums(const CameraData& camera, Shader& shader, const std::vector<CameraData>& debugFrustums);
	void RenderDebugCubes(const CameraData& camera, Shader& shader);
	void RenderDebugCapsules(const CameraData& camera, Shader& shader);
	void RenderDebugSpheres(const CameraData& camera, Shader& shader);
	void Clear() override;

	std::vector<BasicDebugData> basicDebugCubes{};
	std::vector<BasicDebugData> basicDebugCapsules{};
	std::vector<BasicDebugData> basicDebugSpheres{};

private:
	DebugCircle debugCircle;
	DebugFrustum debugFrustum;
	DebugCube debugCube;
	DebugCapsule debugCapsule;
};

struct ParticleRenderer : BasicRenderer {
	void InitializeParticleRendererMeshes();
	void Render(const CameraData& camera, Shader& shader);
	void Clear() override;
	std::vector<BasicParticleData> particlesToDraw{};
	std::vector<BasicParticleInstance> instancedBasicParticles{};

private:
	BasicParticleMesh basicParticleMesh;
};

