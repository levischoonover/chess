#pragma once

#include <optional>
#include <vector>

#include "game_state.hpp"

struct SearchConstraints {
	std::optional<int> wtime_ms;
	std::optional<int> btime_ms;
	std::optional<int> winc_ms;
	std::optional<int> binc_ms;
	std::optional<int> depth;
	std::optional<int> nodes;
	std::optional<int> mate_in;
	std::optional<int> movetime_ms;
	std::optional<std::vector<Move>> moves_to_search;
	bool infinite;
};

Move search_position(GameState state, SearchConstraints search_constraints);