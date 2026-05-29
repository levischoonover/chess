#pragma once

#include <optional>
#include <vector>

#include "game_state.hpp"

enum class EngineState {
	Idle,
	Ponder,
	Think
};

struct SearchConstraints {
	GameState game_state;
	std::optional<int> white_time_ms;
	std::optional<int> black_time_ms;
	std::optional<int> white_increment_ms;
	std::optional<int> black_increment_ms;
	std::optional<int> depth;
	std::optional<int> nodes;
	std::optional<int> mate_in;
	std::optional<int> movetime_ms;
	std::optional<std::vector<Move>> moves_to_search;
	bool infinite;
	EngineState engine_state;
};

void search_position(GameState state, SearchConstraints search_constraints);