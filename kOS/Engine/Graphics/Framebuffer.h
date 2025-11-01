/********************************************************************/
/*!
\file      Framebuffer.h
\author    Gabe Ng 2301290
\par       gabe.ng@digipen.edu
\date      Oct 03 2025
\brief     Creates a framebuffer to be used for different purposes
		   - Frame bufffer for general
		   - DEpth buffer for shadow
		   - Gbuffer for defferred rendering

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#pragma once
#include "Config/pch.h"
#include "GraphicsReferences.h"
#include "Shader.h"

struct FrameBuffer {
	GLuint fbo{}, texID{}, rbo{};
	unsigned int  vaoId{};
	Shader* shader{};
	GLint drawCount{};
	int width{}, height{};

	void InitializeFBO(int width, int height) {
		//Generate the FBO
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		//Generate the texture id
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
		//Generate RBO
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		float clearColor[4] = { 0.f, 0.f, 0.f, 0.f }; // black + fully transparent
		glClearBufferfv(GL_COLOR, 0, clearColor);
		//Check if FBO is generated properly
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//Created render

		float verticeSquare[]{
			-1.f, 1.f, 0.f, 1.f,
			-1.f, -1.f, 0.f, 0.f,
			1.f, 1.f, 1.f, 1.f,
			1.f, -1.f, 1.f, 0.f
		};

		short indices[]{
			0, 1, 2, 3
		};

		unsigned int VBO;
		unsigned int EBO;
		//Bind VBO
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenVertexArrays(1, &(this->vaoId));

		glBindVertexArray(this->vaoId);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticeSquare), verticeSquare, GL_STATIC_DRAW);
		//Bindn the element buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		//Bind the element array buffer
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		//glEnableVertexAttribArray(1);
		this->drawCount = 4;
		glBindVertexArray(0);

		this->height = height;
		this->width = width;
	}
	void Update(int width, int height) {
		//Bind texture data
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		this->height = height;
		this->width = width;
		//Rebind data
	}
	void Render() {
		shader->Use();
		glBindVertexArray(vaoId);
		shader->SetVec4("color2", { 1.f,1.f,1.f,1.f });

		//glActiveTexture(GL_TEXTURE0);
		//glUniform1i(glGetUniformLocation(shader->ID, "screenTexture"), 0);
		//texID = AssetManager::currentManager.GetTexture("CEO")->RetrieveTexture();
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, this->texID);
		glUniform1i(glGetUniformLocation(shader->ID, "screenTexture"), 7);
		glDrawElements(GL_TRIANGLE_STRIP, drawCount, GL_UNSIGNED_SHORT, NULL);
		glBindVertexArray(0);
	}
	void BindForDrawing()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
		glViewport(0, 0, static_cast<GLsizei>(this->width), static_cast<GLsizei>(this->height));
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
};

class GBuffer {

public:
	void InitializeGBuffer(int width, int height);
	void BindGBuffer();
	void UseGTextures();
	unsigned int RetrieveBuffer() { return gBuffer;;}
	void UseGBufferShader() { shader->Use();;}
	unsigned int gMaterial;
private:
	unsigned int gBuffer;
	unsigned int rboDepth;
	//Textures to generate
	unsigned int gPosition, gNormal, gAlbedoSpec,gReflect;
	Shader* shader;

};

class DepthBuffer {
public:
	void InitializeDepthBuffer(int width, int height);
	void BindDepthBuffer();
	unsigned int RetrieveBuffer() { return depthMap; }
	void UseGBufferShader() { shader->Use(); }
	unsigned int depthMapFBO;
private:
	unsigned int depthMap;
	Shader* shader;
};

class UIBuffer {
public:
	void InitializeUIBuffer(int width, int height);
	void InitializeUIBuffer(int width, int height,GLuint gBuffTtex);
	void BindForDrawing();
	unsigned int RetrieveBuffer() { return uiBuffer; }
	unsigned int gMaterial;
	GLuint texID{};
private:
	int width{}, height{};
	unsigned int uiBuffer;
	unsigned int rboDepth;
};