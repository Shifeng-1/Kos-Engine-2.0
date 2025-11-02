#include "Config/pch.h"
#include "DebugCapsuleColliderRenderSystem.h"
#include "Graphics/GraphicsManager.h"
#include "Resources/ResourceManager.h"

namespace ecs {

    void DebugCapsuleColliderRenderSystem::Init() {}

    void DebugCapsuleColliderRenderSystem::Update() {
        ResourceManager* rm = ResourceManager::GetInstance();
        std::shared_ptr<GraphicsManager> gm = GraphicsManager::GetInstance();

        ECS* ecs = ECS::GetInstance();
        const auto& entities = m_entities.Data();

        for (const EntityID id : entities) {
            TransformComponent* transform = ecs->GetComponent<TransformComponent>(id);
            CapsuleColliderComponent* capsule = ecs->GetComponent<CapsuleColliderComponent>(id);

            if (!transform || !capsule) { continue; }

            glm::vec3 scale = transform->WorldTransformation.scale;
            glm::vec3 center = capsule->capsule.center * scale + transform->WorldTransformation.position;

            glm::mat4 rotationMatrix = glm::mat4{ 1.0f };
            float radiusScale = 1.0f;
            float heightScale = 1.0f;

            switch (capsule->capsule.capsuleDirection) {
            case CapsuleDirection::X:
                rotationMatrix = glm::rotate(glm::mat4{ 1.0f }, glm::radians(90.0f), glm::vec3{ 0.0f, 0.0f, 1.0f });
                radiusScale = std::max(scale.y, scale.z);
                heightScale = scale.x;
                break;
            case CapsuleDirection::Y:
                radiusScale = std::max(scale.x, scale.z);
                heightScale = scale.y;
                break;
            case CapsuleDirection::Z:
                rotationMatrix = glm::rotate(glm::mat4{ 1.0f }, glm::radians(90.0f), glm::vec3{ 1.0f, 0.0f, 0.0f });
                radiusScale = std::max(scale.x, scale.y);
                heightScale = scale.z;
                break;
            }

            glm::mat4 rot = glm::mat4_cast(glm::quat(glm::radians(transform->WorldTransformation.rotation)));

            glm::mat4 result{ 1.0f };
            result = glm::translate(glm::mat4{ 1.0f }, center) * rot * rotationMatrix * glm::scale(glm::mat4(1.0f), glm::vec3{ capsule->capsule.radius * radiusScale, capsule->capsule.height * heightScale, capsule->capsule.radius * radiusScale });

            gm->gm_PushCapsuleDebugData(BasicDebugData{ result });
        }
    }

}