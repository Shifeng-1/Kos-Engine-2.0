/********************************************************************/
/*!
\file      Material.cpp
\author    Gabe Ng 2301290
\par       gabe.ng@digipen.edu
\date      Oct 03 2025
\brief     Contains the material type to be used for PBR

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/
#include "Config/pch.h"
#include "Mesh.h"

void BasicDebugData::ConstructMat4(glm::vec3 center, glm::vec3 size) {
    glm::mat4 model{ 1.f };
    model = glm::translate(model, center) * glm::scale(model, size);
    worldTransform = model;
}
void Cube::CreateMesh() {

    float verticeCube[] = {
        // Back face (looking at -Z, CCW from outside)
        -0.5f, -0.5f, -0.5f,   0, 0, -1,   0,0,   1,0,0,   0,1,0,
        -0.5f,  0.5f, -0.5f,   0, 0, -1,   0,1,   1,0,0,   0,1,0,
         0.5f,  0.5f, -0.5f,   0, 0, -1,   1,1,   1,0,0,   0,1,0,
         0.5f,  0.5f, -0.5f,   0, 0, -1,   1,1,   1,0,0,   0,1,0,
         0.5f, -0.5f, -0.5f,   0, 0, -1,   1,0,   1,0,0,   0,1,0,
        -0.5f, -0.5f, -0.5f,   0, 0, -1,   0,0,   1,0,0,   0,1,0,

        // Front face (looking at +Z, CCW from outside)
        -0.5f, -0.5f,  0.5f,   0, 0,  1,   0,0,   1,0,0,   0,-1,0,
         0.5f, -0.5f,  0.5f,   0, 0,  1,   1,0,   1,0,0,   0,-1,0,
         0.5f,  0.5f,  0.5f,   0, 0,  1,   1,1,   1,0,0,   0,-1,0,
         0.5f,  0.5f,  0.5f,   0, 0,  1,   1,1,   1,0,0,   0,-1,0,
        -0.5f,  0.5f,  0.5f,   0, 0,  1,   0,1,   1,0,0,   0,-1,0,
        -0.5f, -0.5f,  0.5f,   0, 0,  1,   0,0,   1,0,0,   0,-1,0,

        // Left face (looking at -X, CCW from outside)
        -0.5f, -0.5f, -0.5f,  -1, 0,  0,   0,0,   0,0,-1,   0,1,0,
        -0.5f, -0.5f,  0.5f,  -1, 0,  0,   1,0,   0,0,-1,   0,1,0,
        -0.5f,  0.5f,  0.5f,  -1, 0,  0,   1,1,   0,0,-1,   0,1,0,
        -0.5f,  0.5f,  0.5f,  -1, 0,  0,   1,1,   0,0,-1,   0,1,0,
        -0.5f,  0.5f, -0.5f,  -1, 0,  0,   0,1,   0,0,-1,   0,1,0,
        -0.5f, -0.5f, -0.5f,  -1, 0,  0,   0,0,   0,0,-1,   0,1,0,

        // Right face (looking at +X, CCW from outside)
         0.5f, -0.5f, -0.5f,   1, 0,  0,   0,0,   0,0,1,    0,1,0,
         0.5f,  0.5f, -0.5f,   1, 0,  0,   0,1,   0,0,1,    0,1,0,
         0.5f,  0.5f,  0.5f,   1, 0,  0,   1,1,   0,0,1,    0,1,0,
         0.5f,  0.5f,  0.5f,   1, 0,  0,   1,1,   0,0,1,    0,1,0,
         0.5f, -0.5f,  0.5f,   1, 0,  0,   1,0,   0,0,1,    0,1,0,
         0.5f, -0.5f, -0.5f,   1, 0,  0,   0,0,   0,0,1,    0,1,0,

         // Bottom face (looking at -Y, CCW from outside)
         -0.5f, -0.5f, -0.5f,   0, -1, 0,   0,1,   1,0,0,    0,0,1,
          0.5f, -0.5f, -0.5f,   0, -1, 0,   1,1,   1,0,0,    0,0,1,
          0.5f, -0.5f,  0.5f,   0, -1, 0,   1,0,   1,0,0,    0,0,1,
          0.5f, -0.5f,  0.5f,   0, -1, 0,   1,0,   1,0,0,    0,0,1,
         -0.5f, -0.5f,  0.5f,   0, -1, 0,   0,0,   1,0,0,    0,0,1,
         -0.5f, -0.5f, -0.5f,   0, -1, 0,   0,1,   1,0,0,    0,0,1,

         // Top face (looking at +Y, CCW from outside)
         -0.5f,  0.5f, -0.5f,   0,  1, 0,   0,1,   1,0,0,    0,0,-1,
         -0.5f,  0.5f,  0.5f,   0,  1, 0,   0,0,   1,0,0,    0,0,-1,
          0.5f,  0.5f,  0.5f,   0,  1, 0,   1,0,   1,0,0,    0,0,-1,
          0.5f,  0.5f,  0.5f,   0,  1, 0,   1,0,   1,0,0,    0,0,-1,
          0.5f,  0.5f, -0.5f,   0,  1, 0,   1,1,   1,0,0,    0,0,-1,
         -0.5f,  0.5f, -0.5f,   0,  1, 0,   0,1,   1,0,0,    0,0,-1
    };
    //Set up VBO
    GLuint VBO;
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &VBO);

    // Bind VAO first
    glBindVertexArray(vaoId);

    // Bind and upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticeCube), verticeCube, GL_STATIC_DRAW);

    // Set vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14* sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 *sizeof(float)));
    glEnableVertexAttribArray(2);

    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8* sizeof(float)));
    glEnableVertexAttribArray(3);

    // Binormal
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11* sizeof(float)));
    glEnableVertexAttribArray(4);

    // Unbind VAO
    glBindVertexArray(0);

    primitiveType = GL_TRIANGLES;
}

void Cube::DrawMesh()
{
	//std::cout << "DRAWOING SQUARE \n";
	//Draw element
    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, 36);//glDepthMask(GL_TRUE);
    glBindVertexArray(0);

}

void Sphere::CreateMesh() {

    //Referenced from  https://www.songho.ca/opengl/gl_sphere.html
    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;
    std::vector <float>vertexData;

    for (int i{ 0 }; i <= stackCount; i++) {
        stackAngle = PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertexData.push_back(x);
            vertexData.push_back(y);
            vertexData.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            vertexData.push_back(nx);
            vertexData.push_back(ny);
            vertexData.push_back(nz);

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            vertexData.push_back(s);
            vertexData.push_back(t);

            float tx = -xy * sinf(sectorAngle);     // -r * cos(u) * sin(v)
            float ty = xy * cosf(sectorAngle);      // r * cos(u) * cos(v)
            float tz = 0.0f;

            // normalize tangent
            float tangentLength = sqrtf(tx * tx + ty * ty + tz * tz);
            if (tangentLength > 0.0f) {
                tx /= tangentLength;
                ty /= tangentLength;
                tz /= tangentLength;
            }

            vertexData.push_back(tx);
            vertexData.push_back(ty);
            vertexData.push_back(tz);

            // binormal (bitangent) vector (follows latitude direction)
            // binormal = d(position)/d(stackAngle)
            float bx = -radius * sinf(stackAngle) * cosf(sectorAngle);  // -r * sin(u) * cos(v)
            float by = -radius * sinf(stackAngle) * sinf(sectorAngle);  // -r * sin(u) * sin(v)
            float bz = radius * cosf(stackAngle);                       // r * cos(u)

            // normalize binormal
            float binormalLength = sqrtf(bx * bx + by * by + bz * bz);
            if (binormalLength > 0.0f) {
                bx /= binormalLength;
                by /= binormalLength;
                bz /= binormalLength;
            }

            vertexData.push_back(bx);
            vertexData.push_back(by);
            vertexData.push_back(bz);
        }
    }

    //Create indices
    std::vector<short> indices;
    int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }


    //Set up VAO and VBO
        //Set up VBO
    GLuint VBO;
    GLuint EBO;
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VAO first
    glBindVertexArray(vaoId);

    // Bind and upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // Binormal
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    //Set up EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(short), indices.data(), GL_STATIC_DRAW);

    // Unbind VAO
    glBindVertexArray(0);

    primitiveType = GL_TRIANGLES;
    drawCount = indices.size();
    //std::cout << "CREATED SPHERE\n";
}

void Sphere::DrawMesh()
{
    //std::cout << "DRAWOING SQUARE \n";
    //Draw element
    glDisable(GL_CULL_FACE);
    glBindVertexArray(vaoId);
    glDrawElements(primitiveType, drawCount, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);

}
void DebugCube::CreateMesh() {
    std::vector<glm::vec3> positionList;
    float verticeCube[] = {
        -0.5f,-0.5f,-0.5f, //Bottom left
        0.5f,-0.5f,-0.5f, //Bottom right
        -0.5f,0.5f,-0.5f, //top left
        0.5f,0.5f,-0.5f, //top right

        -0.5f,-0.5f,0.5f, //Bottom left
        0.5f,-0.5f, 0.5f, //Bottom right
        -0.5f,0.5f, 0.5f, //top left
        0.5f,0.5f,  0.5, //top left
    };
    GLushort indices[] = {
        // Back face edges
        0, 1,   2, 3,   0, 2,   1, 3,
        // Front face edges  
        4, 5,   5, 7,   7, 6,   6, 4,
        // Connecting edges (back to front)
        0, 4,   1, 5,   2, 6,   3, 7
    };

    GLuint VBO;
    GLuint EBO;
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // Bind VAO first
    glBindVertexArray(vaoId);

    // Bind and upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticeCube), verticeCube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // Unbind VAO
    glBindVertexArray(0);

    primitiveType = GL_LINES;
    drawCount = 24;
}

void DebugCube::DrawMesh()
{
    //std::cout << "DRAWOING SQUARE \n";
    //Draw element
    glLineWidth(lineWidth);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBindVertexArray(vaoId);
    glDrawElements(primitiveType, drawCount, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);

}

void DebugCircle::CreateMesh() {

    //Create sphere VAO
    std::vector<glm::vec3> sphereVertexList;
    float angleIncrement = static_cast<float>(2.0f * PI / 48); // Angle between each vertex
    for (int i = 0; i < 48; ++i) {
        float theta = i * angleIncrement;
        sphereVertexList.push_back(glm::vec3{ cos(theta), sin(theta), 0.f});  // x
    }
    //Create indices
    std::vector<short> sphereIndexList;
    for (int i{ 0 }; i < 48; i++) {
        sphereIndexList.push_back(i);
    }

    GLuint VBO,EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);



    // Bind and upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * sphereVertexList.size(),
        &sphereVertexList[0][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  // Added stride

    // Bind and upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndexList.size() * sizeof(short),
        sphereIndexList.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

}
void DebugCircle::DrawMesh(){
    glBindVertexArray(vaoId);
    glDrawElements(GL_LINE_LOOP, 48, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

void DebugFrustum::CreateMesh()
{
    if (vaoId != 0) return; // only once

    glm::vec3 ndcCorners[8] = {
        {-1, -1, -1}, { 1, -1, -1},
        { 1,  1, -1}, {-1,  1, -1},
        {-1, -1,  1}, { 1, -1,  1},
        { 1,  1,  1}, {-1,  1,  1}
    };

    unsigned int indices[] = {
        // Near plane
        0,1, 1,2, 2,3, 3,0,
        // Far plane
        4,5, 5,6, 6,7, 7,4,
        // Connect near <-> far
        0,4, 1,5, 2,6, 3,7
    };

    drawCount = sizeof(indices) / sizeof(unsigned int);

    unsigned int ebo;
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ndcCorners), ndcCorners, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

void DebugFrustum::DrawMesh()
{
    glBindVertexArray(vaoId);
    glDrawElements(GL_LINES, drawCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
