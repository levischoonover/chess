#include <iostream>
#include <string>
#include <vector>
#include <array>

#include "game_state.hpp"



/* DEBUG */

#include <optional>
char piece_to_char(const Piece& piece) {
	char c = '?';

	switch (piece.type) {
		case PieceType::Pawn:   c = 'P'; break;
		case PieceType::Knight: c = 'N'; break;
		case PieceType::Bishop: c = 'B'; break;
		case PieceType::Rook:   c = 'R'; break;
		case PieceType::Queen:  c = 'Q'; break;
		case PieceType::King:   c = 'K'; break;
	}

	// Black pieces are lowercase
	if (piece.color == Player::Black) {
		c = static_cast<char>(std::tolower(c));
	}

	return c;
}

void print_board(const GameState& state) {
	std::cerr << "  A B C D E F G H\n";

	for (int rank = 0; rank < 8; ++rank) {
		std::cerr << (8 - rank) << ' ';

		for (int file = 0; file < 8; ++file) {
			const auto& square = state.board[rank][file];

			if (square.has_value()) {
				std::cerr << piece_to_char(*square);
			} else {
				std::cerr << '.';
			}

			std::cerr << ' ';
		}

		std::cerr << (8 - rank) << '\n';
	}

	std::cerr << "  A B C D E F G H\n";
}


/* END DEBUG */

int main(int argc, char* argv[]) {

	GameState state = create_starting_state();

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

		// Command: debug
		else if (input[0] == "debug") {
			if (input.size() == 1) {
				std::cerr << "[!] Invalid option for command `debug`: expects argument on | off" << std::endl;
			}
			else if (input.size() == 2) {
				if (input[1] == "on") {
					// Normally this would set the debug option to ON.
				} else if (input[1] == "off") {
					// Normally this would se the debug option to OFF.
				} else {
					std::cerr << "[!] Invalid option for command `debug`: expects argument on | off" << std::endl;
				}
			}
			else {
				std::cerr << "[!] Command `debug` expects no more than one argument" << std::endl;
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

		// Command: register
		else if (input[0] == "register") {
			// This is not implemented so anything goes
			std::cout << "registration ok" << std::endl;
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
			GameState new_state;
			if (input[1] == "fen") {
				std::vector<std::string> fen(input.begin() + 2, input.begin() + 8);
				try {
					new_state = fen_to_gamestate(fen);
				} catch (const ParseError&) {
					std::cerr << "[!] Invalid option for command `position`: invalid FEN" << std::endl;
					continue;
				}
				index = 8; // To account for the extra terms
			}
			else if (input[1] == "startpos") {
				new_state = create_starting_state();
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
						make_move_unsafe(new_state, string_to_move(input[index]));
						index++;
					} catch (const ParseError&) {
						std::cerr << "[!] Invalid option for command `position`: invalid move \"" << input[index] << "\"" << std::endl;
						continue;
					}
				}
			}

			// The assignment only happens if no command syntax errors
			state = new_state;

			/* DEBUG */
			print_board(state);
		}

		// Command: go
		else if (input[0] == "go") {
			int index = 1;
			// Process other arguments
			std::optional<std::vector<Move>> moves_to_search;
			while (index < input.size()) {

				// Subcommand: searchmoves
				if (input[index] == "searchmoves") {
					moves_to_search.emplace();
					while (++index < input.size()) {
						try {
							moves_to_search->push_back(string_to_move(input[index]));
						} catch (const ParseError&) {
							index--; // It was not a move, so we should give it another chance, it may be another command
							break;
						}
					}
				}

				// Subcommand: ponder
				if (input[index] == "ponder") {
					
				}

				index++;
			}

		}

		// Unknown command
		else {
			std::cerr << "[!] Unknown command: " << input[0] << std::endl;
		}
		
	}
}