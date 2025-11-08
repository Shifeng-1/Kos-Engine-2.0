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
		int testtest = 0;
		while (openSet.size()) {
			// Max iterations reached, abort algorithm.
			std::cout << "PATHFIND LOOP: " << testtest++ << std::endl;
			if (++iterationCount > maxIterations) {
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
				std::cout << "PATH FOUND\n";
				ReconstructPath(current);
				return true;
			}

			closedSet.push_back(current);

			for (Edge edge : current.edges) {
				neighbour = ((edge.a) == current) ? (edge.b) : (edge.a);
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

					//if (neighbour) {
					//	std::cout << "YES NEIGHBOUR\n";
					//}
					//else {
					//	std::cout << "NO NEIGHBOUR\n";
					//}
					openSet.insert(*neighbour);
 				}
			}
		}

		std::cout << "NO PATH FOUND\n";
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

	float Graph::Heuristic(Node const& a, Node const& b) {
		float result = glm::length2(a.octreeNode.bounds.center - b.octreeNode.bounds.center);
		return result;
	}

	void Graph::AddNode(OctreeNode _octreeNode) {
		for (Node node : nodes) {
			if (node.octreeNode.id == _octreeNode.id) {
				return;
			}
		}

		Node nodeToPush(_octreeNode);

		nodes.push_back(nodeToPush);
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
	}

	Node* Graph::FindNode(OctreeNode* _octreeNode) {
		//std::cout << "RUNNING FIND NODE\n";
		for (Node& node : nodes) {
			if (node.octreeNode.id == _octreeNode->id) {
				return &node;
			}
		}

		return nullptr;
	}

	void Graph::DrawGraph(GraphicsManager* gm) {
		for (Node node : nodes) {
			glm::mat4 model{ 1.f };
			model = glm::translate(model, node.octreeNode.bounds.center) * glm::scale(model, { 0.3f, 0.3f, 0.3f });
			BasicDebugData basicDebug;
			basicDebug.color = glm::vec3(1, 1, 0);
			basicDebug.worldTransform = model;
			gm->gm_PushCubeDebugData(BasicDebugData{ basicDebug });
		}
	}

}