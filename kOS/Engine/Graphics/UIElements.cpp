/********************************************************************/
/*!
\file      UIElements.cpp
\author    Sean Tiu (2303398)
\par       Emails: s.tiu@digipen.edu
\date      Oct 03, 2025
\brief     Defines data structures and mesh classes used for
           rendering 2D UI elements such as text, sprites, and grids
           within the engine.

           This file defines several key components of the UI system:
           - **UIElementData**: Base data shared by all UI elements,
             including position, scale, rotation, and color.
           - **ScreenTextData**: Holds information for rendering
             on-screen text, such as font, string content, and
             alignment.
           - **ScreenSpriteData**: Represents 2D sprites with
             optional sprite-sheet animation support.
           - **ScreenGridData**: Defines a grid structure (currently
             unimplemented).

           It also declares mesh structures derived from `UIMesh`
           used for rendering:
           - **TextMesh**: Builds and draws text glyphs.
           - **ScreenSpriteMesh**: Renders 2D sprites and icons.
           - **GridMesh**: Reserved for rendering UI grids.

           These components work with shaders and camera data to draw
           2D user interface elements efficiently in the rendering
           pipeline.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "UIElements.h"
#include "GraphicsManager.h"

void TextMesh::CreateMesh()
{
	glGenVertexArrays(1, &this->vaoID);
	glGenBuffers(1, &this->vboID);

	glBindVertexArray(this->vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboID);

	constexpr int numberOfVertexes = 6;
	constexpr int numberOfWorldCoordinates = 2;
	constexpr int numberOfUVCoordinates = 2;
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numberOfVertexes * (numberOfWorldCoordinates + numberOfUVCoordinates), NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, (numberOfWorldCoordinates + numberOfUVCoordinates), GL_FLOAT, GL_FALSE, (numberOfWorldCoordinates + numberOfUVCoordinates) * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextMesh::DrawMesh(const ScreenTextData& textData, Shader& shader, const CameraData& camera)
{
	//Unimplemented for now
}

void ScreenSpriteMesh::CreateMesh()
{
    float vertices[] = {
        // pos       // tex
        -0.5f, -0.5f,  0.0f, 0.0f, // bottom left
         0.5f, -0.5f,  1.0f, 0.0f, // bottom right
         0.5f,  0.5f,  1.0f, 1.0f, // top right

         0.5f,  0.5f,  1.0f, 1.0f, // top right
        -0.5f,  0.5f,  0.0f, 1.0f, // top left
        -0.5f, -0.5f,  0.0f, 0.0f  // bottom left
    };

    GLuint vbo;
    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vaoID);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribute 0 = vec4 (x, y, u, v)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                // location = 0 in shader
        4,                // 4 components: x, y, u, v
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),// stride
        (void*)0          // offset
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ScreenSpriteMesh::DrawMesh(const ScreenSpriteData& spriteData, Shader& shader, const CameraData& camera)
{
    shader.Use();

    constexpr float radianConversion = 3.14159f / 180.f;
    float angle = spriteData.rotation * radianConversion;

    shader.SetMat3("projection", camera.GetUIOrthoMtx());
    shader.SetVec3("point", spriteData.position);
    shader.SetVec2("scale", spriteData.scale);
    shader.SetFloat("rotation", angle);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spriteData.textureToUse->RetrieveTexture());
    shader.SetInt("sprite", 0);
    shader.SetVec4("color", spriteData.color);
    shader.SetInt("entityID", spriteData.entityID+1);
    glBindVertexArray(this->vaoID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        //LOGGING_ERROR("First OpenGL Error: 0x%X", err);h
        std::cout << "after OpenGL Error: " << err << std::endl;
    }
}

void GridMesh::CreateMesh()
{
    //Unimplemented for now
}

void GridMesh::DrawMesh(const ScreenGridData& gridData, Shader& shader, const CameraData& camera)
{
    //Unimplemeted for now
}
