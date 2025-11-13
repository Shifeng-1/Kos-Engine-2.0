R"(
#version 460 core
layout (location = 0) in vec3 vertex;  // usually a quad: (-0.5, -0.5, 0) to (0.5, 0.5, 0)
layout (location = 1) in vec2 texCO;

out vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 point;   // (x, y, z)
uniform vec2 scale;
uniform float rotation;  // rotation around camera-facing axis

void main()
{
    // Pass through texture coordinates
    texCoords = texCO;
    
    float cosRot = cos(rotation);
    float sinRot = sin(rotation);
    mat2 rotMat = mat2(cosRot, -sinRot, sinRot, cosRot);
    vec2 rotatedXY = rotMat * vertex.xy;
    
    vec3 scaledVertex = vec3(rotatedXY * scale, vertex.z);
    
    vec3 worldPos = point + scaledVertex;
    
    gl_Position = projection * view * vec4(worldPos, 1.0);
}
)"