/********************************************************************/
/*!
\file      Graphics Manager.cpp
\author    Gabe Ng 2301290 Sean Tiu 2303398
\par       gabe.ng@digipen.edu s.tiu@digipen.edu
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

#include "Config/pch.h"
#include "GraphicsManager.h"
#include "Camera.h"

std::shared_ptr<GraphicsManager> GraphicsManager::gm = std::make_shared<GraphicsManager>();

//Variables to debug graphics
namespace DebugGraphics {
	std::map<std::string, Shader>shaderList;
	std::vector<PBRMaterial> materialList;
	std::vector<Textures> textureList;
	std::vector<Model> modelList;

	//Lights
	SpotLightData light;
	SpotLightData light2;
	DirectionalLightData dirLight;
	SpotLightData spotLight;
	Camera camera;

	//Test skybox
	Skybox cubeMap;
}

void GraphicsManager::gm_Initialize(float width, float height) {

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	this->windowHeight = height;
	this->windowWidth = width;

	//Initialize Graphics Manager
	gm_InitializeMeshes();
	framebufferManager.Initialize(static_cast<int>(this->windowWidth), static_cast<int>(this->windowHeight));
	shaderManager.Initialize();

	//Bind shader to editor buffer
	framebufferManager.editorBuffer.shader = &shaderManager.engineShaders.find("FrameBufferShader")->second;

	lightRenderer.InitializeLightRenderer();

	DebugGraphics::cubeMap.LoadCubeModel();
	DebugGraphics::cubeMap.LoadCubeTextureDDS(
		{ "Resource/9bf58179-dd3f-5e5e-6b9b-bdf9c91d302e.dds", "Resource/e5dc8795-343f-5d23-24ec-b3f49d015de8.dds",
		  "Resource/5d8311c4-da53-b0cc-ebec-376730e44ab4.dds", "Resource/099a08b9-935e-d6b8-cbdd-1f2acf398fc8.dds",
		  "Resource/8fee6749-a54c-3071-9cd2-018187d80c78.dds", "Resource/fbc7a73b-7fe4-f273-c5c2-73b9f0e08796.dds" });

}

void GraphicsManager::gm_Update()
{

}

void GraphicsManager::gm_Render()
{
	glViewport(0, 0, static_cast<GLsizei>(windowWidth), static_cast<GLsizei>(windowHeight));

	//Force only first camera to be active for now
	currentGameCameraIndex = 0;
	if (currentGameCameraIndex + 1 <= gameCameras.size())
		gm_RenderToGameFrameBuffer();
	if (editorCameraActive) {
		gm_RenderToEditorFrameBuffer();
		/*		std::vector<float> alpha(1920 * 1080);
				glBindTexture(GL_TEXTURE_2D, framebufferManager.gBuffer.gMaterial);
				glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_FLOAT, alpha.data());
				std::cout << alpha[0] << '\n'*/
	}
	gm_Clear();
}
void GraphicsManager::gm_RenderDebug()
{
	glViewport(0, 0, 1920, 1080);
	if (editorCameraActive)
		gm_RenderToEditorFrameBuffer();

	gm_Clear();
}
void GraphicsManager::gm_Clear()
{
	gm_ClearGBuffer();
	textRenderer.Clear();
	spriteRenderer.Clear();
	meshRenderer.Clear();
	lightRenderer.Clear();
	debugRenderer.Clear();
	gameCameras.clear();
	cubeRenderer.Clear();
	skinnedMeshRenderer.Clear();
	//editorCameraActive = false;
}

void GraphicsManager::gm_ResetFrameBuffer() {
	glBindVertexArray(framebufferManager.frameBuffer.vaoId);
	glDisable(GL_BLEND);
}

void GraphicsManager::gm_InitializeMeshes()
{
	cube.CreateMesh();
	sphere.CreateMesh();
	textRenderer.InitializeTextRendererMeshes();
	spriteRenderer.InitializeSpriteRendererMeshes();
	debugRenderer.InitializeDebugRendererMeshes();
}


void GraphicsManager::gm_RenderToEditorFrameBuffer()
{
	gm_FillDataBuffers(editorCamera);
	lightRenderer.dcm[0]=lightRenderer.testDCM;

	framebufferManager.sceneBuffer.BindForDrawing();


	gm_RenderCubeMap(editorCamera);
	gm_RenderDeferredObjects(editorCamera);
	gm_RenderDebugObjects(editorCamera);

	//Particle buffer

	framebufferManager.UIBuffer.BindForDrawing();

	gm_RenderUIObjects(editorCamera);

	Shader* fboCompositeShader{ &shaderManager.engineShaders.find("FBOCompositeShader")->second };
	framebufferManager.ComposeBuffers(framebufferManager.sceneBuffer.texID, framebufferManager.UIBuffer.texID,
		framebufferManager.editorBuffer, *fboCompositeShader);


}

void GraphicsManager::gm_RenderToGameFrameBuffer()
{
	//Render deffered rendering
	gm_FillDataBuffers(gameCameras[currentGameCameraIndex]);

	framebufferManager.sceneBuffer.BindForDrawing();

	gm_RenderCubeMap(gameCameras[currentGameCameraIndex]);
	gm_RenderDeferredObjects(gameCameras[currentGameCameraIndex]);

	//Render UI
	framebufferManager.UIBuffer.BindForDrawing();
	gm_RenderUIObjects(gameCameras[currentGameCameraIndex]);

	Shader* fboCompositeShader{ &shaderManager.engineShaders.find("FBOCompositeShader")->second };
	framebufferManager.ComposeBuffers(framebufferManager.sceneBuffer.texID, framebufferManager.UIBuffer.texID,
		framebufferManager.gameBuffer, *fboCompositeShader);
}



void GraphicsManager::gm_FillDataBuffers(const CameraData& camera)
{
	gm_FillGBuffer(camera);
	gm_FillDepthBuffer(camera);
	gm_FillDepthCube(camera);
}

void GraphicsManager::gm_FillGBuffer(const CameraData& camera)
{
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//Render to G buffer 
	framebufferManager.gBuffer.BindGBuffer();
	Shader* gBufferPBRShader{ &shaderManager.engineShaders.find("GBufferPBRShader")->second };
	gBufferPBRShader->Use();
	gBufferPBRShader->SetFloat("uShaderType", 0.f);
	gBufferPBRShader->SetTrans("projection", camera.GetPerspMtx()); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	gBufferPBRShader->SetTrans("view", camera.GetViewMtx());
	gBufferPBRShader->SetVec3("cameraPosition", camera.position);
	gBufferPBRShader->SetFloat("uShaderType", 0.f);

	//Render all meshes
	meshRenderer.Render(camera, *gBufferPBRShader);
	skinnedMeshRenderer.Render(camera, *gBufferPBRShader);
	cubeRenderer.Render(camera, *gBufferPBRShader, &this->cube);
	//Render debug objects if any
	debugRenderer.RenderPointLightDebug(camera, *gBufferPBRShader, lightRenderer.pointLightsToDraw);
	debugRenderer.RenderDebugFrustums(camera, *gBufferPBRShader, gameCameras);
	debugRenderer.RenderDebugCubes(camera, *gBufferPBRShader);

	gBufferPBRShader->Disuse();


}

void GraphicsManager::gm_FillDepthBuffer(const CameraData& camera)
{
	//Render to Depth buffer
	glCullFace(GL_FRONT);
	Shader* depthMapShader{ &shaderManager.engineShaders.find("DepthMapShader")->second };
	lightRenderer.RenderAllLights(camera, *depthMapShader);

	depthMapShader->Use();

	//Manual bind
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferManager.depthBuffer.depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	//Render Objects
	meshRenderer.Render(camera, *depthMapShader);
	skinnedMeshRenderer.Render(camera, *depthMapShader);
	cubeRenderer.Render(camera, *depthMapShader, &this->cube);

	//Finish Depth Buffer
	depthMapShader->Disuse();
	glCullFace(GL_BACK);
}

void GraphicsManager::gm_FillDepthCube(const CameraData& camera) {
	
	//glCullFace(GL_FRONT);
	//for (int i{ 0 }; i < lightRenderer.pointLightsToDraw.size(); i++) {
	//	if (!lightRenderer.pointLightsToDraw[i].shadowCon)continue;;
	//	//std::cout << "SHADOW" << '\n';
	//	glViewport(0, 0, 1024.f, 1024.f);
	//	glBindFramebuffer(GL_FRAMEBUFFER, lightRenderer.dcm[i].GetFBO());
	//	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//	if (status != GL_FRAMEBUFFER_COMPLETE) {
	//		std::cout << "FBO " << i << " incomplete! Status: " << status << std::endl;
	//	}
	//	glClear(GL_DEPTH_BUFFER_BIT);
	//	pointLightShadow->Use();

	//	lightRenderer.dcm[i].FillMap(lightRenderer.pointLightsToDraw[i].position);
	//	for (int j = 0; j < 6;j++) {
	//		pointLightShadow->SetMat4("shadowMatrices[" + std::to_string(j) + "]", lightRenderer.dcm[i].shadowTransforms[j]);
	//	}
	//	pointLightShadow->SetFloat("far_plane", lightRenderer.dcm[i].far_plane);
	//	pointLightShadow->SetVec3("lightPos", lightRenderer.pointLightsToDraw[i].position);
	//	//Render Objects
	//	meshRenderer.Render(camera, *pointLightShadow);

	//	skinnedMeshRenderer.Render(camera, *pointLightShadow);
	//	cubeRenderer.Render(camera, *pointLightShadow, &this->cube);
	//	pointLightShadow->Disuse();

	//	//Render objects in shadow
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//}

	//glCullFace(GL_BACK);

			//Render to cube depth map
	Shader* pointShadowShader{ &shaderManager.engineShaders.find("PointShadowShader")->second };
	glCullFace(GL_FRONT);

	for (int i{ 0 }; i < lightRenderer.pointLightsToDraw.size(); i++) {
		if (!lightRenderer.pointLightsToDraw[i].shadowCon)continue;;
		glViewport(0, 0, 1024.f, 1024.f);
		glBindFramebuffer(GL_FRAMEBUFFER, lightRenderer.dcm[i].GetFBO());
		glClear(GL_DEPTH_BUFFER_BIT);
		pointShadowShader->Use();
		lightRenderer.dcm[i].FillMap(lightRenderer.pointLightsToDraw[i].position);
		for (unsigned int j = 0; j < 6; ++j) {
			pointShadowShader->SetMat4("shadowMatrices[" + std::to_string(j) + "]", lightRenderer.dcm[i].shadowTransforms[j]);
		}
		pointShadowShader->SetFloat("far_plane", lightRenderer.dcm[i].far_plane);
		pointShadowShader->SetVec3("lightPos", lightRenderer.pointLightsToDraw[i].position);
		for (MeshData& md : meshRenderer.meshesToDraw) {
			pointShadowShader->SetTrans("model", md.transformation);
			md.meshToUse->PBRDraw(*pointShadowShader, md.meshMaterial);

		}
		for (SkinnedMeshData& md : skinnedMeshRenderer.skinnedMeshesToDraw) {
			pointShadowShader->SetTrans("model", md.transformation);
			md.meshToUse->PBRDraw(*pointShadowShader, md.meshMaterial);

		}
		cubeRenderer.Render(camera, *pointShadowShader, &this->cube);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	pointShadowShader->Disuse();
	glCullFace(GL_BACK);

}

void GraphicsManager::gm_FillDepthCube(const CameraData& camera, int index) {
	Shader* pointShadowShader{ &shaderManager.engineShaders.find("PointShadowShader")->second };
	glCullFace(GL_FRONT);

	glViewport(0, 0, 1024.f, 1024.f);
	glBindFramebuffer(GL_FRAMEBUFFER, lightRenderer.dcm[index].GetFBO());
	glClear(GL_DEPTH_BUFFER_BIT);
	pointShadowShader->Use();
	lightRenderer.dcm[index].FillMap(lightRenderer.pointLightsToDraw[index].position);
	for (unsigned int j = 0; j < 6; ++j) {
		pointShadowShader->SetMat4("shadowMatrices[" + std::to_string(j) + "]", lightRenderer.dcm[index].shadowTransforms[j]);
	}
	pointShadowShader->SetFloat("far_plane", lightRenderer.dcm[index].far_plane);
	pointShadowShader->SetVec3("lightPos", lightRenderer.pointLightsToDraw[index].position);
	for (MeshData& md : meshRenderer.meshesToDraw) {
		pointShadowShader->SetTrans("model", md.transformation);
		md.meshToUse->PBRDraw(*pointShadowShader, md.meshMaterial);

	}
	for (SkinnedMeshData& md : skinnedMeshRenderer.skinnedMeshesToDraw) {
		pointShadowShader->SetTrans("model", md.transformation);
		md.meshToUse->PBRDraw(*pointShadowShader, md.meshMaterial);

	}
	cubeRenderer.Render(camera, *pointShadowShader, &this->cube);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	pointShadowShader->Disuse();
	glCullFace(GL_BACK);
}
void GraphicsManager::gm_DrawMaterial(const PBRMaterial& md,FrameBuffer& fb) {

	//Fill g buffer first
	glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
	glViewport(0, 0, static_cast<GLsizei>(fb.width), static_cast<GLsizei>(fb.height));
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete: " << status << std::endl;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	Shader* materialShader{ &shaderManager.engineShaders.find("MaterialShader")->second };
	materialShader->Use();
	glm::mat4 projection = glm::perspective(
		glm::radians(45.0f),
		(float)fb.width / fb.height,
		0.1f, 200.0f
	);
	materialShader->SetTrans("projection", projection);

	glm::mat4 view = glm::lookAt(
		glm::vec3(0, 0, 5),   // camera in front of cube
		glm::vec3(0, 0, 0),   // looking at cube center
		glm::vec3(0, 1, 0)
	);
	materialShader->SetTrans("view", view);

	glm::mat4 model(1.0f);   // cube at origin
	model = glm::scale(model, glm::vec3(1.75f));
	materialShader->SetTrans("model", model);


	glActiveTexture(GL_TEXTURE0); // activate proper texture unit before binding
	materialShader->SetInt("texture_diffuse1", 0);
	unsigned int currentTexture = 0;
	currentTexture = (md.albedo) ? md.albedo->RetrieveTexture() : 0;
	glBindTexture(GL_TEXTURE_2D, currentTexture);
	//Bind sepcular
	glActiveTexture(GL_TEXTURE1); // activate proper texture unit before binding
	materialShader->SetInt("texture_specular1", 1);
	currentTexture = (md.specular) ? md.specular->RetrieveTexture() : 0;
	glBindTexture(GL_TEXTURE_2D, currentTexture);
	//Bind normal
	glActiveTexture(GL_TEXTURE2); // activate proper texture unit before binding
	materialShader->SetInt("texture_normal1", 2);

	currentTexture = (md.normal) ? md.normal->RetrieveTexture() : 0;
	glBindTexture(GL_TEXTURE_2D, currentTexture);
	//Bind Metallic map
	glActiveTexture(GL_TEXTURE4); // activate proper texture unit before binding
	materialShader->SetInt("texture_ao1", 4);
	currentTexture = (md.ao) ? md.ao->RetrieveTexture() : 0;
	glBindTexture(GL_TEXTURE_2D, currentTexture);
	//Bind roughness
	glActiveTexture(GL_TEXTURE5); // activate proper texture unit before binding
	materialShader->SetInt("texture_roughness1", 5);
	currentTexture = (md.roughness) ? md.roughness->RetrieveTexture() : 0;
	glBindTexture(GL_TEXTURE_2D, currentTexture);

	glDisable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Bind materials

	sphere.DrawMesh();

	glEnable(GL_CULL_FACE);


	materialShader->Disuse();
	GLenum err = glGetError();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
void GraphicsManager::gm_RenderCubeMap(const CameraData& camera)
{
	//Render the cubemap
	DebugGraphics::cubeMap.Render(&shaderManager.engineShaders.find("SkyBoxShader")->second, glm::mat4(glm::mat3(camera.GetViewMtx())), camera.GetPerspMtx());
}

void GraphicsManager::gm_RenderDeferredObjects(const CameraData& camera)
{

	Shader* deferredPBRShader{ &shaderManager.engineShaders.find("DeferredPBRShader")->second };


	lightRenderer.RenderAllLights(camera, *deferredPBRShader);

	//Render everything else
	deferredPBRShader->Use();
	deferredPBRShader->SetVec3("lightAmbience", PointLightData::ambientStrength);
	deferredPBRShader->SetTrans("view", camera.GetViewMtx());


	deferredPBRShader->SetInt("pointLightNo", static_cast<int>(lightRenderer.pointLightsToDraw.size()));
	deferredPBRShader->SetInt("dirLightNo", static_cast<int>(lightRenderer.directionLightsToDraw.size()));
	deferredPBRShader->SetInt("spotLightNo", static_cast<int>(lightRenderer.spotLightsToDraw.size()));

	//Set depth cube maps Sean pls kill me 

	framebufferManager.gBuffer.UseGTextures();
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, DebugGraphics::cubeMap.RetrieveID());
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, framebufferManager.depthBuffer.RetrieveBuffer());

	//Fill point shadow stuff
	for (int i = 0; i < lightRenderer.pointLightsToDraw.size(); i++) {
		if (lightRenderer.pointLightsToDraw[i].bakedCon&& lightRenderer.pointLightsToDraw[i].bakedmapGUID.size()) {
			std::shared_ptr<R_DepthMapCube> dmc = ResourceManager::GetInstance()->GetResource<R_DepthMapCube>(lightRenderer.pointLightsToDraw[i].bakedmapGUID);
			glActiveTexture(GL_TEXTURE7 + i);
			glBindTexture(GL_TEXTURE_CUBE_MAP, dmc->dcm.RetrieveID());
			deferredPBRShader->SetFloat("far_plane", dmc->dcm.far_plane);
		}
		if (!lightRenderer.pointLightsToDraw[i].shadowCon&&!lightRenderer.pointLightsToDraw[i].bakedCon)continue;;
		glActiveTexture(GL_TEXTURE7 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, lightRenderer.dcm[i].RetrieveID());
		deferredPBRShader->SetFloat("far_plane", lightRenderer.dcm[i].far_plane);

	}
	GLint samplerUnits[16];
	for (int i = 0; i < 16; i++) {
		samplerUnits[i] = 7 + i; // Texture units starting from 7
	}
	deferredPBRShader->SetIntArray("depthMap", samplerUnits, 16);

	glUniform1i(glGetUniformLocation(deferredPBRShader->ID, "gPosition"), 0);  // Bind to GL_TEXTURE0
	glUniform1i(glGetUniformLocation(deferredPBRShader->ID, "gNormal"), 1);    // Bind to GL_TEXTURE1
	glUniform1i(glGetUniformLocation(deferredPBRShader->ID, "gAlbedoSpec"), 2); // Bind to GL_TEXTURE2
	glUniform1i(glGetUniformLocation(deferredPBRShader->ID, "gReflect"), 3);
	glUniform1i(glGetUniformLocation(deferredPBRShader->ID, "gMaterial"), 4);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(framebufferManager.frameBuffer.vaoId);
	glDrawElements(GL_TRIANGLE_STRIP, framebufferManager.frameBuffer.drawCount, GL_UNSIGNED_SHORT, NULL);
	glDisable(GL_BLEND);
	deferredPBRShader->Disuse();
}

void GraphicsManager::gm_RenderDebugObjects(const CameraData& camera)
{
	Shader* defaultDraw{ &shaderManager.engineShaders.find("DefaultDraw")->second };
	defaultDraw->Use();
	glDisable(GL_DEPTH_TEST);
	framebufferManager.gBuffer.UseGTextures();
	//Commented for now, we revisit in future
	//glActiveTexture(GL_TEXTURE5);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, testIrradiance.RetrieveID());
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, framebufferManager.depthBuffer.RetrieveBuffer());
	glUniform1i(glGetUniformLocation(defaultDraw->ID, "gPosition"), 0);  // Bind to GL_TEXTURE0
	glUniform1i(glGetUniformLocation(defaultDraw->ID, "gNormal"), 1);    // Bind to GL_TEXTURE1
	glUniform1i(glGetUniformLocation(defaultDraw->ID, "gAlbedoSpec"), 2); // Bind to GL_TEXTURE2
	glUniform1i(glGetUniformLocation(defaultDraw->ID, "gReflect"), 3);
	glUniform1i(glGetUniformLocation(defaultDraw->ID, "gMaterial"), 4);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(framebufferManager.frameBuffer.vaoId);
	glDrawElements(GL_TRIANGLE_STRIP, framebufferManager.frameBuffer.drawCount, GL_UNSIGNED_SHORT, NULL);
	glDisable(GL_BLEND);
	defaultDraw->Disuse();
}

void GraphicsManager::gm_RenderUIObjects(const CameraData& camera)
{
	Shader* screenSpriteShader{ &shaderManager.engineShaders.find("ScreenSpriteShader")->second };
	spriteRenderer.RenderScreenSprites(camera, *screenSpriteShader);
	Shader* fontShader{ &shaderManager.engineShaders.find("ScreenFontShader")->second };
	textRenderer.RenderScreenFonts(camera, *fontShader);
}

void GraphicsManager::gm_ClearGBuffer()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferManager.gBuffer.RetrieveBuffer());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, static_cast<GLint>(windowWidth), static_cast<GLint>(windowHeight), 0, 0,
		static_cast<GLint>(windowWidth), static_cast<GLint>(windowHeight), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}