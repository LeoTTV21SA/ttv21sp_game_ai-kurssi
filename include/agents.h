#pragma once
#include <memory>
#include <vector>
#include <map>

namespace agents {
	
	template<typename StateType, typename ActionType> 
	struct PlanNode {
		// 
		StateType state;
		ActionType action;
		float totalGCost = 0.0f;
		float hCost = 0.0f;
		// Linkki edelliseen solmuun
		std::shared_ptr<PlanNode> prevNode = nullptr;
	};

	inline void traverseList(auto node, auto visitFunc) {
		// Rekursion loppuehto
		if (node == 0) { return; }
		// Vieraile loput edelliset solmut rekursiivisesti.
		traverseList(node->prevNode, visitFunc);
		// Vieraile tämä solmu
		visitFunc(node);
	}

	template<typename ActionType>
	ActionType randomPolicy(auto& agent, auto& gameState) {
		return rand() % gameState.actions.size();
	}


	/// <summary>
	/// 
	/// </summary>
	/// <param name="stateNow"></param>
	/// <param name="isGoalFound"></param>
	/// <param name="branchFunc"></param>
	/// <returns></returns>
	template<typename NodeType, typename StateType>
	auto breadthFirstSearch(const StateType& stateNow, auto isGoalFound, auto branchFunc) {
		auto initialState = std::make_shared<NodeType>();
		initialState->state = stateNow;
		initialState->action = -1;
		initialState->prevNode = 0;
		printf("Intial state: %f,%f\n", initialState->state[0], initialState->state[1]);

		// Lisää alkutila open listaan:
		std::vector < std::shared_ptr<NodeType> > openList = { initialState };
		// Closed lista:
		std::map<StateType, std::shared_ptr<NodeType> > closedList;
		// Löydetty maalisolmu:
		std::shared_ptr<NodeType> goaledNode = 0;

		auto isInClosedList = [&closedList](auto node) {
			return closedList.find(node->state) != closedList.end();
		};

		int counter = 0;
		while (goaledNode == 0 && false == openList.empty()) {
			// Poista ensimmäinen solmu open lististä:
			auto currentNode = openList[0];
			openList.erase(openList.begin(), openList.begin() + 1);
			// Tarkista, löytyikö maali:
			if (isGoalFound(currentNode->state)) {
				goaledNode = currentNode; // Tallenna löydetty node
				continue;
			}

			// Laajenna poistettu solmu seuraavaan leveliin käyttäen makeAllActions funktiota:
			auto nextNodes = branchFunc(currentNode);
			// Lisää laajennetut solmu open listaan, jos se ei ole suljetussa listassa:
			for (auto n : nextNodes) {
				if (isInClosedList(n)) {
					continue; // Jatketaa seuraavaan solmuun openListissa, koskapa tämä tila on jo nähty.
				}
				// Jos ei ole closed lista, niin lisää solmu sinne:
				closedList[n->state] = n;
				++counter;
				openList.push_back(n);
			}
			printf("Open list size now: %d, counter:%d\n", int(openList.size()), counter);
		}

		// Jos löytyi maali, niin tulosta suunnitelma kuinka sinne päästään:
		if (goaledNode) {
			printf("Goal found! Plan to get there is following:\n");
			traverseList(goaledNode, [](auto n) {
				printf("Action: %d, State: %f,%f\n", n->action, n->state[0], n->state[1]);
				});
		}
		return goaledNode;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="stateNow"></param>
	/// <param name="isGoalFound"></param>
	/// <param name="branchFunc"></param>
	/// <returns></returns>
	template<typename NodeType, typename StateType>
	auto dijkstra(const StateType& stateNow, auto popBest, auto isGoalFound, auto branchFunc) {
		auto initialState = std::make_shared<NodeType>();
		initialState->state = stateNow;
		initialState->action = -1;
		initialState->prevNode = 0;
		printf("Intial state: %f,%f\n", initialState->state[0], initialState->state[1]);

		// Lisää alkutila open listaan:
		std::vector < std::shared_ptr<NodeType> > openList = { initialState };
		// Closed lista:
		std::map<StateType, std::shared_ptr<NodeType> > closedList;
		// Löydetty maalisolmu:
		std::shared_ptr<NodeType> goaledNode = 0;

		auto isInClosedList = [&closedList](auto node) {
			return closedList.find(node->state) != closedList.end();
		};

		int counter = 0;
		while (goaledNode == 0 && false == openList.empty()) {
			// Poista ensimmäinen solmu open lististä:
			auto currentNode = popBest(openList);
			// Tarkista, löytyikö maali:
			if (isGoalFound(currentNode->state)) {
				goaledNode = currentNode; // Tallenna löydetty node
				continue;
			}

			// Laajenna poistettu solmu seuraavaan leveliin käyttäen makeAllActions funktiota:
			auto nextNodes = branchFunc(currentNode);
			// Lisää laajennetut solmu open listaan, jos se ei ole suljetussa listassa:
			for (auto n : nextNodes) {
				if (isInClosedList(n)) {
					continue; // Jatketaa seuraavaan solmuun openListissa, koskapa tämä tila on jo nähty.
				}
				// Jos ei ole closed lista, niin lisää solmu sinne:
				closedList[n->state] = n;
				++counter;
				openList.push_back(n);
			}
			printf("Open list size now: %d, counter:%d\n", int(openList.size()), counter);
		}

		// Jos löytyi maali, niin tulosta suunnitelma kuinka sinne päästään:
		if (goaledNode) {
			printf("Goal found! Plan to get there is following:\n");
			traverseList(goaledNode, [](auto n) {
				printf("Action: %d, State: %f,%f\n", n->action, n->state[0], n->state[1]);
				});
		}
		return goaledNode;
	}
}
