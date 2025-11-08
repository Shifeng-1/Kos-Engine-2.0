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

	//float maxTimer = 3.f;
	//float currentTimer = 0.f;

	// REMOVE THIS AFTER M2
	int currentPathCount = 0;
	float proximityCheck = 0.1f;

	void PathfindingSystem::Init() {
		octree.InjectDependency(&m_ecs);
	}

	void PathfindingSystem::Update() {
		// Creating octree
		//const auto& entities = m_entities.Data();

		//EntityID pathfinderTargetID = 9999999;
		//std::vector<EntityID> pathfindersID;

		//for (EntityID id : entities) {
		//	if (auto* octreeGenerator = m_ecs.GetComponent<OctreeGeneratorComponent>(id)) {
		//		// Creating the Octree
		//		if (testing) {
		//			octree = Octrees::Octree(2.f, waypoints, &m_ecs);
		//			testing = false;
		//		}

		//		// Drawing the Octree
		//		if (octreeGenerator->drawWireframe) {
		//			if (octreeGenerator->drawBound) octree.root.DrawNode(&m_graphicsManager);
		//			if (octreeGenerator->drawNodes) octree.graph.DrawGraph(&m_graphicsManager);
		//		}

		//		continue;
		//	}
		//	else if (auto* pathfinderTarget = m_ecs.GetComponent<PathfinderTargetComponent>(id)) {
		//		pathfinderTargetID = id;
		//		continue;
		//	}
		//	else if (auto* pathfinder = m_ecs.GetComponent<PathfinderComponent>(id)) {
		//		pathfindersID.push_back(id);
		//		continue;
		//	}
		//}

		//if (m_ecs.GetState() != GAMESTATE::RUNNING || pathfinderTargetID == 9999999 || pathfindersID.empty()) {
		//	return;
		//}

		//Octrees::OctreeNode closestNodeFrom;
		//Octrees::OctreeNode closestNodeTarget = octree.FindClosestNode(m_ecs.GetComponent<TransformComponent>(pathfinderTargetID)->LocalTransformation.position);

		//for (EntityID pathfinder : pathfindersID) {
		//	auto* pathfinderComp = m_ecs.GetComponent<PathfinderComponent>(pathfinder);
		//	auto* pathfinderTrans = m_ecs.GetComponent<TransformComponent>(pathfinder);
		//	if (pathfinderComp->pathfinderCurrentTimer >= pathfinderComp->pathfinderMaxTimer) {
		//		closestNodeFrom = octree.FindClosestNode(pathfinderTrans->LocalTransformation.position);

		//		// THIS PART IS NOT FOR MULTIPLE PATHFINDERS
		//		octree.graph.AStar(&closestNodeFrom, &closestNodeTarget);
		//		currentPathCount = 0;
		//		//

		//		pathfinderComp->pathfinderCurrentTimer = 0.f;
		//	}

		//	//if (octree.graph.pathList.size() && currentPathCount < octree.graph.pathList.size()) {
		//	//	
		//	//	glm::vec3 direction = octree.graph.pathList[currentPathCount].octreeNode.bounds.center - pathfinderTrans->WorldTransformation.position;
		//	//	direction = glm::normalize(direction);

		//	//	pathfinderTrans->LocalTransformation.position += direction * pathfinderComp->pathfinderMovementSpeed * m_ecs.m_GetDeltaTime();
		//	//	if (glm::distance(pathfinderTrans->WorldTransformation.position, octree.graph.pathList[currentPathCount].octreeNode.bounds.center) <= proximityCheck) {
		//	//		++currentPathCount;
		//	//	}
		//	//}

		//	if (pathfinderComp->pathfinderCurrentTimer < pathfinderComp->pathfinderMaxTimer) {
		//		pathfinderComp->pathfinderCurrentTimer += m_ecs.m_GetDeltaTime();
		//	}
		//}







		//const auto& entities = m_entities.Data();
		//for (EntityID id : entities) {
		//	TransformComponent* trans = m_ecs.GetComponent<TransformComponent>(id);
		//	NameComponent* name = m_ecs.GetComponent<NameComponent>(id);
		//	//OctreeGeneratorComponent* oct = m_ecs.GetComponent<OctreeGeneratorComponent>(id);

		//	if (name->hide) { continue; }

		//	// Move all pathfinders
		//	//const auto& otherEntities = m_entities.Data();
		//	//for (EntityID otherId : otherEntities) {
		//	//	if (m_ecs.GetState() != GAMESTATE::RUNNING) {
		//	//		continue;
		//	//	}

		//	//	auto* pathfinderTarget = m_ecs.GetComponent<PathfinderTargetComponent>(otherId);
		//	//	auto* pathfinderComp = m_ecs.GetComponent<PathfinderComponent>(id);
		//	//	auto* pathfinderTrans = m_ecs.GetComponent<TransformComponent>(id);
		//	//	auto* pathfinderTargetTrans = m_ecs.GetComponent<TransformComponent>(otherId);

		//	//	Octrees::OctreeNode closestNodeFrom;
		//	//	Octrees::OctreeNode closestNodeTarget;

		//	//	if (!pathfinderTarget || !pathfinderComp || !pathfinderTrans || !pathfinderTargetTrans || !pathfinderComp->chase) {
		//	//		continue;
		//	//	}

		//	//	if (currentTimer >= maxTimer) {
		//	//		closestNodeFrom = octree.FindClosestNode(pathfinderTrans->LocalTransformation.position);
		//	//		closestNodeTarget = octree.FindClosestNode(pathfinderTargetTrans->LocalTransformation.position);
		//	//		octree.graph.AStar(&closestNodeFrom, &closestNodeTarget);
		//	//		currentPathCount = 0;

		//	//		currentTimer = 0.f;
		//	//	}

		//	//	if (octree.graph.pathList.size() && currentPathCount < octree.graph.pathList.size()) {
		//	//		glm::vec3 direction = octree.graph.pathList[currentPathCount].octreeNode.bounds.center - pathfinderTrans->WorldTransformation.position;
		//	//		direction = glm::normalize(direction);

		//	//		pathfinderTrans->LocalTransformation.position += direction * pathfinderComp->pathfinderMovementSpeed * m_ecs.m_GetDeltaTime();
		//	//		if (glm::distance(pathfinderTrans->WorldTransformation.position, octree.graph.pathList[currentPathCount].octreeNode.bounds.center) <= proximityCheck) {
		//	//			++currentPathCount;
		//	//		}
		//	//	}
		//	//	else {
		//	//		if (closestNodeFrom == closestNodeTarget) {

		//	//		}
		//	//	}

		//	//	break;
		//	//}

		//	// Octree creation and drawing
		//	if (auto* oct = m_ecs.GetComponent<OctreeGeneratorComponent>(id)) {
		//		if (testing) { // this wont work with DI
		//			octree = Octrees::Octree(2.f, waypoints, &m_ecs);
		//			testing = false;
		//		}

		//		if (oct->drawWireframe) {
		//			if (oct->drawBound) octree.root.DrawNode(&m_graphicsManager);
		//			if (oct->drawNodes) octree.graph.DrawGraph(&m_graphicsManager);
		//		}
		//	}
		//}
	}
}