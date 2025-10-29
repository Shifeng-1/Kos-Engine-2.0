R"(
#version 460 core
out vec4 FragColor;  
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gReflect;
layout (location = 4) out vec4 gMaterial;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 ReflectDir;
in mat3 tangentToWorld;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_ao1;
uniform sampler2D texture_roughness1;

uniform mat4 view;

const float PI = 3.14159265358979323846;
vec4 albedo;
float metallic;
float ggxDistribution(float nDotH,float roughness) 
{
    float alpha2 = roughness * roughness * roughness * roughness;
    float d = (nDotH * nDotH) * (alpha2 - 1.0f) + 1.0f;
    return alpha2 / (PI * d * d);
}
float geomSmith(float nDotL,float roughness) 
{
    float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
    float denom = nDotL * (1.0f - k) + k;
    return 1.0f / denom;
}
vec3 schlickFresnel(float lDotH) 
{
    vec3 f0 = vec3(0.04f); // Dielectrics
    f0 = mix(f0, vec3(albedo.xyz), metallic);
    return f0 + (1.0f - f0) * pow(1.0f - lDotH, 5);
}

vec3 microFacetDirection(vec3 position, vec3 n,vec3 color,float roughness) 
{  


    vec3 lightI = vec3(1.f,1.f,1.f);

    vec3 lightPosWorld = vec3(0.f, 0.f, 0.f);
    
    // Transform to view space
    vec3 lightPosView = vec3(view * vec4(lightPosWorld, 1.0f));
    
    // Light direction: FROM surface TO light (in view space)
    vec3 l = normalize(lightPosView - position);

    vec3 v = normalize(-position);
    vec3 h = normalize(v + l);
    float nDotH = dot(n, h);
    float lDotH = dot(l, h);
    float nDotL = max(dot(n, l), 0.0f);
    float nDotV = dot(n, v);

         vec3 F    = schlickFresnel(lDotH);
           
        //vec3 numerator    = ggxDistribution(nDotH,roughness) * geomSmith(nDotL,roughness) *geomSmith(nDotV,roughness)* F; 
        //float denominator = 4.0 * nDotV * nDotL; // + 0.0001 to prevent divide by zero
        //vec3 specular = numerator / denominator;
        vec3 specBrdf = 0.25f * ggxDistribution(nDotH,roughness) * F 
                            * geomSmith(nDotL,roughness) * geomSmith(nDotV,roughness);
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  
        //kD *= directionalLight[i].Ld;
        // scale light by NdotL

    return ((kD * vec3(albedo.xyz) / PI + specBrdf) * lightI * nDotL);
}

void main()
{
    
    vec4 albedo=vec4(texture(texture_diffuse1, TexCoords).rgb,1.f);
    vec3 normalMap = texture(texture_normal1, TexCoords).rgb * 2.0 - 1.0;        // unpack normal map
    vec3 worldNormal = normalize(tangentToWorld * normalMap);   
    vec3 viewNormal = normalize(mat3(view) * worldNormal);

    metallic = texture(texture_specular1, TexCoords).r;
    float ao=texture(texture_ao1, TexCoords).g;
    float roughness=texture(texture_roughness1, TexCoords).r;

    vec3 viewPosition=vec3(view*vec4(FragPos.xyz,1.f));
    ao=ao==0.f?1.f:ao;
    vec3 newLight=vec3(albedo)*ao;

    newLight+=microFacetDirection(viewPosition, viewNormal ,vec3(albedo),roughness);
    vec3 color = newLight/ (newLight + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 
    FragColor =vec4(color,1.0);  
}
)"