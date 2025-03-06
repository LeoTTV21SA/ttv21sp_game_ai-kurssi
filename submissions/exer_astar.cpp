/// //////////////////////////////////////////////////////////////////////
///
/// TODO FEATURE A (tehd‰‰n yhdess‰ tunnilla): Add rendering 
/// of openList, closedList and plan to map[y][x] with colors 
/// of you choose to main-function.
///
/// HINTS: You need to modify Agent struct found from
/// find_goal_from_grid.h in way that it contains needed
/// variables in rendering. This is openList-, closedList-
/// and paths for partial plans and goaled plans (found from 
/// SearchState-struct). You also need to modify your agent
/// functions in way that it keeps track on search state.
///
/// //////////////////////////////////////////////////////////////////////
///
/// TODO FEATURE B: Make feature, which allows you to modify
/// heuristic constant used in a star search (F = G + k*H).
///
/// HINTS: You need to modify again Agent struct by adding
/// to constant k to its attributes. Give initial value for
/// k it in main-function, when creating initial state, so it
/// is easy for you to try out different k values and see the
/// results in stored screenshots (you need to enable taking 
/// of screenshots in main funcition).
///
/// NOTE: Is is also possible to implement this other way,
/// with out modifying Agent struct, but you need to figure
/// it out by you self, if you want. Hint: std::bind.
///
/// //////////////////////////////////////////////////////////////////////
///
/// ASSINGMENT: Try out different k values (for example: 0, 
/// 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, etc...) and see how it
/// affects to different search outcomes. Answer to questions
/// below:
/// 
/// Q1: How the changes in k value effects to time spend in finding path to goal?
/// 
/// A1: TODO
/// 
/// Q2: How the changes in k value effects to frontier of open nodes (open list)?
/// 
/// A2: TODO
/// 
/// Q3: How the changes in k value effects to Closed list nodes ?
/// 
/// A2: TODO
/// 
/// TODO: Also commit some screenshots named (astar_<kvalue>_<framenum>.png) to
/// version control. For example file names: astar_0_100.png, astar_half_100.png,
/// astar_1_100.png, astar_5_100.png...
///
///
/// //////////////////////////////////////////////////////////////////////


#include <agents.h>
#include <find_goal_in_grid.h>
#include <mikroplot/window.h>


template<typename Map>
void genRow(Map& map, size_t startX, size_t y, int dx, int value) {
	const auto MAX = startX + dx;
	if (dx < 0) {
		for (auto x = startX; x >= MAX && x < map[y].size(); --x) {
			map[y][x] = value;
		}
	}
	else {
		for (auto x = startX; x < MAX; ++x) {
			map[y][x] = value;
		}
	}
}

template<typename Map>
void genCol(Map& map, size_t x, size_t startY, int dy, int value) {
	const auto MAX = startY + dy;
	if (dy < 0) {
		for (auto y = startY; y >= MAX && y < map.size(); --y) {
			map[y][x] = value;
		}
	}
	else {
		for (auto y = startY; y < MAX; ++y) {
			map[y][x] = value;
		}
	}
}

template<typename Map>
void genSquare(Map& map, size_t startX, size_t startY, int dx, int dy, int value) {
	const auto MAX = startY + dy;
	if (dy < 0) {
		for (auto y = startY; y >= MAX && y < map.size(); --y) {
			genRow(map, startX, y, dx, value);
		}
	}
	else {
		for (auto y = startY; y < MAX; ++y) {
			genRow(map, startX, y, dx, value);
		}
	}
}



using namespace find_goal_in_grid;

void Astar(GameState& gameState) 
{
	//A* algorithm
	auto& openList = gameState.agents[0].state.searchState.openList;
	auto& closeList = gameState.agents[0].state.searchState.closedList;
	auto& plan = gameState.agents[0].state.searchState.plan;
	if (plan.size() > 0) {
		return;
	}
	else {
		openList.clear();
		closeList.clear();
	}
	// Add Node star open list 
	openList.push_back(new GameState::Node{ gameState.agents[0].state.position });

	// for loop A*
	while (!openList.empty())
	{
		GameState::Node* q = openList.front();
		int smallestIndex = 0;
		for (size_t i = 1; i < openList.size(); i++)
		{
			if (openList[i]->f < q->f)
			{
				smallestIndex = i;
				q = openList[i];
			}
		}


		openList.erase(openList.begin() + smallestIndex);
		closeList.push_back(q);

		// Check if the current node is the goal
		if (q->position.x == gameState.goals.front().state.x && q->position.y == gameState.goals.front().state.y)
		{
			while (q != nullptr)
			{
				plan.insert(plan.begin(), q);
				q = q->parent;
			}
			break;
		}


		//Generate the successors (neighbors) of the current node.
		std::vector<GameState::Node*> successors
		{
			new GameState::Node{.position = GameState::Position{q->position.x + 1  ,q->position.y}, .parent = q, .actionId=0 },
			new GameState::Node{.position = GameState::Position{q->position.x - 1  ,q->position.y}, .parent = q, .actionId=1 },
			new GameState::Node{.position = GameState::Position{q->position.x  ,q->position.y + 1}, .parent = q, .actionId=3 },
			new GameState::Node{.position = GameState::Position{q->position.x  ,q->position.y - 1}, .parent = q, .actionId=2 },
		};

		for (GameState::Node* successor : successors)
		{
			successor->g = q->g + 1;
			//1-Manhattan Distance  
			/*successor->h = abs(successor->position.x - gameState.goals.front().state.x)
				+ abs(successor->position.y - gameState.goals.front().state.y);*/

			// 2- Euclidean Distance-
			//successor->h = sqrt((successor->position.x - gameState.goals.front().state.x)*2 + (successor->position.y - gameState.goals.front().state.y)*2);

			// Diagonal Distance
			int dx = abs(successor->position.x - gameState.goals.front().state.x);
			int dy = abs(successor->position.y - gameState.goals.front().state.y);
			successor->h = std::max(dx, dy);

			successor->f = successor->g + successor->h;

			// Check if the successor is in the closed list or not valid
			if (std::find(closeList.begin(), closeList.end(), successor) != closeList.end()) {
				delete successor;
				continue;
			}

			if(false == gameState.isLegalAgentPosition(gameState, successor->position)) {
				delete successor;
				continue;
			}


			// Check if the successor is already in the open list with a lower cost.
			auto it = std::find_if(openList.begin(), openList.end(), [successor](GameState::Node* lhs) {
				return lhs->position.x == successor->position.x && lhs->position.y == successor->position.y;
			});

			if (it != openList.end())
			{
				if ((*it)->f > successor->f)
				{
					openList.erase(it);
				}
				else
				{
					delete successor;
					continue;
				}
				
			}
			openList.push_back(successor);
		}
	}
}



///
/// \brief GAME APP: main function for game with mikroplot visualizations.
///
int main() {
	mikroplot::Window window(800, 800, "Exer_Astar");

	// Game initial state
	auto policyFunc = [](size_t agentId, GameState& game) {
		auto& plan = game.agents[agentId].state.searchState.plan;

		// 1. Tee A* haku, jotta saat planin/suunitelman, jos agentilla ei ole jo sit‰.
		if (plan.size() == 0) {
			Astar(game);
		}

		for (const auto node : plan) {
			printf("Node: actionId=%d\n", node->actionId);
		}


		// 2. Suorita planin seuraava action.		
		if (!plan.empty()) {
			// Update agent's position to the next node in the optimal path
			size_t actionId = plan[0]->actionId;
			// Remove the visited node from the optimal path
			plan.erase(plan.begin());
			return actionId;
		}
		return size_t(0);
	};



	/// TODO: Try out diffenret constants for heuristics...
	GameState gameState = {
		// map = 10x10 grid
		mikroplot::gridNM(100,100,0),
		// Single goal at position = 90,90
		{{90, 90}},
		// Single agent at position 10,10
		{GameState::Agent{{10, 10}, policyFunc , }}
	};


	/// Game actions for agent from 0,1,2,3 (functor=mapping between categories):
	const std::vector<find_goal_in_grid::GameState::ActionFunc> actions = {
		// 0 = right
		[](GameState& game, size_t agentId) {
			game.agents[agentId].state.position.x += 1;
		},
		// 1 = left
		[](GameState& game, size_t agentId) {
			game.agents[agentId].state.position.x -= 1;
		},
		// 2 = up
		[](GameState& game, size_t agentId) {
			game.agents[agentId].state.position.y -= 1;
		},
		// 3 = down
		[](GameState& game, size_t agentId) {
			game.agents[agentId].state.position.y += 1;
		},
	};
	gameState.numActions = actions.size();

	// Top left
	genRow(gameState.map, 30, 30, -15, 1);
	genCol(gameState.map, 30, 30, -20, 1);
	genCol(gameState.map, 40, 0, 30, 1);
	genRow(gameState.map, 0, 40, 30, 1);

	// Main Cross in the middle
	genRow(gameState.map, 50, 50, 40, 1); // Right
	genCol(gameState.map, 50, 50, 10, 1); // Down
	genRow(gameState.map, 50, 50, -30, 1);// Left
	genCol(gameState.map, 50, 50, -40, 1);// Up
	
	genRow(gameState.map, 99, 50, -5, 1);
	genCol(gameState.map, 50, 99, -35, 1);
	genRow(gameState.map, 0, 50, 5, 1);
	genCol(gameState.map, 50, 0, 5, 1);

	// Top right
	genRow(gameState.map, 75, 25, 10, 1);
	genCol(gameState.map, 75, 25, 15, 1);
	genRow(gameState.map, 75, 25, -10, 1);
	genCol(gameState.map, 75, 25, -22, 1);
	genCol(gameState.map, 65, 47, -19, 1);
	genRow(gameState.map, 65, 40, 30, 1);

	// Bottom left
	genRow(gameState.map, 25, 75, 10, 1);
	genCol(gameState.map, 25, 75, 10, 1);
	genRow(gameState.map, 25, 75, -18, 1);
	genCol(gameState.map, 25, 75, -25, 1);
	genRow(gameState.map, 50, 75, -10, 1);
	genRow(gameState.map, 7, 65, 15, 1);
	genCol(gameState.map, 15, 80, 15, 1);
	genCol(gameState.map, 7, 65, 10, 1);

	// Bottom right
	genRow(gameState.map, 80, 80, 15, 1);
	genRow(gameState.map, 55, 80, 20, 1);
	genRow(gameState.map, 80, 80, 10, 1);
	genCol(gameState.map, 80, 80, 15, 1);
	//genRow(gameState.map, 99, 60, -25, 1);
	genCol(gameState.map, 80, 65, 10, 1);


	// Run game loop
	size_t n = 0;
	while (false == window.shouldClose()) {
		// Update game
		if (false == update(gameState)) {
			// Game over
			break;
		}

		// MOve agent along the optimal path
		//auto& agentState = gameState.agents[0].state;

		// Get map to render
		auto map = gameState.map;
		std::vector< std::vector<float> > heatMap = mikroplot::heatMapNM(map.size(), map[0].size(), 0.0f);

		for (size_t y = 0; y < map.size(); ++y) {
			for (size_t x = 0; x < map[y].size(); ++x) {
				if (map[y][x] <= 0) {
					heatMap[y][x] = 1 - map[y][x];
				}
				else {
					heatMap[y][x] = 5.0f;
				}
			}
		}

		auto agent = gameState.agents[0].state.position;

		const auto& searchState = gameState.agents[0].state.searchState;

		// TODO: Lis‰‰ open ja closed listin ja polkujen (goaledNode tai curNode) renderˆinti t‰h‰n:
		for (size_t i = 0; i < searchState.openList.size(); ++i) {
			auto node = searchState.openList[i];
			auto y = node->position.y;
			auto x = node->position.x;
			map[y][x] = 13;
		}
		for (size_t i = 0; i < searchState.closedList.size(); ++i) {
			auto node = searchState.closedList[i];
			auto y = node->position.y;
			auto x = node->position.x;
			map[y][x] = 15;
		}
	

		// Render agent
		map[agent.y][agent.x] = 12; // 12 = blue

		// Render goal
		auto goal = gameState.goals[0].state;
		map[goal.y][goal.x] = 10; // 10 = Red


		// Render map using drawPixels
		//window.drawHeatMap(heatMap, 0.0f, 50.0f);
		window.drawPixels(map);

		// Take screenshots
		//window.screenshot("astar_" + std::to_string(n) + ".png");


		// And update windw
		window.update();
		++n;
	}
	window.screenshot("astar_" + std::to_string(n) + ".png");
	window.update();
	return 0;
}




