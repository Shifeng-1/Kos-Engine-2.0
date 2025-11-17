#ifndef PARTICLECOMPONENT_H
#define PARTICLECOMPONENT_H

#include "Component.h"


namespace ecs {
	//For each particle
	struct ParticleVisual {
		glm::vec4 color = glm::vec4(1.f);
		float size = 1.f;
		float rotation = 0.f;
		
	};

	enum Velocity_Mode {
		Absolute,
		Additive,
		Multiplier
	};

	enum EmissionShape {
		BOX,       
		CONE,       
		SPHERE,    
		CIRCLE,    
		EDGE        
	};

	struct ShapeModule {
		EmissionShape type = EmissionShape::CONE;
		glm::vec3 emission_Direction = glm::vec3(0.f, 1.f, 0.f);
		float random_Direction_Amount;
		glm::vec3 box_Size;
		
		float sphere_Radius;
		bool emit_From_Shell;

		float cone_Angle;
		glm::vec3 cone_Direction;
		float cone_Radius;

		float circle_Radius;
		float circle_Arc;
		bool emit_From_Edge;

		float edge_Length;

		REFLECTABLE(ShapeModule, type, emission_Direction,
					box_Size,
					sphere_Radius, emit_From_Shell,
					cone_Angle, cone_Direction, cone_Radius,
					circle_Radius, circle_Arc, emit_From_Edge,
					edge_Length, random_Direction_Amount);
	};

	struct VelocityOverLifetimeModule {
		bool enabled = false;
		Velocity_Mode mode = Velocity_Mode::Absolute;
		glm::vec3 velocity_Modifier = glm::vec3(0);

		REFLECTABLE(VelocityOverLifetimeModule,enabled, mode, velocity_Modifier);
	};

	struct ForceOverLifetimeModule {
		bool enabled = false;
		glm::vec3 force = glm::vec3(0);
		
		REFLECTABLE(ForceOverLifetimeModule, enabled, force);
	};

	struct SizeOverLifetimeModule {
		bool enabled = false;
		float start_Size = 1.0f;
		float end_Size = 1.0f;

		REFLECTABLE(SizeOverLifetimeModule, enabled, start_Size, end_Size);
	};

	struct ColorOverLifetimeModule {
		bool enabled = false;
		glm::vec4 start_Color = glm::vec4(1.f);
		glm::vec4 end_Color = glm::vec4(0.f);
		
		REFLECTABLE(ColorOverLifetimeModule, enabled, start_Color, end_Color);
	};

	struct RotationOverLifetimeModule {
		bool enabled = false;
		float start_Rotation = 0.f;
		float end_Rotation = 0.f;
		float rotation_Modifier = 0.f;

		REFLECTABLE(RotationOverLifetimeModule, enabled, start_Rotation, end_Rotation, rotation_Modifier);
	};

	class ParticleComponent : public Component {
	public:
		//NO CHANGES
		int max_Particles = 255; //max particle size
		float duration = 5.0f;
		bool looping = true;
		bool play_On_Awake = true;
		
		//Lifetime
		float start_Lifetime = 3.0f;
		float end_Lifetime	 = 7.0f;
		bool  lifetime_Random_Enable = true;
		float start_Velocity = 5.0f;

		//Color over lifetime
		ColorOverLifetimeModule colorModule;

		// Velocity over lifetime
		VelocityOverLifetimeModule velocityModule;
		
		//Force over Lifetime
		ForceOverLifetimeModule forceModule;
		
		//Shape module
		ShapeModule shapeModule;

		// Size over lifetime
		SizeOverLifetimeModule sizeModule;

		// Rotation over lifetime
		RotationOverLifetimeModule rotationModule;

		//Drag Gravity Damping
		glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
 
		//NvFlex 
		void* pointers[6];
		void* library;
		void* solver;

		//FOR THE ALIVE PARTICLES
		std::vector<short> freeIndices;                       
		std::vector<short> alive_Particles;
		
		//EMISSTION RATE
		float emitterTime = 0.f;
		float durationCounter = 0.f;
		float emissionInterval = 0.1f;


		//Per particle visual data
		std::vector<ParticleVisual> visualData;


		REFLECTABLE(ParticleComponent, duration, looping, play_On_Awake, 
					start_Lifetime, end_Lifetime, lifetime_Random_Enable,
					start_Velocity,
					velocityModule,forceModule, shapeModule, colorModule, sizeModule, rotationModule,
					gravity,
					emissionInterval);
	};
}
#endif