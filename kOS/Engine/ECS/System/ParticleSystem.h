#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "ECS/ECS.h"
#include "System.h"
#include "Flex/NvFlex.h"
#include "Flex/NvFlexExt.h"

namespace ecs {

    struct Particle {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec4> colors;
        std::vector<glm::vec3> scales;
        std::vector<float>     rotations;
    };

    struct ParticleInstance {
        std::vector<glm::vec3> positions_Particle;
        //new
        std::vector<glm::vec4> colors;
        std::vector<glm::vec3> scales;
        std::vector<float>     rotations;

        //old
        glm::vec4 color{ 1.f,1.f,1.f,1.f };
        glm::vec3 scale{ 1.f,1.f,1.f };
        float rotate{};
    };

    enum STATE {
        POSITION,
        VELOCITY,
        PHASE,
        ACTIVE,
        LIFESPAN,
        counter  
    };


    class ParticleSystem : public ISystem {
    public:
        using ISystem::ISystem;
        void Init() override;
        void Update() override;
       
       

        // Spawn a new particle
        void EmitParticle(EntityID entityId, const glm::vec3& particle_position,
            const glm::vec3& velocity, float lifetime, ParticleComponent*& particle, glm::vec4* position, glm::vec3* velocities, float* lifetime_list, float* lifetime_Counter_list);
        
        // Update particle lifetimes and kill dead particles
        void UpdateParticleLifetimes(float dt, ParticleComponent*& particle, glm::vec4* positions, glm::vec3* velocities, float* lifetime_list, float* lifetime_Counter_list);
        
        // Handle particle emission from emitter components
        void UpdateEmitters(float dt, EntityID id, ParticleComponent*& particleComp,  TransformComponent* transform, glm::vec4* position, glm::vec3* velocities, float* lifetime_list, float* lifetime_Counter_list);

        void SyncActiveBuffer(ParticleComponent* particle);

        void ExtractParticleDataOptimized(ParticleComponent* particle, ParticleInstance& data, glm::vec4* positions);


        //===========================================
        // HELPER FUNCTIONS
        //===========================================

        void* getVoid(ParticleComponent* particle, STATE state);

        inline float RandomRange(float minValue, float maxValue){
            static std::mt19937 generator(std::random_device{}());      // create once
            std::uniform_real_distribution<float> distribution(std::min(minValue,maxValue),std::max(minValue, maxValue));
            return distribution(generator);
        }

        inline glm::vec4 RandomColourRange(glm::vec4 color_Start, glm::vec4 color_End) {
            glm::vec4 ret;
            ret.r = RandomRange(color_Start.r, color_End.r);
            ret.g = RandomRange(color_Start.g, color_End.g);
            ret.b = RandomRange(color_Start.b, color_End.b);
            ret.a = 1.f;
            return ret;
        }

        REFLECTABLE(ParticleSystem);
    };
}
#endif