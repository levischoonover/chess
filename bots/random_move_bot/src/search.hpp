#pragma once

#include <optional>
#include <vector>
#include <atomic>

#include "game_state.hpp"

struct SearchRequest {
	GameState base_game_state;
	std::vector<Move> uci_moves;
	std::optional<int> white_time_ms;
	std::optional<int> black_time_ms;
	std::optional<int> white_increment_ms;
	std::optional<int> black_increment_ms;
	std::optional<int> depth;
	std::optional<int> nodes;
	std::optional<int> mate_in;
	std::optional<int> movetime_ms;
	std::optional<std::vector<Move>> moves_to_consider;
	bool infinite;
	bool ponder;
};

void search_position(SearchRequest& search_request, const std::atomic<bool>& new_request, const std::atomic<bool>& kill_thread);