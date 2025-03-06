#include <memory>
#include <stdio.h>
#include <vector>
#include <functional>
#include <assert.h>
#include <agents.h>
#include <cmath>
#include <string>

namespace find_number {
	// Forward declarations and some typedefs
	struct Agent;
	struct Game;
	typedef int ActionType;
	typedef std::string EventType;

	// Politiikkafunktion määrittely: f(Agent&, const Game&) -> ActionId
	typedef std::function<ActionType(Agent&, const Game&)>	PolicyFunc;
	typedef std::function<void(Agent&, const EventType&)>	EventFunc;
	typedef std::function<EventType(Game&, Agent&)>			ActionFunc;

	// Agentin tila
	struct Agent {
		float		position	= 0.0f;
		PolicyFunc	policy		= 0;
		EventFunc	event		= 0;
	};

	// Pelin tila (+ actionit)
	struct Game {
		typedef agents::PlanNode<float, int> NodeType;
		std::vector<std::shared_ptr<NodeType>> agentHistories;

		std::vector<Agent>		agents;
		std::vector<int>		map;
		float					goal;
		std::vector<ActionFunc>	actions = {
			// Kävely oikealle
			[](Game& game, Agent& agent) {
				agent.position += 1;
				return EventType();
			},
			// Kävely vasemmalle
			[](Game& game, Agent& agent) {
				agent.position -= 1;
				return EventType();
			}
		};
	};

	// Onko peli loppu?
	bool isEnd(const Game& gameState) {
		bool ended = false;
		// Tarkista, onko joku agentti maalissa (toleranssi 0.01f):
		for(const auto& agent : gameState.agents) {
			if(std::abs(agent.position - gameState.goal) < 0.01f) {
				ended = true;
			}
		}
		return ended;
	}

	// Game update:
	void update(Game& gameState, float deltaTime) {
		// Tee kaikkien agenttien actionit:
		int agentId = 0;
		for(auto& agent : gameState.agents) {
			// Kysy tehtävä action id politiikkafunktiolta:
			ActionType action = agent.policy(agent, gameState);
			// Kutsu pelin action funktiota:
			EventType ev = gameState.actions[action](gameState, agent);

			// tee uusi historiasolmu agentin actionin ja uuden tilan mukaan.
			auto prevNode = gameState.agentHistories[agentId];
			auto newNode = std::make_shared<Game::NodeType>();
			newNode->prevNode = prevNode;
			newNode->action = action;
			newNode->state = agent.position;
			gameState.agentHistories[agentId] = newNode;


			// Jos ei tyhjä eventti, kutsu agentin event funktiota (sisäinen eventti):
			if(!ev.empty() && agent.event) {
				agent.event(agent, ev);
			}
		}
		std::vector<EventType> events;
		// TODO: Integroi peliä deltaTimen verran eteenpäin ja kerää ulkoiset eventit:

		// Lähetä kaikki eventit kaikille agenteille:
		for(const auto& ev : events) {
			for(auto& agent : gameState.agents) {
				if(!ev.empty() && agent.event) {
					agent.event(agent, ev);
				}
			}
		}
	}
}

#include <view/sfml_application.h>
#include <thread> // sleep
//template<typename NodeType, typename StateType>
auto findPlanToGoal(const auto& gameState, auto agentId) {
	typedef std::vector<float> StateType;
	typedef agents::PlanNode<StateType, int> NodeType;

	std::vector<int> plan;
	auto isGoalFound = [&gameState](auto state) {
		return state == gameState.goal;
	};

	auto vectorLen = [](const auto& v1, const auto& v2) {
		float dotProduct = 0.0f;
		for (size_t i = 0; i < v1.size(); ++i) {
			auto d = v1[i] - v2[i];
			dotProduct += d * d;
		}
		return std::sqrt(dotProduct);
	};

	auto isLegalState = [&gameState](const auto node) {
		// Tarkista onko mapin sisällä:
		auto sizeY = gameState.map.size();
		auto sizeX = gameState.map[0].size();
		return node->state[0] >= 0 && node->state[0] < sizeX
			&& node->state[1] >= 0 && node->state[1] < sizeY;
	};

	auto systemFunc = [&gameState, &agentId, &vectorLen, isLegalState](auto prevNode, int actionId) {
		auto state1 = std::make_shared<NodeType>();
		// Uusi tila saadaan tekemällä action (actionId indeksistä)
		auto newGameState = gameState;
		newGameState.agents[agentId].position = prevNode->state;
		gameState.actions[actionId](newGameState, newGameState.agents[agentId]);
		state1->state = newGameState.agents[agentId].position;
		state1->action = actionId;
		state1->prevNode = prevNode;
		return state1;
	};

	auto makeAllActions = [systemFunc, &gameState, isLegalState, vectorLen](auto prevNode) {
		std::vector< std::shared_ptr<NodeType> > results;
		for (size_t actionId = 0; actionId < gameState.actions.size(); ++actionId) {
			auto newNode = systemFunc(prevNode, int(actionId));
			if (isLegalState(newNode)) {
				auto factor = vectorLen(prevNode->state, newNode->state);
				auto x = newNode->state[0];
				auto y = newNode->state[1];
				float cost = 1.0f + float(gameState.map[y][x]);
				newNode->totalGCost = prevNode->totalGCost + factor * cost; // Oletetaan 1:n kustannus nyt
				results.push_back(newNode);
			}
		}
		return results;
	};


	auto popBestG = [](auto& openList) {
		auto minNode = openList[0];
		auto minIndex = 0;
		auto minG = openList[0]->totalGCost;
		for (size_t i = 1; i < openList.size(); ++i) {
			auto curG = openList[i]->totalGCost;
			if (curG < minG) {
				minNode = openList[i];
				minIndex = i;
				minG = curG;
			}
		}
		openList.erase(openList.begin() + minIndex, openList.begin() + minIndex + 1);
		return minNode;
	};

	// Tee haku: Alkutila on agentin positio
	auto initialState = gameState.agents[agentId].position;
	auto goaledNode = agents::dijkstra<NodeType, StateType>(initialState, popBestG, isGoalFound, makeAllActions);
	if (goaledNode) {
		printf("Found plan! Cost:%f\n", goaledNode->totalGCost);
		traverseList(goaledNode, [&plan](auto n) {
			printf("Action: %d, State: %f,%f\n", n->action, n->state[0], n->state[1]);
			plan.push_back(n->action);
			});
	}

	return plan;
}


int main() {
	std::vector<std::shared_ptr<sf::Texture> > textures = sfml_application::loadTextures({
		"assets/ground.png",
		"assets/goal.png",
		"assets/wall.png",
		"assets/player.png",
		"assets/enemy.png",
		"assets/item.png",
	});

	auto agentPolicy = [](auto& gameState, auto& agent){
		return rand() % 2;
	};
	auto agent1 = find_number::Agent{
		0.0f,
		agentPolicy
	};

	// Pelin tila muistissa:
	find_number::Game gameState;
	gameState.agents.push_back(agent1);
		//Lissää alkutila agenttien historiann:
	auto initialState = std::make_shared<find_number::Game::NodeType>();
	initialState->action = -1;
	initialState->state = agent1.position;
	gameState.agentHistories.push_back(initialState);

	gameState.map = {
		0, 0, 0, 0, 0, 0, 0, 0
	};
	gameState.goal = 5.0f;
	printf("Game start: Agent=%f, Goal=%f\n", gameState.agents[0].position, gameState.goal);

	int n = 0;
	// Suorita sovellus
	int status = sfml_application::runGame("Sokea haku", [&gameState,&n](float deltaTime) {
		find_number::update(gameState, deltaTime);
		++n;
		return gameState;
	}, [&gameState,&textures](auto& window) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		// Aseta origo keskelle:
		window.setView(sf::View(sf::Vector2f(0,0),sf::Vector2f(800.0f,600.0f)));
		// Render map:
		sfml_application::render(window, gameState.map, textures);
		// Render goal:
		sfml_application::render(window, sf::Vector2f(gameState.goal, 0.0f), 0.0f, *textures[1]);
		// Render player:
		sfml_application::render(window, sf::Vector2f(gameState.agents[0].position, 0.0f), 0.0f, *textures[3]);
		return !find_number::isEnd(gameState);
	});
	printf("Game ended after %d steps\n", n);

	for (size_t agentId = 0; agentId < gameState.agentHistories.size(); ++agentId) {
		printf("Agent %d History:\n", int(agentId));
		agents::traverseList(gameState.agentHistories[agentId], [](auto n) {
			printf("Action: %d, State: %f\n", n->action, n->state);
		});
	}
	return status;
}
