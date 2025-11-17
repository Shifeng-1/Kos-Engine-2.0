#include "Config/pch.h"
#include "ParticleSystem.h"
#include "glm/glm.hpp"
#include "Graphics/GraphicsManager.h"
#include "Inputs/Input.h"

namespace ecs {

    void ParticleSystem::Init() {

        onRegister.Add([&](EntityID id) {
            ParticleComponent* particle = m_ecs.GetComponent<ParticleComponent>(id);

            // ---------- FleX Initialization ----------
            NvFlexInitDesc desc = {};
            desc.deviceIndex = 0;
            desc.enableExtensions = true; 
            desc.computeType = eNvFlexCUDA; 

            particle->library = (void*)NvFlexInit(NV_FLEX_VERSION, nullptr, &desc);
            
            //============================
            // If device does not have nvidia gpu it will ensure that 
            // library will not include eNvFlexCUDA
            //============================
            if (!particle->library) {
                NvFlexInitDesc desc = {};
                desc.deviceIndex = 0;
                desc.enableExtensions = false;
                particle->library = (void*)NvFlexInit(NV_FLEX_VERSION, nullptr, &desc);
                if (!particle->library) {
                    LOGGING_ERROR("Failed to initialize FleX.\n");
                    return;
                }
            }

            NvFlexSolverDesc solverDesc;
            NvFlexSetSolverDescDefaults(&solverDesc);
            solverDesc.maxParticles = particle->max_Particles;
            particle->solver = (void*)NvFlexCreateSolver((NvFlexLibrary*)particle->library, &solverDesc);

            //Allocate Flex Buffer
            /*
            pointer 0 -> positionbuffer
            pointer 1 -> velocitybuffer
            pointer 2 -> phasebuffer
            pointer 3 -> activebuff 
            */
            particle->pointers[0] = (void*)NvFlexAllocBuffer((NvFlexLibrary*)particle->library, particle->max_Particles, sizeof(glm::vec4), eNvFlexBufferHost);
            particle->pointers[1] = (void*)NvFlexAllocBuffer((NvFlexLibrary*)particle->library, particle->max_Particles, sizeof(glm::vec3), eNvFlexBufferHost);
            particle->pointers[2] = (void*)NvFlexAllocBuffer((NvFlexLibrary*)particle->library, particle->max_Particles, sizeof(int), eNvFlexBufferHost);
            particle->pointers[3] = (void*)NvFlexAllocBuffer((NvFlexLibrary*)particle->library, particle->max_Particles, sizeof(int), eNvFlexBufferHost);
            particle->pointers[4] = (void*)NvFlexAllocBuffer((NvFlexLibrary*)particle->library, particle->max_Particles, sizeof(float), eNvFlexBufferHost);
            particle->pointers[5] = (void*)NvFlexAllocBuffer((NvFlexLibrary*)particle->library, particle->max_Particles, sizeof(float), eNvFlexBufferHost);
            //Map Buffers
            glm::vec4* positions = (glm::vec4*)NvFlexMap((NvFlexBuffer*)particle->pointers[0], eNvFlexMapWait);
            glm::vec3* velocities = (glm::vec3*)NvFlexMap((NvFlexBuffer*)particle->pointers[1], eNvFlexMapWait);
            int* phases = (int*)NvFlexMap((NvFlexBuffer*)particle->pointers[2], eNvFlexMapWait);
            int* active = (int*)NvFlexMap((NvFlexBuffer*)particle->pointers[3], eNvFlexMapWait);
            float* particleLifetime = (float*)NvFlexMap((NvFlexBuffer*)particle->pointers[4], eNvFlexMapWait);
            float* particleLifetime_Counter = (float*)NvFlexMap((NvFlexBuffer*)particle->pointers[5], eNvFlexMapWait);

            //init the maps
            for (int i = 0; i < particle->max_Particles; ++i) {
                positions[i] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // w=0 means INACTIVE
                velocities[i] = glm::vec3(0.0f);
                //phases[i] = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid);
                phases[i] = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide);
                //phases[i] = NvFlexMakePhase(0, 0);
                particleLifetime[i] = 0.f;
                particleLifetime_Counter[i] = 0.f;
                //do not need to set active over here
            }

            NvFlexUnmap((NvFlexBuffer*)particle->pointers[0]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[1]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[2]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[3]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[4]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[5]);

            NvFlexSetParticles((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[0], nullptr);
            NvFlexSetVelocities((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[1], nullptr);
            NvFlexSetPhases((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[2], nullptr);
            NvFlexSetActive((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[3], nullptr);
            NvFlexSetActiveCount((NvFlexSolver*)particle->solver, 0);

            //init the particle velocity
            particle->freeIndices.clear();
            particle->freeIndices.reserve(particle->max_Particles);
            particle->alive_Particles.clear();
            particle->alive_Particles.reserve(particle->max_Particles);
            particle->visualData.clear();
            particle->visualData.resize(particle->max_Particles);

            for (short i = particle->max_Particles - 1; i >= 0; --i) {
                particle->freeIndices.push_back(i);
            }

          
            // Initialize emitter timers
            particle->emitterTime = 0.f;
            particle->durationCounter = 0.f;

            // ---------- FleX Parameters ----------
            NvFlexParams params;
            NvFlexGetParams((NvFlexSolver*)particle->solver, &params);
            // Gravity settings
            params.gravity[0] = particle->gravity.x;
            params.gravity[1] = particle->gravity.y;  // Standard gravity
            params.gravity[2] = particle->gravity.z;

            // Particle physical properties
            params.radius = 0.05f;
            params.solidRestDistance = params.radius;
            params.fluidRestDistance = params.radius * 0.55f; // Important for fluid behavior

            // Friction and collision
            params.dynamicFriction = 0.1f;
            params.particleFriction = 0.1f;
            params.restitution = 0.3f;
            params.adhesion = 0.0f;
            params.cohesion = 0.025f; // Fluid cohesion
            params.surfaceTension = 0.0f;
            params.viscosity = 0.001f; // Low viscosity for more fluid-like

            // Solver settings
            params.numIterations = 3;
            params.relaxationFactor = 1.0f;

            // Damping (prevents infinite motion)
            params.damping = 0.0f; // Set to 0 for no artificial damping
            params.drag = 0.0f;

            // Collision distance
            params.collisionDistance = params.radius * 0.5f;
            params.shapeCollisionMargin = params.collisionDistance * 0.05f;

            // Particle collision
            params.particleCollisionMargin = params.radius * 0.05f;

            // Sleep threshold (set high to prevent sleeping)
            params.sleepThreshold = 0.0f; // Particles never sleep
            NvFlexSetParams((NvFlexSolver*)particle->solver, &params);

            LOGGING_INFO("Flex initialized successfully for entity %d\n", id);
            LOGGING_INFO("  - Gravity: (%.2f, %.2f, %.2f)\n", params.gravity[0], params.gravity[1], params.gravity[2]);
            LOGGING_INFO("  - Radius: %.3f\n", params.radius);
            LOGGING_INFO("  - Free slots: %zu\n", particle->freeIndices.size());
            });

        onDeregister.Add([&](EntityID id) {
            auto* particle = m_ecs.GetComponent<ParticleComponent>(id);

            if (particle->pointers[0]) {
                NvFlexFreeBuffer((NvFlexBuffer*)particle->pointers[0]);
                particle->pointers[0] = nullptr;
            }
            if (particle->pointers[1]) {
                NvFlexFreeBuffer((NvFlexBuffer*)particle->pointers[1]);
                particle->pointers[1] = nullptr;
            }
            if (particle->pointers[2]) {
                NvFlexFreeBuffer((NvFlexBuffer*)particle->pointers[2]);
                particle->pointers[2] = nullptr;
            }
            if (particle->pointers[3]) {
                NvFlexFreeBuffer((NvFlexBuffer*)particle->pointers[3]);
                particle->pointers[3] = nullptr;
            }
            if (particle->pointers[4]) {
                NvFlexFreeBuffer((NvFlexBuffer*)particle->pointers[4]);
                particle->pointers[4] = nullptr;
            }

            if (particle->pointers[5]) {
                NvFlexFreeBuffer((NvFlexBuffer*)particle->pointers[5]);
                particle->pointers[5] = nullptr;
            }

            // Destroy solver BEFORE library
            if (particle->solver) {
                NvFlexDestroySolver((NvFlexSolver*)particle->solver);
                particle->solver = nullptr;
            }

            // Destroy library last
            if (particle->library) {
                NvFlexShutdown((NvFlexLibrary*)particle->library);
                particle->library = nullptr;
            }

            // Clear tracking data
            particle->freeIndices.clear();
            particle->alive_Particles.clear();

            });
    }

    void ParticleSystem::Update() {
        const auto& entities = m_entities.Data();
        float dt = m_ecs.m_GetDeltaTime();
       
        for (EntityID id : entities) {
            ParticleComponent* particle = m_ecs.GetComponent<ParticleComponent>(id);
            TransformComponent* transform = m_ecs.GetComponent<TransformComponent>(id);
            if (!particle || !particle->solver || !transform) {
                return;
            }

            // --- PERFORMANCE OPTIMIZATION: Map position buffer ONCE for the frame ---
            glm::vec4* positions = (glm::vec4*)NvFlexMap((NvFlexBuffer*)particle->pointers[0], eNvFlexMapWait);
            glm::vec3* velocity = (glm::vec3*)NvFlexMap((NvFlexBuffer*)particle->pointers[1], eNvFlexMapWait);
            float* lifetime_list = (float*)NvFlexMap((NvFlexBuffer*)particle->pointers[4], eNvFlexMapWait);
            float* lifetime_Counter_List = (float*)NvFlexMap((NvFlexBuffer*)particle->pointers[5], eNvFlexMapWait);
            if (!positions) {
                return;
            }

            //===========================================
            // Update emitter
            //===========================================
            UpdateEmitters(dt, id, particle, transform, positions, velocity, lifetime_list, lifetime_Counter_List);

            // ==========================================
            // Update Lifetimes (modifies alive_Particles)
            // ==========================================
            UpdateParticleLifetimes(dt, particle, positions,velocity, lifetime_list, lifetime_Counter_List);

            // ==========================================
            // Sync Active Buffer to GPU
            // ==========================================
            SyncActiveBuffer(particle);

            // ==========================================
            // UPDATE GRAVITY IN REAL TIME
            // ==========================================
            NvFlexParams params;
            NvFlexGetParams((NvFlexSolver*)particle->solver, &params);

            // Set gravity based on user input / inspector
            params.gravity[0] = particle->gravity.x;
            params.gravity[1] = particle->gravity.y;
            params.gravity[2] = particle->gravity.z;

            NvFlexSetParams((NvFlexSolver*)particle->solver, &params);

            // ==========================================
            // Sync Particle Data to Flex
            // ==========================================
            // === UNMAP BUFFERS BEFORE GPU OPERATIONS ===
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[0]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[1]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[4]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[5]);

            NvFlexSetParticles((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[0], nullptr);
            NvFlexSetVelocities((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[1], nullptr);

            // ==========================================
            // Run Flex Simulation
            // ==========================================

            NvFlexUpdateSolver((NvFlexSolver*)particle->solver, dt, 1, false);

            // ==========================================
            // Retrieve Results
            // ==========================================

            NvFlexGetParticles((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[0], nullptr);
            NvFlexGetVelocities((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[1], nullptr);
            // ==========================================
            // Extract Positions for Rendering (OPTIMIZED)
            // ==========================================
            positions = (glm::vec4*)NvFlexMap((NvFlexBuffer*)particle->pointers[0], eNvFlexMapWait);

            ParticleInstance sending;
            ExtractParticleDataOptimized(particle, sending, positions);

            NvFlexUnmap((NvFlexBuffer*)particle->pointers[0]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[1]);
            // ==========================================
            // Sending over to the Graphic Pipeline to render
            // ==========================================
            m_graphicsManager.gm_PushBasicParticleData(BasicParticleData{ sending.positions_Particle, sending.colors, sending.sizes, sending.rotates });
        }
    }


    void ParticleSystem::EmitParticle(EntityID entityId, const glm::vec3& particle_position, const glm::vec3& velocity, float lifetime, ParticleComponent*& particle, glm::vec4* position, glm::vec3* velocities, float* lifetime_list, float* lifetime_Counter_list) {
        // Check if we have any free slots
        if (particle->freeIndices.empty()) {
            LOGGING_WARN("No free particle slots for entity %d\n", entityId);
            return;
        }

        // FIX: Get a free particle index (O(1) operation)
        int particleIdx = particle->freeIndices.back();
        particle->freeIndices.pop_back();

        // FIX: Set particle tracking data
        lifetime_list[particleIdx] = lifetime;
        lifetime_Counter_list[particleIdx] = lifetime;
        particle->alive_Particles.push_back(particleIdx);

        //adding visual data
        ParticleVisual data;

        //init particle data
        data.color = particle->colorModule.enabled ? RandomColourRange(particle->colorModule.start_Color, particle->colorModule.end_Color) : particle->colorModule.start_Color;
        data.size = particle->sizeModule.enabled ? AbsRandomRange(particle->sizeModule.start_Size, particle->sizeModule.end_Size) : particle->sizeModule.start_Size;
        data.rotation = particle->rotationModule.enabled ? glm::radians(RandomRange(particle->rotationModule.start_Rotation, particle->rotationModule.end_Rotation)) : glm::radians(particle->rotationModule.start_Rotation);




        particle->visualData[particleIdx] = data;

        if (position && velocities) {
            // FIX: Set position with w=1.0 to make particle ACTIVE
            position[particleIdx] = glm::vec4(particle_position, 1.0f);
            velocities[particleIdx] = velocity;
        }
    }

    void ParticleSystem::UpdateParticleLifetimes(float dt, ParticleComponent*& particle, glm::vec4* positions,glm::vec3* velocities, float* lifetime_list, float* lifetime_Counter_list) {
        if (!positions) {
            return;
        }

        auto it = particle->alive_Particles.begin();
        while (it != particle->alive_Particles.end()) {
            short particleIdx = *it;
            ParticleVisual& pv = particle->visualData[particleIdx];
            float t =  1.f - (lifetime_list[particleIdx] / lifetime_Counter_list[particleIdx]);
            t = glm::clamp(t, 0.0f, 1.0f);

            //UPDATING VELOCITY
            if (velocities) {
                if (particle->velocityModule.enabled) {
                    switch (particle->velocityModule.mode) {
                    case Absolute:
                        velocities[particleIdx] = glm::mix(velocities[particleIdx], particle->velocityModule.velocity_Modifier, t);
                        break;

                    case Additive:                       
                        velocities[particleIdx] += glm::mix(glm::vec3(0), particle->velocityModule.velocity_Modifier, t) * dt;
                        break;

                    case Multiplier:
                    {
                        float factor = glm::mix(1.0f, glm::length(particle->velocityModule.velocity_Modifier), t);
                        velocities[particleIdx] *= factor;
                        break;
                    }
                    default:
                        break;
                    }
                }

                if (particle->forceModule.enabled) {
                    velocities[particleIdx] = velocities[particleIdx] + (particle->forceModule.force * dt);
                }
            }

            //UPDATING SIZE
            if (particle->sizeModule.enabled) {
                pv.size = glm::mix(particle->sizeModule.start_Size, particle->sizeModule.end_Size, t);
            }

            //UPDATING COLOR
            if (particle->colorModule.enabled) {
                pv.color = glm::mix(particle->colorModule.start_Color, particle->colorModule.end_Color, t);
            }
            
            //UPDATING ROTATION
            if (particle->rotationModule.enabled) {
                pv.rotation += glm::radians(particle->rotationModule.rotation_Modifier) * dt;
            }

            //Updating each of the particle lifespan
            if (lifetime_list[particleIdx] > 0.0f) {
                lifetime_list[particleIdx] -= dt;
               
                if (lifetime_list[particleIdx] <= 0.0f) {
                    positions[particleIdx].w = 0.0f;
                    particle->freeIndices.push_back(particleIdx);
                    it = particle->alive_Particles.erase(it);
                }
                else {
                    ++it;
                }
            }
            else {
                // Already dead, remove it
                it = particle->alive_Particles.erase(it);
            }
        }
    }

    void ParticleSystem::UpdateEmitters(float dt,EntityID id, ParticleComponent*& particleComp,  TransformComponent* transform, glm::vec4* position, glm::vec3* velocities, float* lifetime_list, float* lifetime_Counter_list) {
        const auto& entities = m_entities.Data();

        // Check if emitter should be active
        bool shouldEmit = false;
        bool particles_Alive = false;
        if (particleComp->looping) {
            if (particleComp->durationCounter >= particleComp->duration) {
                particleComp->durationCounter = 0.f;
            }
            shouldEmit = particleComp->play_On_Awake;
        }
        else {
            if (particleComp->play_On_Awake) {
                particleComp->durationCounter += dt;
                shouldEmit = (particleComp->durationCounter < particleComp->duration);
            }
        }
        if (shouldEmit) {
            particleComp->emitterTime += dt;
            float emissionInterval = particleComp->emissionInterval;

            if (emissionInterval <= 0.0f) {
                return;  // Simply don't emit particles
            }

            while (particleComp->emitterTime >= emissionInterval) {
               

                // ==== = GENERATE DIRECTION BASED ON EMISSION SHAPE ==== =
                EmissionData emission; 

                switch (particleComp->shapeModule.type) {
                case EmissionShape::BOX:
                    emission = GenerateBoxEmission(particleComp);
                    break;
                case EmissionShape::CONE:
                    emission = GenerateConeEmission(particleComp);
                    break;
                case EmissionShape::SPHERE:
                    emission = GenerateSphereEmission(particleComp);
                    break;
                case EmissionShape::CIRCLE:
                    emission = GenerateCircleEmission(particleComp);
                    break;
                case EmissionShape::EDGE:
                    emission = GenerateEdgeEmission(particleComp);
                    break;
                default:
                    emission = GenerateConeEmission(particleComp);
                    break;
                }


                // ===== APPLY SPEED TO DIRECTION =====
                // direction is already normalized, multiply by speed
                glm::vec3 vel = emission.direction * particleComp->start_Velocity;              
                glm::vec3 pos = transform->WorldTransformation.position + emission.positionOffset;
                
                //random to the lifetime
                float particle_Lifetime = particleComp->start_Lifetime;
                if (particleComp->lifetime_Random_Enable) {
                    particle_Lifetime = RandomRange(particleComp->start_Lifetime, particleComp->end_Lifetime);
                }

                EmitParticle(id, pos, vel, particle_Lifetime, particleComp, position, velocities, lifetime_list, lifetime_Counter_list);
                particleComp->emitterTime -= emissionInterval;
            }
        }
    }

 
    void ParticleSystem::SyncActiveBuffer(ParticleComponent* particle) {
        if (!particle) {
            return;
        }
        
        if (particle->alive_Particles.empty()) {
            // No active particles
            NvFlexSetActiveCount((NvFlexSolver*)particle->solver, 0);
            return;
        }
        int activeCount = static_cast<int>(particle->alive_Particles.size());

        int* gpu = (int*)NvFlexMap((NvFlexBuffer*)particle->pointers[3], eNvFlexMapWait);
        if (gpu) {
            for (short i = 0; i < activeCount; i++) {
                gpu[i] = particle->alive_Particles[i];
            }
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[3]);
        }

        // Update Flex
        NvFlexSetActive((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[3], nullptr);
        NvFlexSetActiveCount((NvFlexSolver*)particle->solver, activeCount);
    }

    // NEW: Optimized position extraction
    void ParticleSystem::ExtractParticleDataOptimized(ParticleComponent* particle,
        ParticleInstance& data, glm::vec4* positions) {
        if (!particle || particle->alive_Particles.empty()) {
            data.positions_Particle.clear();
            return;
        }

        // OPTIMIZATION: Only extract active particles
        data.positions_Particle.clear();
        data.colors.clear();
        data.rotates.clear();
        data.sizes.clear();
        data.positions_Particle.reserve(particle->alive_Particles.size());
        data.colors.reserve(particle->alive_Particles.size());
        data.rotates.reserve(particle->alive_Particles.size());
        data.sizes.reserve(particle->alive_Particles.size());

        for (int idx : particle->alive_Particles) {
            if (positions[idx].w > 0.0f) {
                data.positions_Particle.emplace_back(positions[idx].x, positions[idx].y, positions[idx].z);
                data.colors.emplace_back(particle->visualData[idx].color);
                data.sizes.emplace_back(glm::vec2(particle->visualData[idx].size, particle->visualData[idx].size));
                data.rotates.emplace_back(particle->visualData[idx].rotation);
            }
        }

    }
    // ========================================
    // BOX EMISSION (with position offset)
    // ========================================
    EmissionData ParticleSystem::GenerateBoxEmission(ParticleComponent* particle) {
        EmissionData data;

        // Random position within box
        float randX = RandomRange(-particle->shapeModule.box_Size.x * 0.5f, particle->shapeModule.box_Size.x * 0.5f);
        float randY = RandomRange(-particle->shapeModule.box_Size.y * 0.5f, particle->shapeModule.box_Size.y * 0.5f);
        float randZ = RandomRange(-particle->shapeModule.box_Size.z * 0.5f, particle->shapeModule.box_Size.z * 0.5f);

        data.positionOffset = glm::vec3(randX, randY, randZ);

        // Random direction
        glm::vec3 direction = glm::vec3(
            RandomRange(-1.0f, 1.0f),
            RandomRange(-1.0f, 1.0f),
            RandomRange(-1.0f, 1.0f)
        );

        if (glm::length(direction) > 0.001f) {
            direction = glm::normalize(direction);
        }
        else {
            direction = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        data.direction = ApplyRandomDirection(direction, particle->shapeModule.random_Direction_Amount);
        return data;
    }

    // ========================================
    // CONE EMISSION (no position offset needed)
    // ========================================
    EmissionData ParticleSystem::GenerateConeEmission(ParticleComponent* particle) {
        EmissionData data;

        // Optional: Random position at cone base
        if (particle->shapeModule.cone_Radius > 0.0f) {
            float randAngle = RandomRange(0.0f, 2.0f * glm::pi<float>());
            float randRadius = glm::sqrt(RandomRange(0.0f, 1.0f)) * particle->shapeModule.cone_Radius;

            data.positionOffset = glm::vec3(
                cos(randAngle) * randRadius,
                0.0f,
                sin(randAngle) * randRadius
            );
        }
        else {
            data.positionOffset = glm::vec3(0.0f);
        }

        // Generate direction within cone
        float angleRad = glm::radians(particle->shapeModule.cone_Angle);
        float theta = RandomRange(0.0f, 2.0f * glm::pi<float>());
        float phi = RandomRange(0.0f, angleRad);

        float sinPhi = sin(phi);
        glm::vec3 localDir = glm::vec3(
            sinPhi * cos(theta),
            cos(phi),
            sinPhi * sin(theta)
        );

        // Rotate to match emission direction
        glm::vec3 baseDir = glm::normalize(particle->shapeModule.emission_Direction);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        if (abs(glm::dot(baseDir, up)) > 0.999f) {
            up = glm::vec3(1.0f, 0.0f, 0.0f);
        }

        glm::vec3 right = glm::normalize(glm::cross(up, baseDir));
        glm::vec3 forward = glm::normalize(glm::cross(baseDir, right));

        glm::mat3 rotationMatrix = glm::mat3(right, baseDir, forward);
        glm::vec3 worldDir = rotationMatrix * localDir;

        data.direction = ApplyRandomDirection(glm::normalize(worldDir), particle->shapeModule.random_Direction_Amount);
        return data;
    }

    // ========================================
    // SPHERE EMISSION
    // ========================================
    EmissionData ParticleSystem::GenerateSphereEmission(ParticleComponent* particle) {
        EmissionData data;

        // Generate random point on/in sphere
        float theta = RandomRange(0.0f, 2.0f * glm::pi<float>());
        float phi = RandomRange(0.0f, glm::pi<float>());

        float radiusMultiplier = 1.0f;
        if (!particle->shapeModule.emit_From_Shell) {
            // Emit from volume
            radiusMultiplier = glm::pow(RandomRange(0.0f, 1.0f), 1.0f / 3.0f);
        }

        // Spherical to Cartesian
        float sinPhi = sin(phi);
        glm::vec3 direction = glm::vec3(
            sinPhi * cos(theta),
            cos(phi),
            sinPhi * sin(theta)
        );

        // Position offset at sphere surface/volume
        data.positionOffset = direction * particle->shapeModule.sphere_Radius * radiusMultiplier;

        // Direction is outward from center
        data.direction = ApplyRandomDirection(glm::normalize(direction), particle->shapeModule.random_Direction_Amount);
        return data;
    }

    // ========================================
    // CIRCLE EMISSION 
    // ========================================
    EmissionData ParticleSystem::GenerateCircleEmission(ParticleComponent* particle) {
        EmissionData data;

        float arcRad = glm::radians(particle->shapeModule.circle_Arc);
        float angle = RandomRange(0.0f, arcRad);

        float radiusMultiplier = 1.0f;
        if (!particle->shapeModule.emit_From_Edge) {
            // Emit from disc area (sqrt for uniform distribution)
            radiusMultiplier = glm::sqrt(RandomRange(0.0f, 1.0f));
        }

        // Position on circle in XZ plane
        float actualRadius = particle->shapeModule.circle_Radius * radiusMultiplier;
        data.positionOffset = glm::vec3(
            cos(angle) * actualRadius,
            0.0f,
            sin(angle) * actualRadius
        );

        //// Direction: outward from circle + upward component
        //glm::vec3 outward = glm::normalize(glm::vec3(cos(angle), 0.0f, sin(angle)));
        //glm::vec3 baseDir = glm::normalize(particle->emission_Direction);

        //// Mix outward and upward direction
        //data.direction = glm::normalize(outward * 0.3f + baseDir * 0.7f);
        //data.direction = ApplyRandomDirection(data.direction, particle->random_Direction_Amount);
        // 
        //horizontal only
        data.direction = glm::vec3(
            cos(angle),  // Same angle as position
            0.0f,        // NO vertical component!
            sin(angle)
        );
        data.direction = ApplyRandomDirection(data.direction, particle->shapeModule.random_Direction_Amount);
        return data;
    }

    // ========================================
    // EDGE (LINE) EMISSION (FIXED - Now uses length!)
    // ========================================
    EmissionData ParticleSystem::GenerateEdgeEmission(ParticleComponent* particle) {
        EmissionData data;

        // Random position along line
        float t = RandomRange(-1.0f, 1.0f);

        // Position offset along the line (perpendicular to emission direction)
        glm::vec3 lineDir = glm::vec3(1.0f, 0.0f, 0.0f);  // Default: along X axis

        // If emission direction is along X, use Z instead
        glm::vec3 emitDir = glm::normalize(particle->shapeModule.emission_Direction);
        if (abs(glm::dot(emitDir, lineDir)) > 0.9f) {
            lineDir = glm::vec3(0.0f, 0.0f, 1.0f);
        }

        data.positionOffset = lineDir * t * particle->shapeModule.edge_Length * 0.5f;

        // Direction is the emission direction
        data.direction = ApplyRandomDirection(emitDir, particle->shapeModule.random_Direction_Amount);

        return data;
    }

    void* ParticleSystem::getVoid(ParticleComponent* particle, STATE state) {
        void* ret;
        if (state >= STATE::counter) {
            LOGGING_ERROR("WRONG STATE FOR PARTICLE SYSTEM");
            return particle->pointers[0];
        }
        ret = particle->pointers[state];
        return ret;
    }
    glm::vec3 ParticleSystem::ApplyRandomDirection(const glm::vec3& direction, float randomAmount) {
        if (randomAmount <= 0.0f) {
            return direction;  // No randomness
        }

        // Generate a completely random direction
        float theta = RandomRange(0.0f, 2.0f * glm::pi<float>());
        float phi = RandomRange(0.0f, glm::pi<float>());

        float sinPhi = sin(phi);
        glm::vec3 randomDir = glm::vec3(
            sinPhi * cos(theta),
            cos(phi),
            sinPhi * sin(theta)
        );
        glm::vec3 result = glm::normalize(
            glm::mix(direction, randomDir, randomAmount)
        );

        return result;
    }

}