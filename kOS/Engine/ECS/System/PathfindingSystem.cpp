/******************************************************************/
/*!
\file      PathfindingSystem.cpp
\author    Yeo See Kiat Raymond, seekiatraymond.yeo, 2301268
\par       seekiatraymond.yeo@digipen.edu
\date      October 3, 2025
\brief     This file contains the pathfinding system to draw
			Octree wireframe


Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Config/pch.h"
#include "PathfindingSystem.h"
#include "Inputs/Input.h"
#include "Pathfinding/OctreeGrid.h"
#include "Graphics/GraphicsManager.h"

#include <random>

namespace ecs {
	Octrees::Graph waypoints;
	Octrees::Octree octree;
	bool testing = true;
	bool testing2 = false;

	bool test = true;

	float maxTimer = 5.f;
	float currentTimer = 0.f;

	// REMOVE THIS AFTER M2
	int currentPathCount = 0;
	float pathfinderMovementSpeed = 2.5f;
	float proximityCheck = 0.1f;

	void PathfindingSystem::Init() {
		
	}

	void PathfindingSystem::Update() {
		ECS* ecs = ECS::GetInstance();
		const auto& entities = m_entities.Data();

		if (currentTimer < maxTimer) {
			currentTimer += ecs->m_GetDeltaTime();
			//std::cout << "TIMER: " << currentTimer << std::endl;
		}

		for (EntityID id : entities) {
			TransformComponent* trans = ecs->GetComponent<TransformComponent>(id);
			NameComponent* name = ecs->GetComponent<NameComponent>(id);
			//OctreeGeneratorComponent* oct = ecs->GetComponent<OctreeGeneratorComponent>(id);

			if (name->hide) { continue; }

			// Move all pathfinders
			const auto& otherEntities = m_entities.Data();
			for (EntityID otherId : otherEntities) {
				auto* pathfinderTarget = ecs->GetComponent<PathfinderTargetComponent>(otherId);
				auto* pathfinderComp = ecs->GetComponent<PathfinderComponent>(id);
				auto* pathfinderTrans = ecs->GetComponent<TransformComponent>(id);
				auto* pathfinderTargetTrans = ecs->GetComponent<TransformComponent>(otherId);

				if (!pathfinderTarget || !pathfinderComp || !pathfinderTrans || !pathfinderTargetTrans || !pathfinderComp->chase) {
					continue;
				}

				if (currentTimer >= maxTimer) {
					Octrees::OctreeNode closestNodeFrom = octree.FindClosestNode(pathfinderTrans->LocalTransformation.position);
					Octrees::OctreeNode closestNodeTarget = octree.FindClosestNode(pathfinderTargetTrans->LocalTransformation.position);
					octree.graph.AStar(&closestNodeFrom, &closestNodeTarget);

					glm::vec3 directionToMove = octree.graph.pathList[currentPathCount].octreeNode.bounds.center - pathfinderTrans->LocalTransformation.position;
					//pathfinderTrans->LocalTransformation.position += directionToMove * ecs->m_GetDeltaTime() * pathfinderMovementSpeed;


					for (int i = 0; i < octree.graph.pathList.size(); ++i) {
						std::cout << "PATH " << i << ": " << octree.graph.pathList[i].octreeNode.bounds.center.x <<
							octree.graph.pathList[i].octreeNode.bounds.center.y << ", " <<
							octree.graph.pathList[i].octreeNode.bounds.center.z << std::endl;
					}

					//if (std::abs(pathfinderTrans->LocalTransformation.position.x - octree.graph.pathList[currentPathCount].octreeNode.bounds.center.x) < proximityCheck &&
					//	std::abs(pathfinderTrans->LocalTransformation.position.y - octree.graph.pathList[currentPathCount].octreeNode.bounds.center.y) < proximityCheck &&
					//	std::abs(pathfinderTrans->LocalTransformation.position.z - octree.graph.pathList[currentPathCount].octreeNode.bounds.center.z) < proximityCheck &&
					//	currentPathCount < octree.graph.pathList.size()) {
					//	++currentPathCount;
					//}

					currentTimer = 0.f;
				}

				break;
			}

			if (auto* oct = ecs->GetComponent<OctreeGeneratorComponent>(id)) {
				if (oct->drawWireframe) {
					if (testing) {
						testing = false;
						octree = Octrees::Octree(2.f, waypoints);
					}
					//octree = Octrees::Octree(1.f, waypoints);
					octree.root.DrawNode();
					// DRAWING TAKES A LOT OF LATENCY
					octree.graph.DrawGraph();
				}
				else {
					testing = true;
				}
			}

		}
	}
}