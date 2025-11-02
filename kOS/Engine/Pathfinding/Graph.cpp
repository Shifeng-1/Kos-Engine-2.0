/******************************************************************/
/*!
\file      Graph.cpp
\author    Yeo See Kiat Raymond, seekiatraymond.yeo, 2301268
\par       seekiatraymond.yeo@digipen.edu
\date      October 3, 2025
\brief     This file contains the list of nodes and edges to access
			and determine the most optimized path through AStar.


Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/********************************************************************/

#include "Graph.h"
#include "OctreeGrid.h"

#include <queue>

namespace Octrees {
	int Node::nextNodeId = 0;

	Node::Node() {
		id = -1;
	}

	Node::Node(OctreeNode _octreeNode) {
		id = nextNodeId++;
		octreeNode = _octreeNode;
		fromID = -1;
	}

	//Edge& Edge::operator=(Edge const& other) {
	//	if (other.a != nullptr && other.b != nullptr) {
	//		a = new Node(*other.a);
	//		b = new Node(*other.b);
	//	}
	//	else {
	//		a = nullptr;
	//		b = nullptr;
	//	}

	//	return *this;
	//}

	//Edge::~Edge() {
	//	if(a)
	//		delete a;

	//	if (b)
	//		delete b;
	//}

	bool Node::operator==(Node const& other) const {
		return id == other.id;
	}

	bool Node::operator==(Node* other) const {
		return id == other->id;
	}

	bool Edge::operator==(Edge const& other) const {
		return ((a == other.a && b == other.b) || (b == other.a && a == other.b));
	}

	int Graph::GetPathLength() {
		return static_cast<int>(pathList.size());
	}

	OctreeNode Graph::GetPathNode(int index) {
		if (pathList.empty()) {
			return OctreeNode();
		}

		if (index < 0 || index >= pathList.size()) {
			return OctreeNode();
		}

		return pathList[index].octreeNode;
	}

	const int maxIterations = 1000;

	bool Graph::AStar(OctreeNode* startNode, OctreeNode* endNode) {
		std::cout << "RUNNING PATHFINDING\n";
		std::cout << "Pathfind Start: " << startNode->bounds.center.x << ", " << startNode->bounds.center.y << ", " << startNode->bounds.center.z << std::endl;
		std::cout << "Pathfind End: " << endNode->bounds.center.x << ", " << endNode->bounds.center.y << ", " << endNode->bounds.center.z << std::endl;

		// Reset path list at the start whenever trying to evaluate another path.
		pathList.clear();

		Node* start = FindNode(startNode);
		Node* end = FindNode(endNode);

		if (!start || !end) {
			return false;
		}

		std::multiset<Node, CompareNode> openSet;
		std::vector<Node> closedSet;

		// The iteration count for when this number increases pass the max iteration and stops the algorithm.
		int iterationCount = 0;

		// Calculate the current node's g, h and f cost.
		start->g = 0;
		start->h = Heuristic(*start, *end);
		start->f = start->g + start->h;
		start->fromID = -1;
		openSet.insert(*start);

		Node *neighbour, current;

		while (openSet.size()) {
			//std::cout << "ITERATION: " << iterationCount << std::endl;

			// Max iterations reached, abort algorithm.
			if (++iterationCount > maxIterations) {
				std::cout << "NO PATH FOUND1\n";
				return false;
			}

			Node smallestFCostNode = *openSet.begin();
			for (Node nod : openSet) {
				if (nod.f < smallestFCostNode.f) {
					smallestFCostNode = nod;
				}
			}

			current = (*openSet.begin());
			openSet.erase(openSet.begin());

			// If current node is the end node, path is found.
			if (current == end) {
				std::cout << "PATH IS FOUND\n";
				ReconstructPath(current);
				return true;
			}

			closedSet.push_back(current);
			//std::cout << std::endl << "NEW CURRENT NODE: " << std::endl;
			for (Edge edge : current.edges) {
				neighbour = ((edge.a) == current) ? (edge.b) : (edge.a);
				
				//std::cout << "CURRENT NODE: " << current.octreeNode.bounds.center.x << ", "
				//	<< current.octreeNode.bounds.center.y << ", "
				//	<< current.octreeNode.bounds.center.z << std::endl;
				//std::cout << "CURRENT EDGE: " << neighbour->octreeNode.bounds.center.x << ", "
				//	<< neighbour->octreeNode.bounds.center.y << ", "
				//	<< neighbour->octreeNode.bounds.center.z << std::endl;

				auto closedSetIt = std::find(closedSet.begin(), closedSet.end(), neighbour);
				if (closedSetIt != closedSet.end()) {
					continue;
				}

				float tentative_gCost = current.g + Heuristic(current, *neighbour);
				auto openSetIt = std::find(openSet.begin(), openSet.end(), neighbour);

				if (tentative_gCost < neighbour->g || openSetIt == openSet.end()) {
					neighbour->g = tentative_gCost;
					neighbour->h = Heuristic(*neighbour, *end);
					neighbour->f = neighbour->g + neighbour->h;
					neighbour->fromID = current.id;

					// I HAVE TO OPTIZMIE THIS NEXT TIME
					for (Node node : nodes) {
						if (node.id == neighbour->id) {
							node.fromID = current.id;
						}
					}

					openSet.insert(*neighbour);
 				}
			}
		}

		std::cout << "NO PATH FOUND2\n";
		return false;
	}

	void Graph::ReconstructPath(Node current) {
		while (current.fromID != -1) {
			pathList.push_back(current);
			//current = current->from;
			// I HAVE TO OPTIMIZAE THIS NEXT TIE
			for (Node node : nodes) {
				if (node.id == current.fromID) {
					current = node;
					break;
				}
			}
		}

		std::reverse(pathList.begin(), pathList.end());
	}

	float Graph::Heuristic(Node a, Node b) {
		float result = glm::length2(a.octreeNode.bounds.center - b.octreeNode.bounds.center);
		return result;
	}

	void Graph::AddNode(OctreeNode _octreeNode) {
		for (Node node : nodes) {
			if (node.octreeNode.id == _octreeNode.id) {
				return;
			}
		}

		//if (edges.size() > 35 && edges.size() < 37) {
		//	std::cout << "EDGE 0 ADDING NODE BEFORE1: " << edges[0].a.octreeNode.bounds.center.x << ", "
		//		<< edges[0].a.octreeNode.bounds.center.y << ", "
		//		<< edges[0].a.octreeNode.bounds.center.z << " to "
		//		<< edges[0].b.octreeNode.bounds.center.x << ", "
		//		<< edges[0].b.octreeNode.bounds.center.y << ", "
		//		<< edges[0].b.octreeNode.bounds.center.z << std::endl;
		//}

		//if (edges.size() > 35 && edges.size() < 37) {
		//	std::cout << "EDGE 0 ADDING NODE BEFORE1: " << edges[0].a->octreeNode.bounds.center.x << ", "
		//		<< edges[0].a->octreeNode.bounds.center.y << ", "
		//		<< edges[0].a->octreeNode.bounds.center.z << " to "
		//		<< edges[0].b->octreeNode.bounds.center.x << ", "
		//		<< edges[0].b->octreeNode.bounds.center.y << ", "
		//		<< edges[0].b->octreeNode.bounds.center.z << std::endl;
		//}

		Node nodeToPush(_octreeNode);

		nodes.push_back(nodeToPush);

		//if (edges.size() > 35 && edges.size() < 37) {
		//	std::cout << "EDGE 0 ADDING NODE AFTER1: " << edges[0].a.octreeNode.bounds.center.x << ", "
		//		<< edges[0].a.octreeNode.bounds.center.y << ", "
		//		<< edges[0].a.octreeNode.bounds.center.z << " to "
		//		<< edges[0].b.octreeNode.bounds.center.x << ", "
		//		<< edges[0].b.octreeNode.bounds.center.y << ", "
		//		<< edges[0].b.octreeNode.bounds.center.z << std::endl;
		//}
		//if (edges.size() > 35 && edges.size() < 37) {
		//	std::cout << "EDGE 0 ADDING NODE BEFORE1: " << edges[0].a->octreeNode.bounds.center.x << ", "
		//		<< edges[0].a->octreeNode.bounds.center.y << ", "
		//		<< edges[0].a->octreeNode.bounds.center.z << " to "
		//		<< edges[0].b->octreeNode.bounds.center.x << ", "
		//		<< edges[0].b->octreeNode.bounds.center.y << ", "
		//		<< edges[0].b->octreeNode.bounds.center.z << std::endl;
		//}
	}

	void Graph::AddEdge(OctreeNode* a, OctreeNode* b) {
		Node* nodeA = FindNode(a);
		Node* nodeB = FindNode(b);

		if (!nodeA || !nodeB) {
			//std::cout << "ADDING EDGE: NULL\n";
			return;
		}

		Edge edge(nodeA, nodeB);

		auto it = std::find(edges.begin(), edges.end(), edge);
		if (it != edges.end()) {
			//std::cout << "ADDING EDGE: ALREADY EXISTS\n";
			return;
		}

		edges.push_back(edge);

		nodeA->edges.push_back(edge);
		nodeB->edges.push_back(edge);



		//if (edges.size() == 36) {
		//	std::cout << "SIZE 36: " << edges[0].a->octreeNode.bounds.center.x << ", "
		//		<< edges[0].a->octreeNode.bounds.center.y << ", "
		//		<< edges[0].a->octreeNode.bounds.center.z << " to "
		//		<< edges[0].b->octreeNode.bounds.center.x << ", "
		//		<< edges[0].b->octreeNode.bounds.center.y << ", "
		//		<< edges[0].b->octreeNode.bounds.center.z << std::endl;
		//}

		//if (edges.size() == 37) {
		//	std::cout << "SIZE 37: " << edges[0].a->octreeNode.bounds.center.x << ", "
		//		<< edges[0].a->octreeNode.bounds.center.y << ", "
		//		<< edges[0].a->octreeNode.bounds.center.z << " to "
		//		<< edges[0].b->octreeNode.bounds.center.x << ", "
		//		<< edges[0].b->octreeNode.bounds.center.y << ", "
		//		<< edges[0].b->octreeNode.bounds.center.z << std::endl;
		//}
	}

	//Node* Graph::FindNode(OctreeNode* _octreeNode) {
	//	for (Node& node : nodes) {
	//		if (node.octreeNode.id == _octreeNode->id) {
	//			return &node;
	//		}
	//	}

	//	return nullptr;
	//}

	Node* Graph::FindNode(OctreeNode* _octreeNode) {
		//std::cout << "RUNNING FIND NODE\n";
		for (Node& node : nodes) {
			if (node.octreeNode.id == _octreeNode->id) {
				return &node;
			}
		}

		return nullptr;
	}

	void Graph::DrawGraph() {
		for (Node node : nodes) {
			std::shared_ptr<GraphicsManager> gm = GraphicsManager::GetInstance();
			glm::mat4 model{ 1.f };
			model = glm::translate(model, node.octreeNode.bounds.center) * glm::scale(model, { 0.3f, 0.3f, 0.3f });
			BasicDebugData basicDebug;
			basicDebug.worldTransform = model;
			gm->gm_PushCubeDebugData(BasicDebugData{ basicDebug });
		}
	}

}