#pragma once

/******************************************************************/
/*!
\file      Graph.h
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

#ifndef GRAPH_H
#define GRAPH_H

#include "Config/pch.h"
#include "Graphics/GraphicsManager.h"

#include "OctreeNode.h"

namespace Octrees {
	struct Edge; 

	struct Node {
		static int nextNodeId;
		int id;

		float f = 0.f, g = 0.f, h = 0.f;
		//Node* from;
		int fromID = -1;

		std::vector<Edge> edges;
		OctreeNode octreeNode;

		Node();
		Node(OctreeNode _octreeNode);
		bool operator==(Node const& other) const;
		bool operator==(Node* other) const;
	};

	struct Edge {
		//Node a, b;
		Node* a, *b;

		//Edge& operator=(Edge const& other);
		bool operator==(Edge const& other) const;

		//~Edge();
	};

	struct Graph {
		std::vector<Edge> edges;
		std::vector<Node> nodes;
		

		std::vector<Node> pathList;

		int GetPathLength();
		OctreeNode GetPathNode(int index);

		bool AStar(OctreeNode* startNode, OctreeNode* endNode);
		bool AStarGround(OctreeNode* startNode, OctreeNode* endNode);
		void ReconstructPath(Node current);
		float Heuristic(Node const& a, Node const& b);

		void AddNode(OctreeNode _octreeNode);
		//void AddEdge(OctreeNode a, OctreeNode b);
		void AddEdge(OctreeNode* a, OctreeNode* b);
		//Node* FindNode(OctreeNode _octreeNode);
		Node* FindNode(OctreeNode* _octreeNode);

		void DrawGraph(GraphicsManager* gm);

		//struct CompareNode {
		//	bool operator()(Node* a, Node* b) const {
		//		return a->f < b->f;
		//	}
		//};

		struct CompareNode {
			bool operator()(Node const& a, Node const& b) const {
				return a.f < b.f;
			}
		};
	};
}

#endif