/*
 FILENAME: GBuffPBRShader.vs
 AUTHOR(S): Gabe (100%)
 @version 460 core
 All content � 2025 DigiPen Institute of Technology Singapore. All
 rights reserved.
 */

R"(
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBinormal;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4  aWeights;
    
out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 ReflectDir;
out mat3 tangentToWorld;
out float shaderType;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPosition;
uniform int isNotRigged;

const int MAX_BONES = 200;
uniform mat4 bones[MAX_BONES];

//Debug con
uniform float uShaderType;

void main()
{

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoords;  
    shaderType=uShaderType;
}
)"