#include "TemplateSC.h"
//#include "Inputs/Input.h"

class PlayerManagerScript : public TemplateSC {
public:
	int playerHealth;
	float playerMovementSpeed;
	float playerJumpForce;


	void Start() override {

	}

	void Update() override {
		if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {
			float pitch = tc->LocalTransformation.rotation.x;
			float yaw = tc->LocalTransformation.rotation.y;

			glm::vec3 forward = {
				cos(pitch) * sin(yaw),
				-sin(pitch),
				cos(pitch) * cos(yaw)
			};

			glm::vec3 right = {
				cos(yaw),
				0.0f,
				-sin(yaw)
			};

			if (Input->IsKeyTriggered(keys::A)) {
				std::cout << "AAAAAAAAAAAAAAAAAAAAAah\n";
			}

			if (Input->GetInstance()->IsKeyTriggered(keys::S)) {
				std::cout << "BBBBBBBBBBBBBBBBBBB\n";
			}

			if (Input->IsKeyPressed(keys::W)) {
				std::cout << "MOVING FORWARD\n";
				tc->LocalTransformation.position += glm::normalize(forward) * ecsPtr->m_GetDeltaTime();
			}
		}
	}

	REFLECTABLE(PlayerManagerScript, playerHealth, playerMovementSpeed, playerJumpForce);
};