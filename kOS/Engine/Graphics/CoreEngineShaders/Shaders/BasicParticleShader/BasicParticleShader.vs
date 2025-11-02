R"(
#version 460 core

layout(location = 0) in vec3 inPos;       // base quad vertex (-0.5..0.5)
layout(location = 2) in vec3 instancePos; // per-particle center
layout(location = 3) in vec2 instanceScale;
layout(location = 4) in vec4 instanceColor;
layout(location = 5) in float instanceRot; // per-particle rotation (around view axis)

out vec4 vColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Extract camera right and up vectors from view matrix
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp    = vec3(view[0][1], view[1][1], view[2][1]);

    // Local rotation (around Z/view axis)
    float c = cos(instanceRot);
    float s = sin(instanceRot);
    mat2 rot = mat2(c, -s, s, c);

    // Apply local rotation and scale to quad vertex
    vec2 rotated = rot * (inPos.xy * instanceScale);

    // Build the final world position from the camera basis
    vec3 worldPos = instancePos 
                  + cameraRight * rotated.x
                  + cameraUp * rotated.y;

    gl_Position = projection * view * vec4(worldPos, 1.0);
    vColor = instanceColor;
}
)"