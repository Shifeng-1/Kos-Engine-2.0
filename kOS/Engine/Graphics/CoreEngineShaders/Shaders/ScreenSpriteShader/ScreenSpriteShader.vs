R"(
#version 460 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texCO;

out vec2 texCoords;

uniform mat3 projection;
uniform vec3 point; // (x, y, z)
uniform vec2 scale;
uniform float rotation;

void main()
{
    vec3 pos = vec3(vertex.xy, 1.0);
    pos.x *= scale.x;
    pos.y *= scale.y;
    float c = cos(rotation);
    float s = sin(rotation);
    pos.xy = vec2(
        pos.x * c - pos.y * s,
        pos.x * s + pos.y * c
    );
    pos.xy += point.xy;  // move quad
    gl_Position = vec4(projection * pos, 1.0);
    gl_Position.z = vertex.z;
    texCoords = texCO;
}
)"