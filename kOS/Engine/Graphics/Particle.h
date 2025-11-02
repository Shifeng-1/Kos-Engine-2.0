#pragma once

class BasicParticleData
{
public:
    std::vector<glm::vec3> particlePositions;
    glm::vec4 color{ 1.f };
    glm::vec2 scale{ 1.f };
    float rotate{};
};

class BasicParticleInstance
{
public:
    glm::vec3 position{ 1.f };
    glm::vec2 scale{ 1.f };
    glm::vec4 color{ 1.f };
    float rotation;
};

class BasicParticleMesh
{
public:
    unsigned int vaoid{};
    unsigned int vboid{};
};