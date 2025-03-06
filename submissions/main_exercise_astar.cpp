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

///
/// \brief GAME APP: main function for game with mikroplot visualizations.
///
int main() {
	using namespace find_goal_in_grid;
	mikroplot::Window window(800, 800, "A star exercise");

	// Game initial state
	auto policyFunc = [](size_t agentId, GameState& game) {
		return size_t(0);
	};


	/// TODO: Try out diffenret constants for heuristics...
	GameState gameState = {
		// map = 10x10 grid
		mikroplot::gridNM(100,100,0),
		// Single goal at position = 90,90
		{{90, 90}},
		// Single agent at position 10,10
		{GameState::Agent{{10, 10}, policyFunc}}
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
	genRow(gameState.map, 99, 60, -25, 1);
	genCol(gameState.map, 80, 65, 10, 1);

	// Run game loop
	size_t n = 0;
	while (false == window.shouldClose()) {
		// Update game
		if (false == update(gameState)) {
			// Game over
			break;
		}

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

		/*const auto& searchState = gameState.agents[0].state.searchState;

		/// TODO: Lis‰‰ open ja closed listin ja polkujen (goaledNode tai curNode) renderˆinti t‰h‰n:
		for (size_t i = 0; i < searchState.openList.size(); ++i) {
			auto node = searchState.openList[i];
			auto y = node->state.agents[0].state.position.y;
			auto x = node->state.agents[0].state.position.x;
			map[y][x] = 13;
		}*/

		// Render agent
		map[agent.y][agent.x] = 12; // 12 = blue

		// Render goal
		auto goal = gameState.goals[0].state;
		map[goal.y][goal.x] = 10; // 10 = Red


		// Render map using drawPixels
		//window.drawHeatMap(heatMap, 0.0f, 50.0f);
		window.drawPixels(map);

		// Take screenshots
		window.screenshot("astar_" + std::to_string(n) + ".png");

		// And update windw
		window.update();
		++n;
	}
	return 0;
}
