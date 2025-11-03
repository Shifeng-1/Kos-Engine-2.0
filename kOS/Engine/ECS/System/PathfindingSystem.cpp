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

	float maxTimer = 3.f;
	float currentTimer = 0.f;

	// REMOVE THIS AFTER M2
	int currentPathCount = 0;
	float proximityCheck = 0.1f;

	void PathfindingSystem::Init() {
		octree.InjectDependency(&m_ecs);
	}

	void PathfindingSystem::Update() {
		const auto& entities = m_entities.Data();

		if (currentTimer < maxTimer) {
			currentTimer += m_ecs.m_GetDeltaTime();
			//std::cout << "TIMER: " << currentTimer << std::endl;
		}

		for (EntityID id : entities) {
			TransformComponent* trans = m_ecs.GetComponent<TransformComponent>(id);
			NameComponent* name = m_ecs.GetComponent<NameComponent>(id);
			//OctreeGeneratorComponent* oct = m_ecs.GetComponent<OctreeGeneratorComponent>(id);

			if (name->hide) { continue; }

			// Move all pathfinders
			const auto& otherEntities = m_entities.Data();
			for (EntityID otherId : otherEntities) {
				if (m_ecs.GetState() != GAMESTATE::RUNNING) {
					continue;
				}

				auto* pathfinderTarget = m_ecs.GetComponent<PathfinderTargetComponent>(otherId);
				auto* pathfinderComp = m_ecs.GetComponent<PathfinderComponent>(id);
				auto* pathfinderTrans = m_ecs.GetComponent<TransformComponent>(id);
				auto* pathfinderTargetTrans = m_ecs.GetComponent<TransformComponent>(otherId);

				Octrees::OctreeNode closestNodeFrom;
				Octrees::OctreeNode closestNodeTarget;

				if (!pathfinderTarget || !pathfinderComp || !pathfinderTrans || !pathfinderTargetTrans || !pathfinderComp->chase) {
					continue;
				}

				if (currentTimer >= maxTimer) {
					//octree = Octrees::Octree(1.f, waypoints);

					closestNodeFrom = octree.FindClosestNode(pathfinderTrans->LocalTransformation.position);
					closestNodeTarget = octree.FindClosestNode(pathfinderTargetTrans->LocalTransformation.position);
					octree.graph.AStar(&closestNodeFrom, &closestNodeTarget);
					currentPathCount = 0;

					//for (int i = 0; i < octree.graph.pathList.size(); ++i) {
					//	std::cout << "PATH " << i << ": " << octree.graph.pathList[i].octreeNode.bounds.center.x << ", " <<
					//		octree.graph.pathList[i].octreeNode.bounds.center.y << ", " <<
					//		octree.graph.pathList[i].octreeNode.bounds.center.z << std::endl;
					//}

					currentTimer = 0.f;
				}

				if (octree.graph.pathList.size() && currentPathCount < octree.graph.pathList.size()) {
					glm::vec3 direction = octree.graph.pathList[currentPathCount].octreeNode.bounds.center - pathfinderTrans->WorldTransformation.position;
					direction = glm::normalize(direction);

					pathfinderTrans->LocalTransformation.position += direction * pathfinderComp->pathfinderMovementSpeed * m_ecs.m_GetDeltaTime();
					if (glm::distance(pathfinderTrans->WorldTransformation.position, octree.graph.pathList[currentPathCount].octreeNode.bounds.center) <= proximityCheck) {
						++currentPathCount;
					}
				}
				else {
					if (closestNodeFrom == closestNodeTarget) {

					}
				}
				


				//if (octree.graph.pathList.size() > 0 && currentPathCount >= octree.graph.pathList.size()) {
				//	break;
				//}

				//if (std::abs(pathfinderTrans->LocalTransformation.position.x - pathfinderTargetTrans->LocalTransformation.position.x) >= proximityCheck ||
				//	std::abs(pathfinderTrans->LocalTransformation.position.y - pathfinderTargetTrans->LocalTransformation.position.y) >= proximityCheck ||
				//	std::abs(pathfinderTrans->LocalTransformation.position.z - pathfinderTargetTrans->LocalTransformation.position.z) >= proximityCheck) {
				//	glm::vec3 directionToMove;

				//	//if ((!octree.graph.pathList.size() && closestNodeFrom == closestNodeTarget) || currentPathCount == octree.graph.pathList.size() - 1) {
				//	//	directionToMove = pathfinderTargetTrans->LocalTransformation.position - pathfinderTrans->LocalTransformation.position;
				//	//}
				//	//else {
				//	//	directionToMove = octree.graph.pathList[currentPathCount].octreeNode.bounds.center - pathfinderTrans->LocalTransformation.position;
				//	//}
				//	directionToMove = octree.graph.pathList[currentPathCount].octreeNode.bounds.center - pathfinderTrans->LocalTransformation.position;

				//	pathfinderTrans->LocalTransformation.position += glm::normalize(directionToMove) * m_ecs.m_GetDeltaTime() * pathfinderComp->pathfinderMovementSpeed;

				//	if (!octree.graph.pathList.size()) {
				//		break;
				//	}

				//	if (std::abs(pathfinderTrans->LocalTransformation.position.x - octree.graph.pathList[currentPathCount].octreeNode.bounds.center.x) < proximityCheck &&
				//		std::abs(pathfinderTrans->LocalTransformation.position.y - octree.graph.pathList[currentPathCount].octreeNode.bounds.center.y) < proximityCheck &&
				//		std::abs(pathfinderTrans->LocalTransformation.position.z - octree.graph.pathList[currentPathCount].octreeNode.bounds.center.z) < proximityCheck) {
				//		++currentPathCount;
				//	}
				//}

				break;
			}

			if (auto* oct = m_ecs.GetComponent<OctreeGeneratorComponent>(id)) {
				if (testing) { // this wont work with DI
					octree = Octrees::Octree(2.f, waypoints, &m_ecs);
					testing = false;
				}

				if (oct->drawWireframe) {
					if (oct->drawBound) octree.root.DrawNode(&m_graphicsManager);
					if (oct->drawNodes) octree.graph.DrawGraph(&m_graphicsManager);
				}
			}

		}
	}
}