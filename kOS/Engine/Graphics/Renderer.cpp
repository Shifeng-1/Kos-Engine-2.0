/********************************************************************/
/*!
\file      Renderer.cpp
\author    Sean Tiu (2303398)
\par       Emails: s.tiu@digipen.edu
\date      Oct 03, 2025
\brief     Defines various renderer structures used in the graphics
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
#include "Config/pch.h"
#include "Renderer.h"
#include "Resources/R_Font.h"
#include "Resources/R_Model.h"

void TextRenderer::InitializeTextRendererMeshes()
{
	screenTextMesh.CreateMesh();
}

void TextRenderer::RenderScreenFonts(const CameraData& camera, Shader& shader)
{
	if (!screenTextToDraw.empty() && camera.size.y != 0 && camera.size.x != 0) // Need to check for camera size
	{
		for (ScreenTextData& textData : screenTextToDraw)
		{
			//if (!textData.fontToUse)continue;;
			// Activate corresponding render state
			Shader& fontShader{ shader };
			fontShader.Use();
			glm::vec3 point = textData.position;
			constexpr float radianConversion = 3.1451f / 180.f;
			float angle = textData.rotation * radianConversion;

			glm::mat3 rotationMatrix = {
			cos(angle), -sin(angle), 0.0f,
			sin(angle), cos(angle),  0.0f,
			0.0f,       0.0f,       1.0 };

			fontShader.SetMat3("projection", camera.GetUIOrthoMtx()); // Orthographic Projection
			fontShader.SetMat3("rotate", rotationMatrix);
			fontShader.SetVec3("point", point);
			fontShader.SetVec4("textColor", textData.color);

			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(screenTextMesh.vaoID);
			float originalXPosition{ textData.position.x };
			float originX{ textData.position.x };
			float originY{ textData.position.y };

			constexpr float loadedFontSize = 48.f;
			const float textXScalar = textData.scale.x * textData.fontSize / loadedFontSize;
			const float textYScalar = textData.scale.y * textData.fontSize / loadedFontSize;

			if (textData.isCentered)
			{
				// Step 1: Calculate total width and height of the text
				float totalWidth = 0.0f;
				float maxAscent = 0.0f;
				float maxDescent = 0.0f;
				for (int i = 0; i < textData.textToDraw.size(); ++i) // To calculate the total width of the text
				{
					char c = textData.textToDraw[i];
					R_Font::CharacterData ch = textData.fontToUse->m_characters[c];
					if (i == textData.textToDraw.size() - 1) // If it's the last letter, add only the letter size
					{
						totalWidth += ((ch.m_size.x * textXScalar) / ((static_cast<float>(camera.size.y))));
					}
					else // Add the letter size and the space
					{
						totalWidth += ((ch.m_advance) * textXScalar) / ((static_cast<float>(camera.size.y)));
					}
					maxAscent = std::max(maxAscent, (ch.m_bearing.y * textYScalar) / ((static_cast<float>(camera.size.y))));
					maxDescent = std::max(maxDescent, ((ch.m_size.y - ch.m_bearing.y) * textYScalar) / ((static_cast<float>(camera.size.y))));
				}
				float totalHeight = maxAscent + maxDescent;

				// Adjust starting position to center the text
				textData.position.x = originX - totalWidth / 2.0f;  // Horizontal centering
				textData.position.y = originY + maxAscent / 2.0f - totalHeight / 2.0f;  // Vertical centering
			}

			else
			{
				if (!textData.textToDraw.size())return;
				//std::cout << textData.fontToUse->m_characters.size()<< '\n';
				//float xpos = (fonts["SF-Pro.ttf"][textData.textToDraw[0]].m_bearing.x / ((static_cast<float>(camera.size.y))) * (textXScalar));
				float xpos = (textData.fontToUse->m_characters[textData.textToDraw[0]].m_bearing.x
					/ ((static_cast<float>(camera.size.y))) * (textXScalar));
				textData.position.x -= xpos;
			}


			// Step 2: Render the text
			for (const char& c : textData.textToDraw)
			{
				R_Font::CharacterData ch = textData.fontToUse->m_characters[c];
				// Calculate position and size for each character quad
				float xpos = (textData.position.x + ch.m_bearing.x / ((static_cast<float>(camera.size.y))) * (textXScalar));
				float ypos = (textData.position.y - ((float)ch.m_size.y - (float)ch.m_bearing.y) / ((static_cast<float>(camera.size.y))) * (textYScalar));
				float w = ch.m_size.x * textXScalar / ((static_cast<float>(camera.size.y)));
				float h = ch.m_size.y * textYScalar / ((static_cast<float>(camera.size.y)));

				// Update VBO for each character with texture coordinates from the atlas
				float vertices[6][4] =
				{
					{ xpos,     ypos + h,   ch.m_topLeftTexCoords.x, ch.m_topLeftTexCoords.y },
					{ xpos,     ypos,       ch.m_topLeftTexCoords.x, ch.m_bottomRightTexCoords.y },
					{ xpos + w, ypos,       ch.m_bottomRightTexCoords.x, ch.m_bottomRightTexCoords.y },

					{ xpos,     ypos + h,   ch.m_topLeftTexCoords.x, ch.m_topLeftTexCoords.y },
					{ xpos + w, ypos,       ch.m_bottomRightTexCoords.x, ch.m_bottomRightTexCoords.y },
					{ xpos + w, ypos + h,   ch.m_bottomRightTexCoords.x, ch.m_topLeftTexCoords.y }
				};

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				// Bind the texture atlas (once for all characters)
				glBindTexture(GL_TEXTURE_2D, ch.m_textureID);

				// Update the content of the VBO memory
				glBindBuffer(GL_ARRAY_BUFFER, screenTextMesh.vboID);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				// Render quad
				glDrawArrays(GL_TRIANGLES, 0, 6);

				glDisable(GL_BLEND);

				// Advance 
				//  for next glyph
				textData.position.x += ((ch.m_advance) * textXScalar) / ((static_cast<float>(camera.size.y)));
			}
			textData.position.x = originalXPosition;

			// Unbind for safety
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void TextRenderer::Clear()
{
	///std::cout << "CLEARING DATA\n";
	screenTextToDraw.clear();
}

void MeshRenderer::Render(const CameraData& camera, Shader& shader)
{
	shader.SetBool("isNotRigged", true);
	shader.SetVec3("color", glm::vec3{1.f,1.f,1.f});
	for (MeshData& mesh : meshesToDraw)
	{
		shader.SetTrans("model", mesh.transformation);
		shader.SetInt("entityID", mesh.entityID+1);

		mesh.meshToUse->PBRDraw(shader, mesh.meshMaterial);
	}
}

void SkinnedMeshRenderer::Render(const CameraData& camera, Shader& shader)
{
	shader.SetBool("isNotRigged", false);
	shader.SetVec3("color", glm::vec3{ 1.f,1.f,1.f });
	for (SkinnedMeshData& mesh : skinnedMeshesToDraw)
	{
		shader.SetTrans("model", mesh.transformation);
		shader.SetInt("entityID", mesh.entityID+1);
		if (mesh.animationToUse)
		{
			mesh.animationToUse->Update(mesh.animationToUse->GetCurrentTime(), glm::mat4(1.f), glm::mat4(1.f), mesh.meshToUse->GetBoneMap(), mesh.meshToUse->GetBoneInfo());
			mesh.meshToUse->DrawAnimation(shader, mesh.meshMaterial, mesh.animationToUse->GetBoneFinalMatrices());
		}
		else
		{
			mesh.meshToUse->PBRDraw(shader, mesh.meshMaterial);
		}

	}
}
void LightRenderer::InitializeLightRenderer() {
	for (int i{ 0 }; i < 16; i++) {
		dcm[i].InitializeMap();
	}
	testDCM.LoadDepthCubeMap("D:/CJJJ2/kOS/Kos Editor/Assets/DepthMap/test.dcm");

	LOGGING_INFO("Initialized shadow maps\n");
}
void LightRenderer::UpdateDCM() {
	for (size_t i = 0; i < pointLightsToDraw.size(); i++)
	{
		PointLightData& pointLight = pointLightsToDraw[i];
		if (pointLight.shadowCon) {
			dcm[i].FillMap(pointLight.position);
		}
	}
}
void LightRenderer::RenderAllLights(const CameraData& camera, Shader& shader)
{
	for (size_t i = 0; i < pointLightsToDraw.size(); i++)
	{
		PointLightData& pointLight = pointLightsToDraw[i];
		if (pointLight.shadowCon) {
			//FIll up with uniform data
		}
		pointLight.SetUniform(&shader, i);
		//pointLight.SetShaderMtrx(&shader, i);

	}

	for (size_t i = 0; i < directionLightsToDraw.size(); i++)
	{
		DirectionalLightData& directionLight = directionLightsToDraw[i];
		directionLight.SetShaderMtrx(&shader, i);
		directionLight.SetUniform(&shader, i);

	}

	for (size_t i = 0; i < spotLightsToDraw.size(); i++)
	{
		SpotLightData& spotLight = spotLightsToDraw[i];
		//spotLight.SetShaderMtrx(&shader, i);
		spotLight.SetUniform(&shader, i);
	}
	//std::cout << "D LIGHT SIZE" << directionLightsToDraw.size() << '\n';
}
void LightRenderer::DebugRender(const CameraData& camera, Shader& shader) {
	for (size_t i = 0; i < pointLightsToDraw.size(); i++)
	{
		PointLightData& pointLight = pointLightsToDraw[i];
		//Draw debug sphere

	}
}

void MeshRenderer::Clear()
{
	meshesToDraw.clear();
}

void SkinnedMeshRenderer::Clear()
{
	skinnedMeshesToDraw.clear();
}
void CubeRenderer::Render(const CameraData& camera, Shader& shader, Cube* cubePtr) {
	for (CubeData& cd : cubesToDraw) {
		shader.SetTrans("model", cd.transformation);
		shader.SetVec3("color", glm::vec3{ 1.f,1.f,1.f });

		shader.SetInt("entityID", cd.entityID + 1);
		glActiveTexture(GL_TEXTURE0); // activate proper texture unit before binding
		shader.SetInt("texture_diffuse1", 0);
		unsigned int currentTexture = 0;
		currentTexture = (cd.meshMaterial.albedo) ? cd.meshMaterial.albedo->RetrieveTexture() : 0;
		glBindTexture(GL_TEXTURE_2D, currentTexture);
		//Bind sepcular
		glActiveTexture(GL_TEXTURE1); // activate proper texture unit before binding
		shader.SetInt("texture_specular1", 1);
		currentTexture = (cd.meshMaterial.specular) ? cd.meshMaterial.specular->RetrieveTexture() : 0;
		glBindTexture(GL_TEXTURE_2D, currentTexture);
		//Bind normal
		glActiveTexture(GL_TEXTURE2); // activate proper texture unit before binding
		shader.SetInt("texture_normal1", 2);
		currentTexture = (cd.meshMaterial.normal) ? cd.meshMaterial.normal->RetrieveTexture() : 0;
		glBindTexture(GL_TEXTURE_2D, currentTexture);
		//Bind Metallic map
		glActiveTexture(GL_TEXTURE4); // activate proper texture unit before binding
		shader.SetInt("texture_ao1", 4);
		currentTexture = (cd.meshMaterial.ao) ? cd.meshMaterial.ao->RetrieveTexture() : 0;
		glBindTexture(GL_TEXTURE_2D, currentTexture);
		//Bind roughness
		glActiveTexture(GL_TEXTURE5); // activate proper texture unit before binding
		shader.SetInt("texture_roughness1", 5);
		currentTexture = (cd.meshMaterial.roughness) ? cd.meshMaterial.roughness->RetrieveTexture() : 0;
		glBindTexture(GL_TEXTURE_2D, currentTexture);
		//std::cout << "RENDERING MESH\n";
		glActiveTexture(GL_TEXTURE0);
		cubePtr->DrawMesh();
	}
}
void CubeRenderer::Clear() {
	cubesToDraw.clear();
}
void SpriteRenderer::InitializeSpriteRendererMeshes()
{
	screenSpriteMesh.CreateMesh();
}

void SpriteRenderer::RenderScreenSprites(const CameraData& camera, Shader& shader)
{
	for (const ScreenSpriteData& screenSprite : screenSpritesToDraw)
	{
		screenSpriteMesh.DrawMesh(screenSprite, shader, camera);
	}
}

void SpriteRenderer::Clear()
{
	screenSpritesToDraw.clear();
}

void LightRenderer::Clear()
{
	pointLightsToDraw.clear();
	spotLightsToDraw.clear();
	directionLightsToDraw.clear();
}

void DebugRenderer::InitializeDebugRendererMeshes() {
	debugCircle.CreateMesh();
	debugFrustum.CreateMesh();
	debugCircle.CreateMesh();
	debugCube.CreateMesh();
}
void DebugRenderer::Render(const CameraData& camera, Shader& shader) {

}
//Replace model with sphere positional data later
void DebugRenderer::RenderPointLightDebug(const CameraData& camera, Shader& shader, std::vector<PointLightData> pointLights) {
	for (size_t i = 0; i < pointLights.size(); i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 trY = glm::mat4(1.0f), trX = glm::mat4(1.0f);
		model = glm::translate(model, pointLights[i].position) * glm::scale(model, glm::vec3{ 1.f });
		trY = model * glm::rotate(trY, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		trX = model * glm::rotate(trX, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		float distance = glm::length(camera.position - pointLights[i].position);
		float l = glm::sqrt(glm::pow(distance, 2.f) - glm::pow(20.f, 2.f));
		float radiusPrime = (l * 1.f) / distance;
		float t = glm::sqrt(glm::pow(1.f, (float)2) -
			glm::pow(radiusPrime, (float)2));
		t = t / distance;

		glm::vec3 centerPrime{ pointLights[i].position + t * (camera.position - pointLights[i].position) };
		glm::mat4 trS = glm::mat4(1.0f);
		trS = glm::translate(trS, centerPrime) * glm::scale(trS, glm::vec3(radiusPrime, radiusPrime, radiusPrime)) * DebugCircle::RotateZtoV(camera.position - pointLights[i].position);

		shader.SetTrans("model", model);
		shader.SetFloat("uShaderType", 2.1f);
		debugCircle.DrawMesh();
		shader.SetTrans("model", trY);
		shader.SetFloat("uShaderType", 2.1f);
		debugCircle.DrawMesh();
		shader.SetTrans("model", trX);
		shader.SetFloat("uShaderType", 2.1f);
		debugCircle.DrawMesh();
		shader.SetTrans("model", trS);
		shader.SetFloat("uShaderType", 2.1f);
		debugCircle.DrawMesh();
	}

}
void DebugRenderer::RenderDebugFrustums(const CameraData& camera, Shader& shader, const std::vector<CameraData>& debugFrustums)
{
	shader.Use();
	for (const CameraData& cam : debugFrustums)
	{
		glm::mat4 proj = cam.GetPerspMtx();   // or OrthoMtx()
		glm::mat4 view = cam.GetViewMtx();
		glm::mat4 invVP = glm::inverse(proj * view);

		glm::vec3 ndc[8] =
		{
			{-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1}, // near
			{-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1}  // far
		};

		std::array<glm::vec3, 8> corners;
		for (int i = 0; i < 8; i++)
		{
			glm::vec4 w = invVP * glm::vec4(ndc[i], 1.0f);
			w /= w.w;
			corners[i] = glm::vec3(w);
		}
		shader.SetTrans("model", glm::mat4{ 1.f });
		shader.SetMat4("vp", camera.GetViewMtx());
		shader.SetFloat("uShaderType", 2.1f);

		glBindBuffer(GL_ARRAY_BUFFER, debugFrustum.vboId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * 8, corners.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		debugFrustum.DrawMesh();
	}
}

void DebugRenderer::RenderDebugCubes(const CameraData& camera, Shader& shader)
{
	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, { 0.f,0.f,0.f }) * glm::scale(model, glm::vec3(20.f, 20.f, 20.f));	// it's a bit too big for our scene, so scale it down
	//static float dt = 0.f;
	//dt += 0.01f;
	////Draw debug object to test
	//model = glm::mat4(1.0f);
	//glm::vec3 pos = glm::vec3(0.f, 0.f, 0.f);
	//glm::quat rot = glm::vec3(glm::radians(0.f), glm::radians(0.f), glm::radians(0.f));
	//glm::vec3 sca = glm::vec3(20.f, 20.f, 20.f);
	//model = glm::translate(model, pos) * glm::mat4_cast(rot) * glm::scale(model, sca);
	for (size_t i = 0; i < basicDebugCubes.size(); i++)
	{
		shader.SetFloat("uShaderType", 2.1f);
		shader.SetTrans("model", basicDebugCubes[i].worldTransform);
		shader.SetVec3("color", basicDebugCubes[i].color);
		debugCube.DrawMesh();
	}
}

void DebugRenderer::Clear() {
	basicDebugCubes.clear();
}