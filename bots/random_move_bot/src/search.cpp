#include <chrono>
#include <atomic>
#include <stop_token>
#include <mutex>
#include <optional>

#include "search.hpp"
#include "game_state.hpp"
#include "util.hpp"


void search_position(const std::stop_token& stop_token, SearchRequest& search_request_shared, std::atomic<bool>& new_request, std::mutex& mutex) {

	std::chrono::steady_clock::time_point start;
	const std::chrono::seconds move_time{2};

	std::optional<SearchRequest> processing;

	GameState ponder_position{};
	std::vector<Move> line;

	GameState current_position{};
	int index_of_nonreality = 0;

	while (!stop_token.stop_requested()) {

		// Handle new request
		if (new_request) {

			// Entering a sensitive part of the code
			std::lock_guard<std::mutex> lock(mutex);

			if (
				base_position == search_request_shared.base_game_state
				&& search_request_shared.uci_moves.size() >= index_of_nonreality
				&& std::equal(line.begin(), line.begin() + index_of_nonreality, search_request_shared.uci_moves.begin())
			) {
				// The new position is only the last position + some extra moves.
				// Therefore we can save time and only evaluate the new moves, building off the previous position
				// The index_of_nonreality check exists to make sure we don't regress from the current state,
				// we should only be building off the previous position not going back, because the previous
				// moves have not been made by us and are not the best
				for (int i = )
			} else {

				current_position = search_request_shared.base_game_state;


			}

			processing = std::move(search_request_shared); // Move the search request

			new_request = false; // It was consumed, set to make sure it isn't read again

			start = std::chrono::steady_clock::now();

		}

		if (processing) {

			if (std::chrono::steady_clock::now() - start >= move_time) {

			}

		} else {
			
		}

	}

	
}


/*
To Prevent Data Race:
SearchRequest& search_request
std::atomic<bool>& new_request


*/