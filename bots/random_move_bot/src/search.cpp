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

	GameState current_position{};
	std::vector<Move> line;

	GameState ponder_position{};
	int ponder_start_index = 0;

	while (!stop_token.stop_requested()) {

		// Handle new request
		if (new_request) {

			// Check once more that the stop_token wasn't triggered
			// Otherwise we have a situation where the main thread sets the stop_token and new_request after the beginning of the while loop
			if (stop_token.stop_requested()) {
				break;
			}

			// Entering a sensitive part of the code
			std::lock_guard<std::mutex> lock(mutex);

			int i;
			if (
				processing->base_game_state == search_request_shared.base_game_state
				&& search_request_shared.uci_moves.size() >= ponder_start_index
				&& std::equal(line.begin(), line.begin() + ponder_start_index, search_request_shared.uci_moves.begin()) // the new request matches the ENTIRETY of the REAL portion of the line
			) {
				// The new position is only the last position + some extra moves.
				// Therefore we can save time and only evaluate the new moves, building off the previous position
				// The index_of_nonreality check exists to make sure we don't regress from the current state,
				// we should only be building off the previous position not going back, because the previous
				// moves have not been made by us and are not the best
				i = ponder_start_index;

			} else {
				current_position = search_request_shared.base_game_state;
				i = 0;
			}

			for (/* i already set */; i < search_request_shared.uci_moves.size(); i++) {
				make_move_unsafe(current_position, search_request_shared.uci_moves[i]);
				line.push_back(search_request_shared.uci_moves[i]);
			}

			processing = std::move(search_request_shared); // Move the search request

			new_request = false; // It was consumed, set to make sure it isn't read again

			start = std::chrono::steady_clock::now();

			// TODO reset ponder position?

		}

		if (processing) {

			if (std::chrono::steady_clock::now() - start >= move_time) {

				

				if (processing->ponder || processing->infinite) {



				} else {



				}

			}

		} else {
			// Nothing to do
			new_request.wait(false); // Wait until true
			// Note: This will not interrupt for a change in stop token, so main thread must set new_request = true in order to stop the thread.
		}

	}

	// If it has a best move and it was thinking, it should output it here
	// This behavior is not expressed while pondering

	
}