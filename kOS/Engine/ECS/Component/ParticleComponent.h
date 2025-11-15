#ifndef PARTICLECOMPONENT_H
#define PARTICLECOMPONENT_H

#include "Component.h"


namespace ecs {
	struct ParticleVisual {
		float size;
		float rotation;
		glm::vec4 color = glm::vec4(1.f);
	};
	class ParticleComponent : public Component {
	public:
		float duration = 5.0f;
		bool looping = true;
		bool play_On_Awake = true;
		float start_Lifetime = 3.0f;
		float end_Lifetime	 = 7.0f;
		bool  lifetime_Random = true;
		float start_Velocity = 5.0f;
		int max_Particles = 255; //max particle size
		
		//Color over Lifetime
		glm::vec4 start_Color = glm::vec4(1.0f);
		glm::vec4 end_Color = glm::vec4(1.0f);
		bool color_Over_Lifetime = false;

		// Velocity over lifetime
		bool velocity_Over_Lifetime = false;
		glm::vec3 velocity_Modifier = glm::vec3(0.0f);
		//TEST
		// Size over lifetime
		float start_Size = 0.1f;
		float end_Size = 0.1f;
		bool size_Over_Lifetime = false;

		// Rotation over lifetime
		float rotation = 0.f;
		float rotation_Over_Lifetime = 0.0f;

		// === PHYSICS (Flex handles this, but we control the params) ===
		glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
		float drag = 0.0f;
		float damping = 0.0f;


		//NvFlex 
		void* pointers[6];
		void* library;
		void* solver;

		//FOR THE ALIVE PARTICLES
		std::vector<short> freeIndices;                       
		std::vector<short> alive_Particles;
		
		//EMISSTION RATE
		float emissionRate = 10.f; //particles per second
		float emitterTime = 0.f;
		float durationCounter = 0.f;
		float emissionInterval = 0.1f;


		//Per particle visual data
		std::vector<ParticleVisual> visualData;


		REFLECTABLE(ParticleComponent, duration, looping, play_On_Awake, start_Lifetime, start_Velocity,
					start_Color, end_Color, color_Over_Lifetime,
					velocity_Over_Lifetime, velocity_Modifier, 
					start_Size, end_Size , size_Over_Lifetime, 
					rotation, rotation_Over_Lifetime,
					gravity,
					emissionInterval);
	};
}
#endif