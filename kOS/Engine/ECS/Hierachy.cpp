
#include "Config/pch.h"
#include "Hierachy.h"
#include "Scene/SceneManager.h"
#include "Utility/MathUtility.h"

namespace hierachy {

	void m_SetParent(EntityID parent, EntityID child, bool updateTransform) {

		ECS* ecs = ECS::GetInstance();

		m_RemoveParent(child);

		TransformComponent* parentTransform = ecs->GetComponent<TransformComponent>(parent);
		//checks if child is already in parent
		if (GetParent(child).has_value()) {
			return;
		}

		//checks if parent is getting dragged into its child
		EntityID id = parent;
		while (GetParent(id).has_value()) {
			EntityID checkParentid = GetParent(id).value();
			if (checkParentid == child) {
				LOGGING_WARN("Cannot assign parent to its own child");
				return;
			}
			id = checkParentid;

		}

		parentTransform->m_childID.push_back(child);

		TransformComponent* childTransform = ecs->GetComponent<TransformComponent>(child);
		childTransform->m_haveParent = true;
		childTransform->m_parentID = parent;
		// Recalculate Local Transform after parenting
		if (updateTransform) {
			childTransform->localTransform = glm::inverse(parentTransform->transformation) * childTransform->transformation;
			math::DecomposeMtxIntoTRS(childTransform->localTransform, childTransform->LocalTransformation.position, childTransform->LocalTransformation.rotation, childTransform->LocalTransformation.scale);
		}
	}

	void m_RemoveParent(EntityID child) {
		// removes id from both the child and the parents vector
		ECS* ecs = ECS::GetInstance();

		if (!GetParent(child).has_value()) {
			// does not have parrent
			return;
		}

		EntityID parent = GetParent(child).value();
		TransformComponent* parentTransform = ecs->GetComponent<TransformComponent>(parent);
		size_t pos{};
		for (EntityID& id : parentTransform->m_childID) {
			if (child == id) {
				parentTransform->m_childID.erase(parentTransform->m_childID.begin() + pos);
				break;
			}
			pos++;
		}

		TransformComponent* childTransform = ecs->GetComponent<TransformComponent>(child);
		childTransform->m_haveParent = false;
		childTransform->m_parentID = 0;
		// Updating Transformation Mtxs
		childTransform->localTransform = childTransform->transformation;
		math::DecomposeMtxIntoTRS(childTransform->localTransform, childTransform->LocalTransformation.position, childTransform->LocalTransformation.rotation, childTransform->LocalTransformation.scale);
	}

	std::optional<EntityID> GetParent(EntityID child)
	{
		ECS* ecs = ECS::GetInstance();
		TransformComponent* childTransform = ecs->GetComponent<TransformComponent>(child);
		if (!childTransform || !childTransform->m_haveParent) {
			return std::optional<EntityID>();
		}

		return childTransform->m_parentID;

	}

	std::optional<std::vector<EntityID>>m_GetChild(EntityID parent)
	{
		ECS* ecs = ECS::GetInstance();
		TransformComponent* parentTransform = ecs->GetComponent<TransformComponent>(parent);
		if (parentTransform->m_childID.size() <= 0) {
			return std::optional<std::vector<EntityID>>();
		}

		return parentTransform->m_childID;

	}

	void m_UpdateChildScene(EntityID parent)
	{
		ECS* ecs = ECS::GetInstance();
		scenes::SceneManager* scenemanager = scenes::SceneManager::m_GetInstance();
		std::string parentscene = ecs->GetSceneByEntityID(parent);
		const auto& child = m_GetChild(parent);

		if (child.has_value()) {
			auto& children = child.value();

			for (auto& childid : children) {
				//if child scene does not belong to parent scene change scene
				//create a update child function that recurse	
				std::string childscene = ecs->GetSceneByEntityID(childid);
				if (parentscene != childscene) {
					scenemanager->SwapScenes(childscene, parentscene, childid);
				}
				//check if child have more children and call a recursion
				if (m_GetChild(childid).has_value()) {
					m_UpdateChildScene(childid);
				}


			}


		}


		

	}

}