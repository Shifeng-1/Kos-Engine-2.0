R"(
#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gReflect;
layout (location = 4) out vec4 gMaterial;

in vec2 texCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 ReflectDir;
in mat3 tangentToWorld;
uniform float uShaderType;


uniform int entityID=-1;
uniform sampler2D sprite;
uniform vec4 color;

void main()
{    
    gAlbedoSpec =texture(sprite, texCoords) * color;
    gMaterial.a=entityID;
    gMaterial.b=uShaderType;
}
)"