#include "Config/pch.h"
#include "ParticleSystem.h"
#include "glm/glm.hpp"
#include "Graphics/GraphicsManager.h"


namespace ecs {

    void ParticleSystem::Init() {

        onRegister.Add([](EntityID id) {
            ECS* ecs = ECS::GetInstance();
            ParticleComponent* particle = ecs->GetComponent<ParticleComponent>(id);

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
            //Map Buffers
            glm::vec4* positions = (glm::vec4*)NvFlexMap((NvFlexBuffer*)particle->pointers[0], eNvFlexMapWait);
            glm::vec3* velocities = (glm::vec3*)NvFlexMap((NvFlexBuffer*)particle->pointers[1], eNvFlexMapWait);
            int* phases = (int*)NvFlexMap((NvFlexBuffer*)particle->pointers[2], eNvFlexMapWait);
            int* active = (int*)NvFlexMap((NvFlexBuffer*)particle->pointers[3], eNvFlexMapWait);
            float* particleLifetime = (float*)NvFlexMap((NvFlexBuffer*)particle->pointers[4], eNvFlexMapWait);


            //init the maps
            for (int i = 0; i < particle->max_Particles; ++i) {
                positions[i] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // w=0 means INACTIVE
                velocities[i] = glm::vec3(0.0f);
                phases[i] = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid);
                particleLifetime[i] = 0.f;
                //do not need to set active over here
            }

            NvFlexUnmap((NvFlexBuffer*)particle->pointers[0]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[1]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[2]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[3]);
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[4]);


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
            params.gravity[0] = 0.0f;
            params.gravity[1] = -9.8f;  // Standard gravity
            params.gravity[2] = 0.0f;

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

        onDeregister.Add([](EntityID id) {
            ECS* ecs = ECS::GetInstance();
            auto* particle = ecs->GetComponent<ParticleComponent>(id);

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
        ECS* ecs = ECS::GetInstance();
        std::shared_ptr<GraphicsManager> gm = GraphicsManager::GetInstance();
        const auto& entities = m_entities.Data();
        float dt = ecs->m_GetDeltaTime();
       
        for (EntityID id : entities) {
            ParticleComponent* particle = ecs->GetComponent<ParticleComponent>(id);
            TransformComponent* transform = ecs->GetComponent<TransformComponent>(id);
            if (!particle || !particle->solver || !transform) {
                continue;
            }

            // --- PERFORMANCE OPTIMIZATION: Map position buffer ONCE for the frame ---
            glm::vec4* positions = (glm::vec4*)NvFlexMap((NvFlexBuffer*)particle->pointers[0], eNvFlexMapWait);
            if (!positions) {
                continue;
            }

            //===========================================
            //STEP 0: Update emitter
            //===========================================
            std::chrono::duration<float> systemDuration{};
            auto start = std::chrono::steady_clock::now();
            UpdateEmitters(dt,id , particle, transform);
            auto end = std::chrono::steady_clock::now();
            systemDuration = (end - start);
            //std::cout << "UpdateEmitter " << systemDuration.count() << std::endl;
            // ==========================================
            // STEP 1: Update Lifetimes (modifies alive_Particles)
            // ==========================================

            start = std::chrono::steady_clock::now();
            UpdateParticleLifetimes(dt, particle, positions);
            end = std::chrono::steady_clock::now();
            systemDuration = (end - start);
            //std::cout << "UpdateParticle " << systemDuration.count() << std::endl;
            // ==========================================
            // STEP 2: Sync Active Buffer to GPU
            // ==========================================
            start = std::chrono::steady_clock::now();
            SyncActiveBuffer(particle);
            end = std::chrono::steady_clock::now();
            systemDuration = (end - start);
            //std::cout << "Active " << systemDuration.count() << std::endl;
            // ==========================================
            // STEP 3: Sync Particle Data to Flex
            // ==========================================
            start = std::chrono::steady_clock::now();
            NvFlexSetParticles((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[0], nullptr);
            NvFlexSetVelocities((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[1], nullptr);
            end = std::chrono::steady_clock::now();
            systemDuration = (end - start);
            //std::cout << "Step 3 " << systemDuration.count() << std::endl;
            // ==========================================
            // STEP 4: Run Flex Simulation
            // ==========================================
            start = std::chrono::steady_clock::now();
            NvFlexUpdateSolver((NvFlexSolver*)particle->solver, dt, 1, false);
            end = std::chrono::steady_clock::now();
            systemDuration = (end - start);
            //std::cout << "Step 4 " << systemDuration.count() << std::endl;
            // ==========================================
            // STEP 5: Retrieve Results
            // ==========================================
            start = std::chrono::steady_clock::now();
            NvFlexGetParticles((NvFlexSolver*)particle->solver, (NvFlexBuffer*)particle->pointers[0], nullptr);
            end = std::chrono::steady_clock::now();
            systemDuration = (end - start);
            //std::cout << "Step 5 " << systemDuration.count() << std::endl;
            // ==========================================
            // STEP 6: Extract Positions for Rendering (OPTIMIZED)
            // ==========================================
            ParticleInstance sending;
            start = std::chrono::steady_clock::now();
            ExtractParticlePositionsOptimized(particle, sending.positions_Particle, positions);
            end = std::chrono::steady_clock::now();
            systemDuration = (end - start);
            //std::cout << "Step 6 " << systemDuration.count() << std::endl;
            sending.color = particle->color;
            sending.scale = glm::vec3(particle->size);
            sending.rotate = particle->rotation;

            // --- PERFORMANCE OPTIMIZATION: Unmap position buffer ONCE at the end ---
            NvFlexUnmap((NvFlexBuffer*)particle->pointers[0]);

            
            gm->gm_PushBasicParticleData(BasicParticleData{ sending.positions_Particle, sending.color, {sending.scale.x, sending.scale.y}, sending.rotate });
        }
    }

    void ParticleSystem::EmitParticle(EntityID entityId, const glm::vec3& particle_position, const glm::vec3& velocity, float lifetime, ParticleComponent*& particle, glm::vec4* position, glm::vec3* velocities, float* lifetime_list) {
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
        
        particle->alive_Particles.push_back(particleIdx);
       
        if (position && velocities) {
            // FIX: Set position with w=1.0 to make particle ACTIVE
            position[particleIdx] = glm::vec4(particle_position, 1.0f);
            velocities[particleIdx] = velocity;
        }
    }

    void ParticleSystem::UpdateParticleLifetimes(float dt, ParticleComponent*& particle, glm::vec4* positions) { 
        if (!positions) {
            return;
        }

        float* lifetime = (float*)NvFlexMap((NvFlexBuffer*)particle->pointers[4], eNvFlexMapWait);
        particle->alive_Particles.clear();
        for (short i = 0; i < (short)particle->max_Particles; ++i) {
            // Only process particles that are alive
            if (lifetime[i] > 0.0f) {
                lifetime[i] -= dt;

                // Check if particle died this frame
                if (lifetime[i] <= 0.0f) {
                    // DEACTIVATE particle by setting w = 0 (Flex will ignore it)
                    positions[i].w = 0.0f;
                    particle->freeIndices.push_back(i);
                }
                else {
                    particle->alive_Particles.push_back(i);
                }
            }
        }
        NvFlexUnmap((NvFlexBuffer*)particle->pointers[4]);
    }

    //LOGIC ERROR IN THE EMITTER
    void ParticleSystem::UpdateEmitters(float dt,EntityID id, ParticleComponent*& particleComp,  TransformComponent* transform) {
        ECS* ecs = ECS::GetInstance();
        const auto& entities = m_entities.Data();


            
        // Check if emitter should be active
        bool shouldEmit = false;

        if (particleComp->looping) {
            shouldEmit = particleComp->play_On_Awake;
        }
        else {
            if (particleComp->play_On_Awake) {
                particleComp->durationCounter += dt;
                shouldEmit = (particleComp->durationCounter < particleComp->duration);
            }
        }

        if (shouldEmit) {
            // --- PERFORMANCE OPTIMIZATION: Map buffers ONCE before emission loop ---
            glm::vec4* positions = (glm::vec4*)NvFlexMap((NvFlexBuffer*)particleComp->pointers[0], eNvFlexMapWait);
            glm::vec3* velocities = (glm::vec3*)NvFlexMap((NvFlexBuffer*)particleComp->pointers[1], eNvFlexMapWait);
            float* lifetime = (float*)NvFlexMap((NvFlexBuffer*)particleComp->pointers[4], eNvFlexMapWait);
            particleComp->emitterTime += dt;
            //std::cout << particleComp->emitterTimers[entityIdx] << std::endl;
            // Emission rate: 10 particles per second (0.1s interval)
            float emissionInterval = 0.1f;

            while (particleComp->emitterTime >= emissionInterval) {
                glm::vec3 pos = transform->WorldTransformation.position;

                // Add some randomness to velocity
                float randX = (rand() % 200 - 100) / 100.0f;
                float randZ = (rand() % 200 - 100) / 100.0f;

                //Calculating of the velocity
                glm::vec3 vel = glm::vec3(
                    randX * particleComp->start_Velocity,
                    particleComp->start_Velocity,
                    randZ * particleComp->start_Velocity
                );


                EmitParticle(id, pos, vel, particleComp->start_Lifetime, particleComp, positions, velocities, lifetime);
                particleComp->emitterTime -= emissionInterval;
            }
            // --- Unmap buffers ONCE after the emission loop ---
            NvFlexUnmap((NvFlexBuffer*)particleComp->pointers[0]);
            NvFlexUnmap((NvFlexBuffer*)particleComp->pointers[1]);
            NvFlexUnmap((NvFlexBuffer*)particleComp->pointers[4]);
        }

    }

    // NEW: Sync active buffer to GPU (only when needed)
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
    void ParticleSystem::ExtractParticlePositionsOptimized(ParticleComponent* particle,
        std::vector<glm::vec3>& outPositions, glm::vec4* positions) {
        if (!particle || particle->alive_Particles.empty()) {
            outPositions.clear();
            return;
        }

        // OPTIMIZATION: Only extract active particles
        outPositions.clear();
        outPositions.reserve(particle->alive_Particles.size());

        for (int idx : particle->alive_Particles) {
            if (positions[idx].w > 0.0f) {
                outPositions.emplace_back(positions[idx].x, positions[idx].y, positions[idx].z);
                //std::cout << "Particle " << idx << " " << positions[idx].x << " " << positions[idx].y << " " << positions[idx].z << std::endl;
            }
        }
    }

}