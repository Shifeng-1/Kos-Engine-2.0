#include "TemplateSC.h"
//#include "Inputs/Input.h"

class PlayerManagerScript : public TemplateSC {
public:
	int playerHealth;
	float playerMovementSpeed;
	float playerCrouchingSpeed;
	float playerJumpForce;

	float playerCameraSpeedX;
	float playerCameraSpeedY;

	float interactPowerupRange = 5.f;

	utility::GUID creationPoint;
	ecs::EntityID creationPointID;
	utility::GUID cameraObject;
	ecs::EntityID cameraObjectID;
	utility::GUID armModel;
	ecs::EntityID armModelID;
	utility::GUID groundCheck;
	ecs::EntityID groundCheckID;

	utility::GUID bulletPrefab;
	utility::GUID fireballPrefab;
	utility::GUID lightningStrikePrefab;
	utility::GUID acidBlastPrefab;
	utility::GUID groundSpikesPrefab;
	utility::GUID flamethrowerPrefab;
	utility::GUID starfallPrefab;

	float rotationX = 0.f, rotationY = 0.f;
	bool cursorIsHidden = false;
	glm::vec3 cameraFacingDirection;

	std::string currentPowerup = "none";

	ecs::TransformComponent* cameraTransform;
	bool isGrounded = false;

	float cameraStandingHeight, cameraCrouchingHeight, cameraSlidingHeight;

	bool playerIsRunning = false, playerIsSliding = false;
	float playerCurrentMovementSpeed;

	// JUST FOR TESTING, DELETE AFTERWARDS


	void Start() override {
		creationPointID = ecsPtr->GetEntityIDFromGUID(creationPoint);
		cameraObjectID = ecsPtr->GetEntityIDFromGUID(cameraObject);
		armModelID = ecsPtr->GetEntityIDFromGUID(armModel);
		groundCheckID = ecsPtr->GetEntityIDFromGUID(groundCheck);

		cameraTransform = ecsPtr->GetComponent<ecs::TransformComponent>(cameraObjectID);
		cameraStandingHeight = cameraTransform->LocalTransformation.position.y;
		cameraCrouchingHeight = cameraStandingHeight * 0.5f;
		cameraSlidingHeight = cameraStandingHeight * 0.3f;

		playerCurrentMovementSpeed = playerMovementSpeed;
	}

	void Update() override {
		if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {

			isGrounded = ecsPtr->GetComponent<GroundCheckScript>(groundCheckID)->groundCheck;

			glm::vec3 rotationInDegrees(tc->LocalTransformation.rotation);
			glm::vec3 rotationInRad = glm::radians(rotationInDegrees);
			glm::quat q = glm::quat(rotationInRad);

			glm::vec3 forward = q * glm::vec3(0.f, 0.f, 1.f);
			glm::vec3 right = q * glm::vec3(1.f, 0.f, 0.f);

			// Movement Inputs
			if (Input->IsKeyPressed(keys::W)) {
				if (Input->IsKeyPressed(keys::LeftShift)) {
					tc->LocalTransformation.position += glm::normalize(forward) * playerCurrentMovementSpeed * ecsPtr->m_GetDeltaTime() * 1.75f;
					//physicsPtr->AddForce(ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor, glm::normalize(forward) * playerCurrentMovementSpeed * 1.45f, ForceMode::VelocityChange);
					playerIsRunning = true;
				}
				else {
					tc->LocalTransformation.position += glm::normalize(forward) * playerCurrentMovementSpeed * ecsPtr->m_GetDeltaTime();
					//physicsPtr->AddForce(ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor, glm::normalize(forward) * playerCurrentMovementSpeed, ForceMode::VelocityChange);
				}
			}

			if (Input->IsKeyReleased(keys::LeftShift)) {
				playerIsRunning = false;
			}

			if (Input->IsKeyPressed(keys::S)) {
				tc->LocalTransformation.position -= glm::normalize(forward) * playerCurrentMovementSpeed * ecsPtr->m_GetDeltaTime();
				//physicsPtr->AddForce(ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor, -glm::normalize(forward) * playerCurrentMovementSpeed);
			}

			if (Input->IsKeyPressed(keys::D)) {
				tc->LocalTransformation.position -= glm::normalize(right) * playerCurrentMovementSpeed * ecsPtr->m_GetDeltaTime();
				//physicsPtr->AddForce(ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor, -glm::normalize(right) * playerCurrentMovementSpeed);
			}

			if (Input->IsKeyPressed(keys::A)) {
				tc->LocalTransformation.position += glm::normalize(right) * playerCurrentMovementSpeed * ecsPtr->m_GetDeltaTime();
				//physicsPtr->AddForce(ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor, glm::normalize(right) * playerCurrentMovementSpeed);
			}

			if (Input->IsKeyTriggered(keys::SPACE) && isGrounded) {
				physicsPtr->AddForce(ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor, { 0.f, playerJumpForce, 0.f }, ForceMode::Impulse);
			}

			if (Input->IsKeyPressed(keys::LeftControl) && isGrounded) {
				if (playerIsRunning) {
					cameraTransform->LocalTransformation.position.y = cameraSlidingHeight;
				}
				else {
					cameraTransform->LocalTransformation.position.y = cameraCrouchingHeight;
					playerCurrentMovementSpeed = playerCrouchingSpeed;
				}
			}
			else if (Input->IsKeyReleased(keys::LeftControl)) {
				cameraTransform->LocalTransformation.position.y = cameraStandingHeight;
				playerCurrentMovementSpeed = playerMovementSpeed;
			}
			
			// First Person Camera
			// THIS IS SUPER CURSED FOR NOW I SWEAR -> HARDCODING THE CAMERA TO BE FIRST CHILD
			//if (auto* cc = ecsPtr->GetComponent<ecs::CameraComponent>(ecsPtr->GetComponent<ecs::TransformComponent>(entity)->m_childID[0])) {	
			if (auto* cc = ecsPtr->GetComponent<ecs::CameraComponent>(cameraObjectID)) {
				//std::cout << "CAMERA EXISTS\n";
				
				float mouseRotationX = Input->GetAxisRaw("Mouse Y") * playerCameraSpeedX;
				float mouseRotationY = Input->GetAxisRaw("Mouse X") * playerCameraSpeedY;
				rotationX += mouseRotationX;
				rotationY += mouseRotationY;
				rotationX = glm::clamp(rotationX, -90.f, 90.f);
				//auto* cameraTransform = ecsPtr->GetComponent<ecs::TransformComponent>(cameraObjectID);
				cameraTransform->LocalTransformation.rotation = glm::vec3(rotationX, rotationY + 90.f, 0.f);
				//std::cout << "CAMERA: " << cameraTransform->LocalTransformation.rotation.x << ", " << cameraTransform->LocalTransformation.rotation.y << std::endl;
				tc->LocalTransformation.rotation = glm::vec3(0.f, -rotationY, 0.f);

				if (auto* armModelTrans = ecsPtr->GetComponent<TransformComponent>(armModelID)) {
					armModelTrans->LocalTransformation.rotation.x = -rotationX;
				}

				// Interact Inputs
				if (Input->IsKeyTriggered(keys::E)) {
					std::cout << "Pressing E\n";
					RaycastHit hit;
					hit.entityID = 9999999;

					//float yaw = glm::radians(rotationY + 90.f);
					//float pitch = glm::radians(rotationX);

					//glm::vec3 dir;
					//dir.x = std::cos(pitch) * std::cos(yaw);
					//dir.y = std::sin(pitch);
					//dir.z = cos(pitch) * std::sin(yaw);

					//dir = glm::normalize(dir);

					glm::vec3 dir = GetCameraFacingDirection();
					
					physicsPtr->Raycast(cameraTransform->WorldTransformation.position, dir, 5.f, hit, ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor);
					//ecs::EntityID test = ecsPtr->CreateEntity(ecsPtr->GetSceneByEntityID(entity));
					//ecsPtr->GetComponent<NameComponent>(test)->entityName = "Test";
					//ecsPtr->AddComponent<BoxColliderComponent>(test);
					//ecsPtr->GetComponent<BoxColliderComponent>(test)->isTrigger = true;
					//ecsPtr->GetComponent<TransformComponent>(test)->LocalTransformation.position = cameraTransform->WorldTransformation.position + dir * 5.f;

					if (hit.entityID != 9999999 && ecsPtr->GetComponent<NameComponent>(hit.entityID)->entityTag == "Powerup") {

						if (auto* powerupComp = ecsPtr->GetComponent<PowerupManagerScript>(hit.entityID)) {
							if (powerupComp->powerupType == "fire") {
								if (currentPowerup == "lightning") {
									currentPowerup = "firelightning";
								}
								else if (currentPowerup == "acid") {
									currentPowerup = "fireacid";
								}
								else if(currentPowerup == "none") {
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
								else if (currentPowerup == "none") {
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
								else if (currentPowerup == "none") {
									currentPowerup = "acid";
								}
							}

							std::cout << "CURRENT POWERUP: " << currentPowerup << std::endl;
						}
					}
				}
			}

			// Shooting Inputs
			if (Input->IsKeyTriggered(keys::LMB)) {
				std::shared_ptr<R_Scene> bullet = resource->GetResource<R_Scene>(bulletPrefab);

				if (bullet) {
					std::string currentScene = ecsPtr->GetSceneByEntityID(entity);
					//ecs::EntityID bulletID = bullet->DuplicatePrefabIntoScene(currentScene);
					ecs::EntityID bulletID = DuplicatePrefabIntoScene<R_Scene>(currentScene, bulletPrefab);

					if (auto* bulletTransform = ecsPtr->GetComponent<TransformComponent>(bulletID)) {
						bulletTransform->LocalTransformation.position = ecsPtr->GetComponent<TransformComponent>(creationPointID)->WorldTransformation.position;
					}

					if (auto* bulletScript = ecsPtr->GetComponent<BulletLogic>(bulletID)) {
						bulletScript->direction = GetCameraFacingDirection();
					}
				}
			}

			// Powerup shooting
			if (Input->IsKeyTriggered(keys::RMB)) {
				if (currentPowerup == "fire") {
					std::shared_ptr<R_Scene> fireball = resource->GetResource<R_Scene>(fireballPrefab);

					if (fireball) {
						std::string currentScene = ecsPtr->GetSceneByEntityID(entity);
						//ecs::EntityID fireballID = fireball->DuplicatePrefabIntoScene(currentScene);
						ecs::EntityID fireballID = DuplicatePrefabIntoScene<R_Scene>(currentScene, fireballPrefab);

						if (auto* fireballTransform = ecsPtr->GetComponent<TransformComponent>(fireballID)) {
							fireballTransform->LocalTransformation.position = ecsPtr->GetComponent<TransformComponent>(creationPointID)->WorldTransformation.position;
						}

						if (auto* fireballScript = ecsPtr->GetComponent<FirePowerupManagerScript>(fireballID)) {
							fireballScript->direction = GetCameraFacingDirection();
						}

						currentPowerup = "none";
					}
				}
				else if (currentPowerup == "lightning") {
					std::shared_ptr<R_Scene> lightningStrike = resource->GetResource<R_Scene>(lightningStrikePrefab);

					if (lightningStrike) {
						std::string currentScene = ecsPtr->GetSceneByEntityID(entity);
						//ecs::EntityID lightningStrikeID = lightningStrike->DuplicatePrefabIntoScene(currentScene);
						ecs::EntityID lightningStrikeID = DuplicatePrefabIntoScene<R_Scene>(currentScene, lightningStrikePrefab);

						if (auto* lightningStrikeTransform = ecsPtr->GetComponent<TransformComponent>(lightningStrikeID)) {
							RaycastHit hit;
							hit.entityID = 9999999;
							glm::vec3 dir = GetCameraFacingDirection();

							float range = ecsPtr->GetComponent<LightningPowerupManagerScript>(ecsPtr->GetComponent<TransformComponent>(lightningStrikeID)->m_childID[0])->range;
							physicsPtr->Raycast(cameraTransform->WorldTransformation.position, dir, range, hit, ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor);

							if (hit.entityID != 9999999) {
								lightningStrikeTransform->LocalTransformation.position = hit.point;

								currentPowerup = "none";
							}

						}
					}
				}
				else if (currentPowerup == "acid") {
					std::shared_ptr<R_Scene> acidBlast = resource->GetResource<R_Scene>(acidBlastPrefab);

					if (acidBlast) {
						std::string currentScene = ecsPtr->GetSceneByEntityID(entity);
						//ecs::EntityID acidBlastID = acidBlast->DuplicatePrefabIntoScene(currentScene);
						ecs::EntityID acidBlastID = DuplicatePrefabIntoScene<R_Scene>(currentScene, acidBlastPrefab);

						if (auto* acidBlastTransform = ecsPtr->GetComponent<TransformComponent>(acidBlastID)) {
							acidBlastTransform->LocalTransformation.position = ecsPtr->GetComponent<TransformComponent>(creationPointID)->WorldTransformation.position;
						}

						if (auto* acidBlastScript = ecsPtr->GetComponent<AcidPowerupManagerScript>(acidBlastID)) {
							acidBlastScript->direction = GetCameraFacingDirection();
						}

						currentPowerup = "none";
					}
				}
				else if (currentPowerup == "firelightning") {
					std::shared_ptr<R_Scene> groundSpikes = resource->GetResource<R_Scene>(groundSpikesPrefab);

					if (groundSpikes) {
						std::string currentScene = ecsPtr->GetSceneByEntityID(entity);
						//ecs::EntityID groundSpikesID = groundSpikes->DuplicatePrefabIntoScene(currentScene);
						ecs::EntityID groundSpikesID = DuplicatePrefabIntoScene<R_Scene>(currentScene, groundSpikesPrefab);

						if (auto* groundSpikesTransform = ecsPtr->GetComponent<TransformComponent>(groundSpikesID)) {
							groundSpikesTransform->LocalTransformation.position = ecsPtr->GetComponent<TransformComponent>(entity)->WorldTransformation.position;
						}

						currentPowerup = "none";
					}
				}
				else if (currentPowerup == "fireacid") {
					std::shared_ptr<R_Scene> flamethrower = resource->GetResource<R_Scene>(flamethrowerPrefab);

					if (flamethrower) {
						std::string currentScene = ecsPtr->GetSceneByEntityID(entity);
						//ecs::EntityID flamethrowerID = flamethrower->DuplicatePrefabIntoScene(currentScene);
						ecs::EntityID flamethrowerID = DuplicatePrefabIntoScene<R_Scene>(currentScene, flamethrowerPrefab);

						if (auto* flamethrowerTransform = ecsPtr->GetComponent<TransformComponent>(flamethrowerID)) {
							flamethrowerTransform->LocalTransformation.position = ecsPtr->GetComponent<TransformComponent>(creationPointID)->WorldTransformation.position;
							//flamethrowerTransform.
							ecsPtr->SetParent(creationPointID, flamethrowerID);
						}

						currentPowerup = "none";
					}
				}
				else if (currentPowerup == "lightningacid") {
					std::shared_ptr<R_Scene> starfall = resource->GetResource<R_Scene>(starfallPrefab);

					if (starfall) {
						std::string currentScene = ecsPtr->GetSceneByEntityID(entity);
						//ecs::EntityID starfallID = starfall->DuplicatePrefabIntoScene(currentScene);
						ecs::EntityID starfallID = DuplicatePrefabIntoScene<R_Scene>(currentScene, starfallPrefab);

						if (auto* starfallTransform = ecsPtr->GetComponent<TransformComponent>(starfallID)) {
							starfallTransform->LocalTransformation.position = ecsPtr->GetComponent<TransformComponent>(creationPointID)->WorldTransformation.position;
						}

						if (auto* starfallScript = ecsPtr->GetComponent<AcidPowerupManagerScript>(starfallID)) {
							starfallScript->direction = GetCameraFacingDirection();
						}

						currentPowerup = "none";
					}
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

			// Spawn another enemy
			if (Input->IsKeyTriggered(keys::Z)) {

			}
		}
	}

	glm::vec3 GetCameraFacingDirection() {
		float yaw = glm::radians(rotationY + 90.f);
		float pitch = glm::radians(rotationX);

		glm::vec3 dir;
		dir.x = std::cos(pitch) * std::cos(yaw);
		dir.y = std::sin(pitch);
		dir.z = cos(pitch) * std::sin(yaw);

		dir = glm::normalize(dir);

		return dir;
	}

	REFLECTABLE(PlayerManagerScript, playerHealth, playerMovementSpeed, playerCrouchingSpeed, playerJumpForce, playerCameraSpeedX,
		playerCameraSpeedY, creationPoint, cameraObject, armModel, groundCheck, bulletPrefab, fireballPrefab, lightningStrikePrefab,
		acidBlastPrefab, groundSpikesPrefab, starfallPrefab);
};