#pragma once

#include "ScriptAdapter/TemplateSC.h"

class PlayerScript : public TemplateSC {
public:
    int health;
    int shield;
    bool healthbool;
    std::string shield2;
    glm::vec3 vec3;
    glm::vec4 vec4;
	utility::GUID Targetguid;
	utility::GUID prefabGUID;

    void Start() override {

		ecs::EntityID target = ecsPtr->GetEntityIDFromGUID(Targetguid);

        std::shared_ptr<R_Scene> sceneResource = resource->GetResource<R_Scene>(prefabGUID);

        if (sceneResource) {
            std::cout << entity << std::endl;
            std::string currentScene = ecsPtr->GetSceneByEntityID(entity);
            sceneResource->DuplicatePrefabIntoScene(currentScene);
        }


        health = 1;
        shield = 50;
        physicsPtr->OnCollisionEnter.Add([this](const physics::Collision& col) {
            if (col.thisEntityID != this->entity) { return; }
            std::cout << "Collided with Entity: " << col.otherEntityID << std::endl;
            
            });
    }

    void Update() override {
        // Example: simple movement / regen
        if (shield < 50) shield += 2;

        if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {
            tc->LocalTransformation.position.x += 0.01f;
        }

        if (auto* rb = ecsPtr->GetComponent<ecs::RigidbodyComponent>(entity)) {
            if (Input->IsKeyPressed(keys::SPACE)) physicsPtr->AddForce(rb->actor, glm::vec3{ 0.0f, 100.0f, 0.0f });
        }
    }

    REFLECTABLE(PlayerScript, health, shield, healthbool, shield2, Targetguid, prefabGUID);
};
