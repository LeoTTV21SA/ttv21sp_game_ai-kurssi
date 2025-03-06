#include <assert.h> // Ei vara venettä kaada, eikä assert toimivaa softaa...
#include <cmath>
#include <functional> // std::function
#include <array> // std::array
#include <vector> // std::array
#include <functional> // std::function
#include <array> // std::array
#include <vector> // std::array
#include <algorithm>

namespace find_goal_in_grid {

	///
	/// \brief MODEL: The GameState data
	///
	struct GameState {

		/// predict(GameState&, AgentId, ActionId) -> bool
		typedef std::function<bool(GameState&, size_t, size_t)> PredictFunc;
		/// isLegalState(const GameState&, AgentId) -> bool
		typedef std::function<bool(GameState&, size_t)>			LegalStateFunc;
		/// isGameOver(const GameState&) -> bool
		typedef std::function<bool(GameState&)>					GameOverFunc;
		/// getHCost(const GameState&) -> float
		typedef std::function<float(size_t, const GameState&)>	HCostFunc;
		typedef std::function<float(size_t, const GameState&, size_t)>	QCostFunc;
		/// getHCost(const GameState&) -> std::vector<float>
		typedef std::function<std::vector<float>(size_t, const GameState&)>	HashFunc;

		/// PolicyFunc(AgentId, const GameState&) -> size_t
		typedef std::function<size_t(size_t, GameState&)> PolicyFunc;
		/// ActionFunc(GameState&,AgentId) -> void
		typedef std::function<void(GameState&, size_t)>			ActionFunc;
		 /// Typedef for Position
		struct Position 
		{
			int x, y; 
			bool operator==(const Position& other) const {
				return x == other.x && y == other.y;
			}

			bool operator!=(const Position& other) const {
				return !(*this == other);
			}
		};

		typedef std::function<bool(const GameState&, const Position&)>			LegalAgentPositionFunc;
		
		struct Node
		{
			Position position;
			float g = 0;
			float h = 0;
			float f = 0;
			Node* parent = nullptr;
			int actionId = -1;
		};

		

		/// \brief The Goal entity.
		struct Goal 
		{
			Position	state;
		};

		struct  SearchState
		{
			std::vector<Node*> openList;
			std::vector<Node*> closedList;
			std::vector<Node*> plan;
		};

		struct AgentState {
			Position			position;
			SearchState searchState;
			std::vector<size_t> actions;
			int livesLeft = 3;

		};


		/// \brief The Agent entity consists of state and policy function.

		struct Agent {
			AgentState	state;
			PolicyFunc	policy;
			
		};

		/// Game data is Environment map and game agents and numActions
		std::vector< std::vector<int> >	map;
		std::vector<Goal>				goals;
		std::vector<Agent>		agents;

		/// Actions
		std::vector<ActionFunc> actions;

		/// Functions for agent use:
		PredictFunc		predict;
		LegalStateFunc	isLegalState;
		LegalAgentPositionFunc isLegalAgentPosition;
		GameOverFunc	isGameOver;
		HCostFunc		getHCost;
		QCostFunc		getQCost;
		HashFunc		getHash;

		size_t			numActions = 0;
	};

	///
	/// \brief CONTROLLER: Steps the game a single step
	///
	template<typename GameState>
	bool update(GameState& gameState) {
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

		/// Game update function:
		gameState.predict = [&actions](GameState& game, size_t agentId, size_t actionId) {
			// Check errorneus actionId and return true of not correct action id
			if (actionId >= actions.size()) return true;
			// Apply agent action by actionId
			actions[actionId](game, agentId);
			// Return true, if agent made legal action, which leads to legal state
			return game.isLegalState(game, agentId);
		};

		gameState.isLegalAgentPosition = [](const GameState& game, const GameState::Position& s) {
			// Check map limits
			if (s.y < 0 || s.y >= game.map.size()) return false;
			if (s.x < 0 || s.x >= game.map[s.y].size()) return false;
			// Check that map tile is not occupied
			return game.map[s.y][s.x] <= 0;
		};

		/// Game is legal state function:
		gameState.isLegalState = [](const GameState& game, size_t agentId) {
			return game.isLegalAgentPosition(game, game.agents[agentId].state.position);
		};



		/// Game over function:
		gameState.isGameOver = [](const GameState& game) {
			// If agent is in goal state (10,0), agent is winner
			for (size_t agentId = 0; agentId < game.agents.size(); ++agentId) 
			{
				auto agentState = game.agents[agentId].state.position;
				auto goal = game.goals[agentId].state;
				if (agentState.x == goal.x && agentState.y == goal.y) 
				{
					return true;
				}
			}
			return false;
		};

		///
		/// \brief  Palauttaa etäisyyden maaliin
		///
		gameState.getHCost = [](size_t agentId, const GameState& gameState) 
		{
			const auto& agent = gameState.agents[agentId].state.position;
			const auto& goal = gameState.goals[0].state; // Ensimmäinen maali
			auto dx = goal.x - agent.x; // Loppupiste miinus alkupiste
			auto dy = goal.y - agent.y;
			return (float)std::sqrt(dx * dx + dy * dy); // Laske pythagooraan lauseella etäisyys maaliin
		};

		///
		/// \brief  Palauttaa yhden actionin kustannuksen annetussa pelin tilassa
		///
		gameState.getQCost = [](size_t agentId, const GameState& gameState, size_t actionId) 
		{
			const auto& map = gameState.map;
			const auto& pos = gameState.agents[agentId].state.position;
			int tileValue = map[pos.y][pos.x];
			if (tileValue <= 0)
			{
				return 1.0f - float(tileValue); // 0->1, -1->2, -2->3, -3->4
			}
			assert(0);
			return 100.0f;
		};

		///
		/// \brief  Palauttaa yksilöllisen hash koodin peliobjektin tilalle.
		/// Tässä: Palauta agentin positio vektorissa, koska se yksilöi eri tilat toisistaan.
		///
		gameState.getHash = [](size_t agentId, GameState gameState)
		{
			const auto& agent = gameState.agents[agentId].state.position;
			return std::vector<float>{(float)agent.x, (float)agent.y};
		};

		// Update game by first get agent action from policy func
		auto newState = gameState;
		size_t agentId = 0;
		auto actionId = newState.agents[0].policy(agentId, newState);
		// and then apply action by using game predict function
		if (true == newState.predict(newState, agentId, actionId))
		{
			gameState = newState; // Action did not lead to illegal state, update new state.
		}
		else {
			printf("Agent in illegas state!\n");
		}
		// Return "is not game over"
		return !gameState.isGameOver(gameState);
	}

}

