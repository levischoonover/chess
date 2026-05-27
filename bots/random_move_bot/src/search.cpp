#include "search.hpp"
#include "game_state.hpp"
#include "util.hpp"

// Move search_position(GameState state, SearchConstraints search_constraints) {
// 	std::vector<Move> legal_moves = get_all_moves(state);

// 	// Some annoying random number generation setup
// 	std::random_device rd;
// 	std::mt19937 gen(rd());
// 	std::uniform_int_distribution<> dist(0, legal_moves.size() - 1);

// 	return legal_moves[dist(gen)];
// }


void search_position(GameState state, SearchConstraints search_constraints) {
	// A copy of the state and search_constraints are made so even if the user enters a new position, this thread can keep searching

	
}