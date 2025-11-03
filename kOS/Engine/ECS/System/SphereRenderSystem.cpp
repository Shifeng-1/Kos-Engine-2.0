#include "Config/pch.h"
#include "ECS/ECS.h"
#include "SphereRenderSystem.h"
#include "Graphics/GraphicsManager.h"
#include "ECS/Component/MeshRendererComponent.h"
#include "ECS/Component/TransformComponent.h"
#include "ECS/Component/NameComponent.h"

namespace ecs {

    void SphereRenderSystem::Init()
    {
        // Initialize static mesh rendering resources if needed
    }

    void SphereRenderSystem::Update()
    {
        ECS* ecs = ECS::GetInstance();
        std::shared_ptr<GraphicsManager> gm = GraphicsManager::GetInstance();
        ResourceManager* rm = ResourceManager::GetInstance();
        const auto& entities = m_entities.Data();

        for (const EntityID id : entities) {
            TransformComponent* transform = ecs->GetComponent<TransformComponent>(id);
            MaterialComponent* matRenderer = ecs->GetComponent<MaterialComponent>(id);
            if (!ecs->HasComponent<SphereRendererComponent>(id))continue;
            glm::mat4 model = transform->transformation;
            if (ecs->HasComponent<SphereColliderComponent>(id)) {
                //Base scale on this instead
                SphereColliderComponent* sphere = ecs->GetComponent<SphereColliderComponent>(id);

                glm::vec3 scale = transform->WorldTransformation.scale;
                glm::vec3 size = sphere->sphere.radius* scale;
                glm::vec3 center = sphere->sphere.center * scale + transform->WorldTransformation.position;
                model = glm::mat4{ 1.f };
                model = glm::translate(model, center) * glm::mat4_cast(glm::quat(glm::radians(transform->WorldTransformation.rotation))) * glm::scale(model, size);

            }
            std::shared_ptr<R_Material> mat = rm->GetResource<R_Material>(matRenderer->materialGUID);
            if (!mat)return;;
            std::shared_ptr<R_Texture> diff = rm->GetResource<R_Texture>(mat->md.diffuseMaterialGUID);
            std::shared_ptr<R_Texture> spec = rm->GetResource<R_Texture>(mat->md.specularMaterialGUID);
            std::shared_ptr<R_Texture> norm = rm->GetResource<R_Texture>(mat->md.normalMaterialGUID);
            std::shared_ptr<R_Texture> ao = rm->GetResource<R_Texture>(mat->md.ambientOcclusionMaterialGUID);
            std::shared_ptr<R_Texture> rough = rm->GetResource<R_Texture>(mat->md.roughnessMaterialGUID);
            gm->gm_PushSphereData(SphereRenderer::SphereData{ PBRMaterial(diff,spec,rough,ao,norm),model,id });

        }

    }

}
