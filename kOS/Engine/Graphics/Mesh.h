/********************************************************************/
/*!
\file      Mesh.cpp
\author    Gabe Ng 2301290
\par       gabe.ng@digipen.edu
\date      Oct 03 2025
\brief     Holds basic meshes for different default mesh types to be used in the program
		   Basic shapes contain
		   - Cube
		   - Sphere

		   Debug shapes contain
		   - Debug cube
		   - Debug circle

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#pragma once
#include"GraphicsReferences.h"
#include <vector>
#define PI 3.1415
struct BasicMesh {
	GLenum primitiveType{ 0 };
	GLuint vaoId{ 0 };
	GLint drawCount{ 0 };

	//To be overwrittern, mark as deleted
	virtual void CreateMesh()=0;
	virtual void DrawMesh()=0;
};

struct Cube :BasicMesh {
	void CreateMesh();
	void DrawMesh();
};

struct Sphere :BasicMesh {
	int sectorCount{ 36 }, stackCount{ 18 };
	float radius{1.f};
	void CreateMesh();
	void DrawMesh();
};

struct BasicDebugData
{
	void ConstructMat4(glm::vec3 center, glm::vec3 size);
	glm::mat4 worldTransform;
	glm::vec3 color{ 0.f,1.f,0.f };
};

struct DebugCube :BasicMesh {
	float lineWidth{ 2.0 };
	void CreateMesh();
	void DrawMesh();
};
struct DebugCircle :BasicMesh {
	float radius{ 1.f };
	void CreateMesh();
	void DrawMesh();
	static glm::mat4 RotateZtoV(glm::vec3 V) {
		glm::vec3 U = glm::cross(V, glm::vec3{ 0, 0, 1 });
		//if (glm::dot(U, U) < 1e-6f) return glm::mat4(1.f);

		glm::vec3 C = glm::normalize(V);
		glm::vec3 A = glm::normalize(glm::cross(C, U));
		glm::vec3 B = glm::cross(A, C);

		return glm::mat4{
		glm::vec4{A, 0.f},
		glm::vec4{B, 0.f},
		glm::vec4{C, 0.f},
		glm::vec4{0, 0, 0, 1} };
	}
};

struct DebugFrustum : BasicMesh {
	float lineWidth{ 2.0f };
	GLuint vboId;
	void CreateMesh();
	void DrawMesh();
};