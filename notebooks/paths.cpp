#include <agents.h>
#include <stdio.h>
#include <vector>
#include <functional>

typedef int StateType;
typedef agents::PlanNode<int, int> NodeType;
typedef std::function<StateType(StateType)>actionFunc;



int main() {
	/// Pelin actionit:
	std::vector<actionFunc> actions;

	// action id = 0, kävele oikealle
	actions.push_back([](auto state) {
		return state + 1;
	});

	// action id = 1, käavele vasemalle
	actions.push_back([](auto state) {
		return state - 1;
		});

	// Alkutila 
	auto initialState = std::make_shared<NodeType>();
	initialState->state = 0;
	initialState->action = -1;
	initialState->prevNode = 0;

	auto systemFunc = [&actions](auto prevNode, int actionId) {
		auto state1 = std::make_shared<NodeType>();
		// Uusitila saadaan tekemällö action (actionId)
		state1->state = actions[actionId](prevNode->state);
		state1->action = actionId;
		state1->prevNode = prevNode;
		return state1;
	};
	
	auto newState = systemFunc(initialState, 0);
	printf("State is now: %d\n", newState->state);
	newState = systemFunc(newState, 0);
	printf("State is now: %d\n", newState->state);
    newState = systemFunc(newState, 0);
	printf("State is now: %d\n", newState->state);
	 newState = systemFunc(newState, 0);
	printf("State is now: %d\n", newState->state);
	// Kävele vasemalle 
	newState = systemFunc(newState, 1);
	printf("State is now: %d\n", newState->state);

	printf("Replay history:\n");
	traverseList(newState, [](auto n) {
		printf("Antion: %d , State: %d\n", n->action, n->state);
		});

	printf("hello world!!\n");
	return 0;
}