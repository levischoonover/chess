#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <thread>
#include <mutex>
#include <atomic>

#include "game_state.hpp"
#include "search.hpp"
#include "util.hpp"



/* END DEBUG */

int main(int argc, char* argv[]) {

	GameState base_state = create_starting_state();
	std::vector<Move> uci_moves;

	std::optional<Move> expected_ponder_move;

	std::jthread search_thread;
	std::mutex search_thread_mutex;

	SearchRequest search_request;
	std::atomic<bool> new_request;

	/* Main loop to constantly handle user input */

	while (true) {

		// Read input from stdin
		std::string line;
		std::getline(std::cin, line);
		std::vector<std::string> input = split_string(line, ' ');

		// Ignore empty input to guarantee input[0]
		if (input.empty()) {
			continue;
		}

		// Command: uci
		else if (input[0] == "uci") {
			if (input.size() == 1) {
				std::cout << "id name Random Move Bot" << std::endl;
				std::cout << "id author Levi" << std::endl;
				// Options would be configured here
				std::cout << "uciok" << std::endl;
			} else {
				// Error: does not expect additional arguments
				std::cerr << "[!] Command `uci` takes no arguments" << std::endl;
			}
		}

		// Command: isready
		else if (input[0] == "isready") {
			if (input.size() == 1) {
				std::cout << "readyok" << std::endl;
			} else {
				std::cerr << "[!] Command `isready` takes no arguments" << std::endl;
			}
		}

		// Command: ucinewgame
		else if (input[0] == "ucinewgame") {
			if (input.size() == 1) {
				// Currently there is no reaction to creating a different game.
			} else {
				std::cerr << "[!] Command `ucinewgame` takes no arguments" << std::endl;
			}
		}

		// Command: position
		else if (input[0] == "position") {
			if (input.size() < 2) {
				std::cerr << "[!] Invalid option for command `position`: expects fen | startpos" << std::endl;
				continue;
			}
			int index;
			GameState new_base_state;
			std::vector<Move> new_uci_moves;
			if (input[1] == "fen") {
				std::vector<std::string> fen(input.begin() + 2, input.begin() + 8);
				try {
					new_base_state = fen_to_gamestate(fen);
				} catch (const ParseError&) {
					std::cerr << "[!] Invalid option for command `position`: invalid FEN" << std::endl;
					continue;
				}
				index = 8; // To account for the extra terms
			}
			else if (input[1] == "startpos") {
				new_base_state = create_starting_state();
				index = 2;
			}
			else {
				std::cerr << "[!] Invalid option for command `position`: expects fen | startpos" << std::endl;
				continue;
			}

			// Process any moves that come after
			if (index < input.size()) {
				if (input[index++] != "moves") {
					std::cerr << "[!] Invalid option for command `position`: expected \"moves\" but got \"" << input[index] << "\"" << std::endl;
				}
				while (index < input.size()) {
					try {
						new_uci_moves.push_back(string_to_move(input[index]));
						index++;
					} catch (const ParseError&) {
						std::cerr << "[!] Invalid option for command `position`: invalid move \"" << input[index] << "\"" << std::endl;
						continue;
					}
				}
			}

			// The assignment only happens if no command syntax errors
			base_state = std::move(new_base_state);
			uci_moves = std::move(new_uci_moves);
		}

		// Command: go
		else if (input[0] == "go") {

			// Entering a sensitive part of the code
			std::lock_guard<std::mutex> lock(search_thread_mutex);

			search_request = SearchRequest{}; // Reset search_request to default values
			search_request.base_game_state = base_state;
			search_request.uci_moves = uci_moves;

			int index = 1;

			auto check_for_integer_subcommand = [&](const std::string& name, std::optional<int>& store_value) {
				if (input[index] == name) {
					if (++index >= input.size()) {
						std::cerr << "[!] Invalid option for command `go`: expects a number after `" << name << "`" << std::endl;
						throw ParseError{};
					}
					try {
						store_value = std::stoi(input[index]);
						return true;
					} catch (...) {
						std::cerr << "[!] Invalid option for command `go`: invalid number for `" << name << "`" << std::endl;
						throw ParseError{};
					}
				}
				return false;
			};

			// Process subcommands
			try {
				while (index < input.size()) {

					std::optional<int> _dummy;

					// Subcommand: searchmoves
					if (input[index] == "searchmoves") {
						search_request.moves_to_consider.emplace();
						while (++index < input.size()) {
							try {
								search_request.moves_to_consider->push_back(string_to_move(input[index]));
							} catch (const ParseError&) {
								index--; // It was not a move, so we should give it another chance, it may be another command
								break;
							}
						}
					}

					// Subcommand: ponder
					else if (input[index] == "ponder") {
						if (uci_moves.empty()) {
							std::cerr << "[!] Invalid option for command `go`: ponder is not possible with that position" << std::endl;
							throw ParseError{};
						} else {
							// For pondering, the last move in a position command is not real, it is merely the move that the gui expects the opponent to play
							expected_ponder_move = search_request.uci_moves.back();
							search_request.uci_moves.pop_back();
						}
						search_request.ponder = true;
					}


					// Subcommand: infinite
					else if (input[index] == "infinite") {
						search_request.infinite = true;
					}

					// Various subcommands
					else if (
						   check_for_integer_subcommand("wtime", search_request.white_time_ms)
						|| check_for_integer_subcommand("btime", search_request.black_time_ms)
						|| check_for_integer_subcommand("winc", search_request.white_increment_ms)
						|| check_for_integer_subcommand("binc", search_request.black_increment_ms)
						|| check_for_integer_subcommand("movestogo", _dummy)
						|| check_for_integer_subcommand("depth", search_request.depth)
						|| check_for_integer_subcommand("nodes", search_request.nodes)
						|| check_for_integer_subcommand("mate", search_request.mate_in)
						|| check_for_integer_subcommand("movetime", search_request.movetime_ms)
					) {
						// Nothing
					}

					// If no command recognized, error
					else {
						std::cerr << "[!] Invalid option for command `go`: unknown subcommand `" << input[index] << "`" << std::endl;
						throw ParseError{};
					}


					index++;
				}


			} catch (const ParseError&) {
				continue;
			}

			// The search request is ready, it will be read by the thread
			if (!search_thread.joinable()) {
				// There is no thread, create one
				search_thread = std::jthread(search_position, search_request, new_request, search_thread_mutex);
			}

			new_request = true;		

		} // After this point the lock on the mutex will be released

		// Command: stop
		else if (input[0] == "stop") {
			search_thread.request_stop(); // will automatically manage stop token
		}

		// Command: ponderhit
		else if (input[0] == "ponderhit") {
			// Add the fake move back, because it's now real!
			if (!expected_ponder_move) {
				std::cerr << "[!] Command ponderhit may not be used in this context, `go ponder ...` must be called first" << std::endl;
			} else {
				uci_moves.push_back(expected_ponder_move.value());
				expected_ponder_move.reset();
			}
		}

		// Command: quit
		else if (input[0] == "quit") {
			return 0;
		}

		// Unknown command
		else {
			std::cerr << "[!] Unknown command: " << input[0] << std::endl;
		}
		
	}
}