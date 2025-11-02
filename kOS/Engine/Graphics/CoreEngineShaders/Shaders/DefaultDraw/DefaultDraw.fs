R"(
#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

layout(location=0)  uniform sampler2D gPosition;
layout(location=1)  uniform sampler2D gNormal;
layout(location=2)  uniform sampler2D gAlbedoSpec;
layout(location=3)  uniform sampler2D gReflect;  // Tangent-space light direction
layout(binding=4)   uniform sampler2D gMaterial;

layout(binding=5)   uniform samplerCube cubeTexture;
layout(binding=6)   uniform sampler2D shadowMap;

void main()
{   
        //Albedo value
    // Sample from gMaterial
    vec3 newMat = texture(gMaterial, TexCoords).rgb;
    vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
    
    // Example: take the blue channel
    int val = int(newMat.b)%2;
    int outputVal = (val == 0) ? 1 : 0;

    // Cast back to float for FragColor
    float outFloat = float(outputVal);

    FragColor = vec4(color.r,color.g,color.b, newMat.b);
}
)"