R"(
#version 460 core
in vec2 texCoords;
out vec4 FragColor;
layout (location = 1) out vec4 gMaterial;

uniform sampler2D sprite;
uniform vec4 color;
uniform int entityID;
void main()
{
    gMaterial.a=entityID;
    FragColor = texture(sprite, texCoords) * color;
}
)"