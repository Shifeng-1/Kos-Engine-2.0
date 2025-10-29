R"(

#version 460 core

//World space transformations
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBinormal;


out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 ReflectDir;
out mat3 tangentToWorld;



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    ReflectDir=reflect( normalize(vec3(0.f,0.f,5.f) - aPos), normalize(aNormal));
    mat4 MV = view * model; // Model-View transform matrix

    mat3 N = mat3(vec3(MV[0]), vec3(MV[1]), vec3(MV[2])); // Normal transform matrix

    Normal = normalize(N*aNormal);

    vec3 tang = normalize(vec3(aTangent));
    vec3 binormal = normalize(vec3(aBinormal));

    tangentToWorld = mat3(  tang.x, binormal.x, Normal.x,
                                tang.y, binormal.y, Normal.y,
                                tang.z, binormal.z, Normal.z  );    

    
    vec4 VertexPositionInView = MV * vec4(aPos, 1.0f);
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * VertexPositionInView; 

    TexCoords = aTexCoords;  
} 
)"