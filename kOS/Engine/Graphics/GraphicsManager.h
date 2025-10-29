/********************************************************************/
/*!
\file      Graphics Manager.h
\author    Gabe Ng 2301290 Sean Tiu
\par       gabe.ng@digipen.edu
\date      Oct 03 2025
\brief     The main graphics manager for our system
		   It initializes all the necessary objects to be used for
		   rendering graphics.
		   Houses functions to be called on update as well

		   Rundown for the graphics system goes as follows
		   - Objects rendered to the Gbuffer
		   - Depth buffer is rendered
		   - Different rendering calls are called depending on the shader type

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#pragma once
#include "GraphicsReferences.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Framebuffer.h"
#include "Light.h"
#include "UIElements.h"
#include "Camera.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "FramebufferManager.h"

class GraphicsManager
{
public:
	//Singleton class, remove all copy and assignment operations
	GraphicsManager() = default;
	GraphicsManager(const GraphicsManager&) = delete;
	GraphicsManager& operator=(const GraphicsManager&) = delete;
	GraphicsManager(GraphicsManager&&) = delete;
	GraphicsManager& operator=(GraphicsManager&&) = delete;

	//Accessor to instance
	static std::shared_ptr<GraphicsManager> GetInstance()
	{
		if (!gm)
		{
			gm = std::make_shared<GraphicsManager>();
		}
		return gm;
	}

	//Main Functions
	void gm_Initialize(float width, float height);
	void gm_Update();
	void gm_Render();
	void gm_RenderDebug();
	void gm_Clear();
	void gm_ClearGBuffer();
	void gm_RenderDeferredObjects(const CameraData& camera);
	void gm_ResetFrameBuffer();
	FramebufferManager* gm_GetFBM() { return &framebufferManager;; }

	//Data Transfer Functions
	inline void gm_PushScreenTextData(ScreenTextData&& fontData) { textRenderer.screenTextToDraw.emplace_back(std::move(fontData)); };
	inline void gm_PushMeshData(MeshData&& meshData) { meshRenderer.meshesToDraw.emplace_back(std::move(meshData)); };
	inline void gm_PushScreenSpriteData(ScreenSpriteData&& spriteData) { spriteRenderer.screenSpritesToDraw.emplace_back(std::move(spriteData)); };
	inline void gm_PushPointLightData(PointLightData&& pointLightData) { lightRenderer.pointLightsToDraw.emplace_back(std::move(pointLightData)); };
	inline void gm_PushDirectionalLightData(DirectionalLightData&& directionalLightData) { lightRenderer.directionLightsToDraw.emplace_back(std::move(directionalLightData)); };
	inline void gm_PushSpotLightData(SpotLightData&& spotLightData) { lightRenderer.spotLightsToDraw.emplace_back(std::move(spotLightData)); };
	inline void gm_MoveEditorCameraData(const CameraData& camera) { editorCamera = camera; editorCameraActive = true; };
	inline void gm_PushGameCameraData(CameraData&& camera) { gameCameras.emplace_back(std::move(camera)); };
	inline void gm_PushCubeDebugData(BasicDebugData&& data) { debugRenderer.basicDebugCubes.emplace_back(std::move(data)); };
	inline void gm_PushCubeData(CubeRenderer::CubeData&& data) { cubeRenderer.cubesToDraw.emplace_back(std::move(data)); };
	void gm_DrawMaterial(const PBRMaterial& md, FrameBuffer& fb);
	inline void gm_PushSkinnedMeshData(SkinnedMeshData&& skinnedMeshData) {
		skinnedMeshRenderer.skinnedMeshesToDraw.emplace_back(std::move(skinnedMeshData));
		skinnedMeshRenderer.skinnedMeshLookup[skinnedMeshRenderer.skinnedMeshesToDraw.back().entityID]
			= &skinnedMeshRenderer.skinnedMeshesToDraw.back();
	};

	//Accessors
	inline const FrameBuffer& gm_GetEditorBuffer() const { return framebufferManager.editorBuffer; };
	inline const FrameBuffer& gm_GetGameBuffer() const { return framebufferManager.gameBuffer; };

private:
	//One and only active GraphicsManager object
	static std::shared_ptr<GraphicsManager> gm;

	//Initialize functions
	void gm_InitializeMeshes();

	//Render functions
	void gm_RenderToEditorFrameBuffer();
	void gm_RenderToGameFrameBuffer();
	void gm_FillDataBuffers(const CameraData& camera);
	void gm_FillGBuffer(const CameraData& camera);
	void gm_FillDepthBuffer(const CameraData& camera);
	void gm_RenderCubeMap(const CameraData& camera);
	void gm_RenderDebugObjects(const CameraData& camera);
	void gm_RenderUIObjects(const CameraData& camera);
	//Cameras
	CameraData editorCamera{};
	std::vector<CameraData> gameCameras{};
	bool editorCameraActive{ false };
	int currentGameCameraIndex{};

	//Renderers
	TextRenderer textRenderer;
	SpriteRenderer spriteRenderer;
	MeshRenderer meshRenderer;
	SkinnedMeshRenderer skinnedMeshRenderer;
	LightRenderer lightRenderer;
	DebugRenderer debugRenderer;
	CubeRenderer cubeRenderer;
	//Managers
	ShaderManager shaderManager;
	FramebufferManager framebufferManager;

	Cube cube;
	Sphere sphere;
	//Viewport sizes
	float windowWidth, windowHeight;

};