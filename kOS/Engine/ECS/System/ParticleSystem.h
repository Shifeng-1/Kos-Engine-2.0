#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "ECS/ECS.h"
#include "System.h"
#include "Flex/NvFlex.h"
#include "Flex/NvFlexExt.h"

namespace ecs {

    struct ParticleInstance {
        std::vector<glm::vec3> positions_Particle;
        std::vector<glm::vec4> colors;
        std::vector<glm::vec2> sizes;
        std::vector<float>     rotates;
    };
    struct EmissionData {
        glm::vec3 positionOffset;  // Offset from emitter position
        glm::vec3 direction;       // Normalized direction
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
        // SHAPES FUNCTION
        //===========================================
        EmissionData GenerateBoxEmission(ParticleComponent* particle);
        EmissionData GenerateConeEmission(ParticleComponent* particle);
        EmissionData GenerateSphereEmission(ParticleComponent* particle);
        EmissionData GenerateCircleEmission(ParticleComponent* particle);
        EmissionData GenerateEdgeEmission(ParticleComponent* particle);

        // Apply random chaos to direction
        glm::vec3 ApplyRandomDirection(const glm::vec3& direction, float randomAmount);


        //===========================================
        // HELPER FUNCTIONS
        //===========================================

        void* getVoid(ParticleComponent* particle, STATE state);
       
        inline float RandomRange(float minValue, float maxValue) {
            static std::mt19937 generator(std::random_device{}());      // create once
            std::uniform_real_distribution<float> distribution(glm::min(minValue, maxValue), glm::max(minValue, maxValue));
            return distribution(generator);
        }

        inline float AbsRandomRange(float minValue, float maxValue){
            static std::mt19937 generator(std::random_device{}());      // create once
            std::uniform_real_distribution<float> distribution(glm::min(glm::abs(minValue),glm::abs(maxValue)),glm::max(glm::abs(minValue), glm::abs(maxValue)));
            return distribution(generator);
        }

        inline glm::vec4 RandomColourRange(glm::vec4 color_Start, glm::vec4 color_End) {
            glm::vec4 ret;
            ret.r = AbsRandomRange(color_Start.r, color_End.r);
            ret.g = AbsRandomRange(color_Start.g, color_End.g);
            ret.b = AbsRandomRange(color_Start.b, color_End.b);
            ret.a = 1.f;
            return ret;
        }

        REFLECTABLE(ParticleSystem);
    };
}
#endif