/********************************************************************/
/*!
\file      UIElements.h
\author    Sean Tiu (2303398)
\par       Emails: s.tiu@digipen.edu
\date      Oct 03, 2025
\brief     Declares data structures and mesh classes used for
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

#pragma once
#include"GraphicsReferences.h"
#include "Shader.h"
#include "Resources/ResourceHeader.h"
#include "Resources/R_Font.h"
#include "Resources/R_Model.h"
#include "Camera.h"
#include <vector>
#include <string>

//Foward Declaration
class R_Font;
class R_Texture;

struct UIElementData
{
	glm::vec3 position{};
	glm::vec2 scale{};
	float rotation{};
	glm::vec4 color{ 1.f };
};

struct ScreenTextData : UIElementData
{
	R_Font* fontToUse{nullptr};
	std::string textToDraw{};
	float fontSize{ 16.f };
	bool isCentered{ false };
};

struct ScreenSpriteData : UIElementData
{
	R_Texture* textureToUse{};
	unsigned int rows{ 1 }; //Used for sprite sheet stripping
	unsigned int columns{ 1 }; //Used for sprite sheet stripping
	unsigned int frameNumber{ 0 }; //Used for animation current frame
	unsigned int entityID;
};

struct ScreenGridData : UIElementData
{
	unsigned int rows{ 1 };
	unsigned int columns{ 1 };
	unsigned int height{ 0 };
};

struct UIMesh
{
	GLuint vaoID{};
	GLuint vboID{};
	GLint drawCount{};
	GLint primitiveType{};
	virtual void CreateMesh() = 0;
};

struct TextMesh : UIMesh
{
	void CreateMesh() override;
	void DrawMesh(const ScreenTextData& textData, Shader& shader, const CameraData& camera);
};

struct ScreenSpriteMesh : UIMesh
{
	void CreateMesh() override;
	void DrawMesh(const ScreenSpriteData& spriteData, Shader& shader, const CameraData& camera);
};

// Grid (Unimplemented)
struct GridMesh : UIMesh
{
	void CreateMesh() override;
	void DrawMesh(const ScreenGridData& gridData, Shader& shader, const CameraData& camera);
};

