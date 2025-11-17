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
            //sceneResource->DuplicatePrefabIntoScene(currentScene);

            DuplicatePrefabIntoScene<R_Scene>(currentScene, prefabGUID);
        }




        health = 1;
        shield = 50;
        
        physicsPtr->GetEventCallback()->OnCollisionEnter(entity, [this](const physics::Collision& collision) {
            std::cout << "Hit entity: " << collision.otherEntityID << std::endl;
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

        //RaycastHit hit;
        //if (physicsPtr->Raycast({ 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, 10.f, hit)) {
        //    hit.entityID;
        //}
    }

    REFLECTABLE(PlayerScript, health, shield, healthbool, shield2, Targetguid, prefabGUID);
};
