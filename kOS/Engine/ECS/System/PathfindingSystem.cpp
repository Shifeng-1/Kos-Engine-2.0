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

	float maxTimer = 1.f;
	float currentTimer = 0.f;

	void PathfindingSystem::Init() {
		
	}

	void PathfindingSystem::Update(const std::string& scene) {
		ECS* ecs = ECS::GetInstance();
		const auto& entities = m_entities.Data();
		for (EntityID id : entities) {
			TransformComponent* trans = ecs->GetComponent<TransformComponent>(id);
			NameComponent* name = ecs->GetComponent<NameComponent>(id);
			OctreeGeneratorComponent* oct = ecs->GetComponent<OctreeGeneratorComponent>(id);

			if (!oct || oct->scene != scene || !trans || trans->scene != scene || name->hide) { continue; }

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

		// THIS IS CAUSING A LOT OF LAG AND IDK WHY
		//Octrees::OctreeNode currentNodePos = octree.GetClosestNode({ 0.f, 0.f, 0.f });

		//std::shared_ptr<GraphicsManager> gm = GraphicsManager::GetInstance();
		//glm::mat4 model{ 1.f };
		//model = glm::translate(model, currentNodePos.bounds.center) * glm::scale(model, { 0.3f, 0.3f, 0.3f });
		//BasicDebugData basicDebug;
		//basicDebug.worldTransform = model;
		//gm->gm_PushCubeDebugData(BasicDebugData{ basicDebug });

		if (!testing2) {
			//octree.graph.AStar(currentNodePos, octree.graph.nodes[0].octreeNode);

			//std::cout << std::endl << "TEST1: \n";
			//octree.graph.AStar(&octree.graph.nodes[0].octreeNode, &octree.graph.nodes[1].octreeNode);
			//std::cout << std::endl << "TEST2: ";
			//octree.graph.AStar(&octree.graph.nodes[1].octreeNode, &octree.graph.nodes[2].octreeNode);
			//std::cout << std::endl << "TEST3: ";
			//octree.graph.AStar(&octree.graph.nodes[1].octreeNode, &octree.graph.nodes[3].octreeNode);
			//std::cout << std::endl << "TEST4: ";
			//octree.graph.AStar(&octree.graph.nodes[1].octreeNode, &octree.graph.nodes[4].octreeNode);
			//std::cout << std::endl << "TEST5: ";
			//octree.graph.AStar(&octree.graph.nodes[1].octreeNode, &octree.graph.nodes[5].octreeNode);

			//std::cout << octree.graph.pathList.size() << std::endl;
			//for (int i = 0; i < octree.graph.pathList.size(); ++i) {
			//	std::cout << "PATH LIST " << i << ": " << octree.graph.pathList[i].octreeNode.bounds.center.x << ", " << octree.graph.pathList[i].octreeNode.bounds.center.y << ", " << octree.graph.pathList[i].octreeNode.bounds.center.z << std::endl;
			//}
			//for (Octrees::Node no : octree.graph.nodes) {
			//	std::cout << "EDGES: " << no.edges.size() << std::endl;
			//}

			testing2 = true;
		}

		//if (testing2) {
		//	std::shared_ptr<GraphicsManager> gm = GraphicsManager::GetInstance();
		//	glm::mat4 model{ 1.f };
		//	model = glm::translate(model, octree.graph.nodes[0].octreeNode.bounds.center) * glm::scale(model, {0.5f, 0.5f, 0.5f});
		//	BasicDebugData basicDebug;
		//	basicDebug.worldTransform = model;
		//	gm->gm_PushCubeDebugData(BasicDebugData{ basicDebug });

		//	glm::mat4 model2{ 1.f };
		//	model2 = glm::translate(model2, octree.graph.nodes[1].octreeNode.bounds.center) * glm::scale(model2, { 0.25f, 0.25f, 0.25f });
		//	BasicDebugData basicDebug2;
		//	basicDebug2.worldTransform = model2;
		//	gm->gm_PushCubeDebugData(BasicDebugData{ basicDebug2 });
		//}
	}
}