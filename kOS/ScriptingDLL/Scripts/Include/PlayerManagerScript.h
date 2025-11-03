#include "TemplateSC.h"
//#include "Inputs/Input.h"

class PlayerManagerScript : public TemplateSC {
public:
	int playerHealth;
	float playerMovementSpeed;
	float playerJumpForce;

	float playerCameraSpeedX;
	float playerCameraSpeedY;

	float interactPowerupRange = 5.f;

	utility::GUID armModel;
	ecs::EntityID armModelID;

	float rotationX = 0.f, rotationY = 0.f;
	bool cursorIsHidden = false;

	std::string currentPowerup = "none";

	void Start() override {
		armModelID = ecsPtr->GetEntityIDFromGUID(armModel);
	}

	void Update() override {
		if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {

			glm::vec3 rotationInDegrees(tc->LocalTransformation.rotation);
			glm::vec3 rotationInRad = glm::radians(rotationInDegrees);
			glm::quat q = glm::quat(rotationInRad);

			glm::vec3 forward = q * glm::vec3(0.f, 0.f, 1.f);
			glm::vec3 right = q * glm::vec3(1.f, 0.f, 0.f);

			// Movement Inputs
			if (Input->IsKeyPressed(keys::W)) {
				tc->LocalTransformation.position += glm::normalize(forward) * playerMovementSpeed * ecsPtr->m_GetDeltaTime();
			}

			if (Input->IsKeyPressed(keys::S)) {
				tc->LocalTransformation.position -= glm::normalize(forward) * playerMovementSpeed * ecsPtr->m_GetDeltaTime();
			}

			if (Input->IsKeyPressed(keys::D)) {
				tc->LocalTransformation.position -= glm::normalize(right) * playerMovementSpeed * ecsPtr->m_GetDeltaTime();
			}

			if (Input->IsKeyPressed(keys::A)) {
				tc->LocalTransformation.position += glm::normalize(right) * playerMovementSpeed * ecsPtr->m_GetDeltaTime();
			}

			// First Person Camera
			// THIS IS SUPER CURSED FOR NOW I SWEAR -> HARDCODING THE CAMERA TO BE FIRST CHILD
			if (auto* cc = ecsPtr->GetComponent<ecs::CameraComponent>(ecsPtr->GetComponent<ecs::TransformComponent>(entity)->m_childID[0])) {				
				//std::cout << "CAMERA EXISTS\n";
				
				float mouseRotationX = Input->GetAxisRaw("Mouse Y") * playerCameraSpeedX;
				float mouseRotationY = Input->GetAxisRaw("Mouse X") * playerCameraSpeedY;
				rotationX += mouseRotationX;
				rotationY += mouseRotationY;
				rotationX = glm::clamp(rotationX, -90.f, 90.f);
				auto* cameraTransform = ecsPtr->GetComponent<ecs::TransformComponent>(ecsPtr->GetComponent<ecs::TransformComponent>(entity)->m_childID[0]);
				cameraTransform->LocalTransformation.rotation = glm::vec3(rotationX, rotationY + 90.f, 0.f);
				//std::cout << "CAMERA: " << cameraTransform->LocalTransformation.rotation.x << ", " << cameraTransform->LocalTransformation.rotation.y << std::endl;
				tc->LocalTransformation.rotation = glm::vec3(0.f, -rotationY, 0.f);

				//if (auto* armModelTrans = ecsPtr->GetComponent<TransformComponent>(armModelID)) {
				//	armModelTrans->LocalTransformation.rotation = glm::vec3(0.f, -rotationY, 0.f);
				//}

				// Interact Inputs
				if (Input->IsKeyTriggered(keys::E)) {
					RaycastHit hit;
					hit.entityID = 9999999;

					//glm::vec3 dir = glm::vec3(rotationX, rotationY, 0.f) * forward;
					float yaw = glm::radians(rotationY + 90.f);
					float pitch = glm::radians(rotationX);

					glm::vec3 dir;
					dir.x = std::cos(pitch) * std::cos(yaw);
					dir.y = std::sin(pitch);
					dir.z = cos(pitch) * std::sin(yaw);

					dir = glm::normalize(dir);
					
					physicsPtr->Raycast(tc->LocalTransformation.position, dir, 5.f, hit, ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor);

					if (hit.entityID != 9999999 && ecsPtr->GetComponent<NameComponent>(hit.entityID)->entityTag == "Powerup") {
						if (auto* powerupComp = ecsPtr->GetComponent<PowerupManagerScript>(hit.entityID)) {
							if (powerupComp->powerupType == "fire") {
								if (currentPowerup == "lightning") {
									currentPowerup = "firelightning";
								}
								else if (currentPowerup == "acid") {
									currentPowerup = "fireacid";
								}
								else {
									currentPowerup = "fire";
								}
							}
							else if (powerupComp->powerupType == "lightning") {
								if (currentPowerup == "fire") {
									currentPowerup = "firelightning";
								}
								else if (currentPowerup == "acid") {
									currentPowerup = "lightningacid";
								}
								else {
									currentPowerup = "lightning";
								}
							}
							else if (powerupComp->powerupType == "acid") {
								if (currentPowerup == "fire") {
									currentPowerup = "fireacid";
								}
								else if (currentPowerup == "lightning") {
									currentPowerup = "lightningacid";
								}
								else {
									currentPowerup = "acid";
								}
							}
						}
					}
				}
			}

			// Shooting Inputs
			if (Input->IsKeyTriggered(keys::LMB)) {
				ecs::EntityID bullet = ecsPtr->CreateEntity(ecsPtr->GetSceneByEntityID(entity));
				ecsPtr->GetComponent<NameComponent>(bullet)->entityName = "Bullet";
				ecsPtr->AddComponent<BoxColliderComponent>(bullet);
				ecsPtr->AddComponent<RigidbodyComponent>(bullet);
				ecsPtr->AddComponent<BulletLogic>(bullet);

				if (auto* bulletTransform = ecsPtr->GetComponent<TransformComponent>(bullet)) {
					bulletTransform->LocalTransformation.position = tc->LocalTransformation.position;
					bulletTransform->LocalTransformation.rotation = glm::vec3(360.f - rotationX, 360.f - rotationY, 0.f);
				}

				if (auto* bulletCol = ecsPtr->GetComponent<BoxColliderComponent>(bullet)) {
					bulletCol->isTrigger = true;
				}

				if (auto* bulletRb = ecsPtr->GetComponent<RigidbodyComponent>(bullet)) {
					bulletRb->useGravity = false;
				}
			}

			// Powerup shooting
			if (Input->IsKeyTriggered(keys::RMB)) {
				if (currentPowerup == "fire") {
					ecs::EntityID fireball = ecsPtr->CreateEntity(ecsPtr->GetSceneByEntityID(entity));
					ecsPtr->GetComponent<NameComponent>(fireball)->entityName = "Fireball";
					ecsPtr->AddComponent<SphereColliderComponent>(fireball);
					ecsPtr->AddComponent<RigidbodyComponent>(fireball);
					ecsPtr->AddComponent<FirePowerupManagerScript>(fireball);

					if (auto* fireballTransform = ecsPtr->GetComponent<TransformComponent>(fireball)) {
						fireballTransform->LocalTransformation.position = tc->LocalTransformation.position;
						fireballTransform->LocalTransformation.rotation = glm::vec3(360.f - rotationX, 360.f - rotationY, 0.f);
					}

					if (auto* fireballCol = ecsPtr->GetComponent<SphereColliderComponent>(fireball)) {
						fireballCol->isTrigger = true;
					}

					if (auto* fireballRb = ecsPtr->GetComponent<RigidbodyComponent>(fireball)) {
						fireballRb->useGravity = false;
					}
				}
				else if (currentPowerup == "lightning") {

				}
				else if (currentPowerup == "acid") {

				}
				else if (currentPowerup == "firelightning") {

				}
				else if (currentPowerup == "fireacid") {

				}
				else if (currentPowerup == "lightningacid") {

				}
			}

			// Hide Cursor
			if (Input->IsKeyTriggered(keys::X)) {
				if (cursorIsHidden) {
					Input->HideCursor(false);
					cursorIsHidden = false;
				}
				else {
					Input->HideCursor(true);
					cursorIsHidden = true;
				}
			}
		}
	}

	REFLECTABLE(PlayerManagerScript, playerHealth, playerMovementSpeed, playerJumpForce, playerCameraSpeedX, playerCameraSpeedY, armModel);
};