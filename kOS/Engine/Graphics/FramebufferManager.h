/********************************************************************/
/*!
\file      FramebufferManager.h
\author    Sean Tiu (2303398)
\par       Emails: s.tiu@digipen.edu
\date      Oct 03, 2025
\brief     Defines the FramebufferManager class, which handles the
		   initialization and composition of various framebuffers
		   used in the rendering pipeline.

		   The FramebufferManager is responsible for creating and
		   managing different framebuffer objects, including:
		   - Scene, Game, Editor, and UI buffers
		   - Depth and G-buffers

		   It also provides functionality to compose (blend)
		   multiple buffers together using shaders for final output
		   rendering.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Framebuffer.h"
#include "Shader.h"

class FramebufferManager
{
public:

	FrameBuffer frameBuffer{};
	DepthBuffer depthBuffer{};
	FrameBuffer sceneBuffer{};
	FrameBuffer gameBuffer{};
	FrameBuffer editorBuffer{};
	UIBuffer UIBuffer{};

	GBuffer gBuffer{};

	void Initialize(unsigned int windowWidth, unsigned int windowHeight)
	{
		sceneBuffer.InitializeFBO(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
		frameBuffer.InitializeFBO(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
		editorBuffer.InitializeFBO(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
		gameBuffer.InitializeFBO(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
		depthBuffer.InitializeDepthBuffer(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
		gBuffer.InitializeGBuffer(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
		UIBuffer.InitializeUIBuffer(static_cast<int>(windowWidth), static_cast<int>(windowHeight),gBuffer.gMaterial);
	}

	void ComposeBuffers(GLuint lowerBufferTexID, GLuint upperBufferTexID, FrameBuffer& outputBuffer, Shader& shader)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, outputBuffer.fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Draw a fullscreen quad with both textures
		shader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lowerBufferTexID);
		shader.SetInt("lowerBuffer", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, upperBufferTexID);
		shader.SetInt("upperBuffer", 1);

		glBindVertexArray(outputBuffer.vaoId);
		glDrawElements(GL_TRIANGLE_STRIP, outputBuffer.drawCount, GL_UNSIGNED_SHORT, NULL);
		glBindVertexArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		shader.Disuse();
	}

private:

};