#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <array>

#include "common.hpp"

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
				std::cerr << "[!] Invalid option for command `debug`: expects on | off" << std::endl;
			}
			else if (input.size() == 2) {
				if (input[1] == "on") {
					// Normally this would set the debug option to ON.
				} else if (input[1] == "off") {
					// Normally this would se the debug option to OFF.
				} else {
					std::cerr << "[!] Invalid option for command `debug`: expects on | off" << std::endl;
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
					new_state = fen_to_gamestate(fen).value();
				} catch (const std::bad_optional_access& e) {
					std::cerr << "[!] Invalid FEN for command `position`" << std::endl;
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
			// TODO

			// This way, the assignment only happens if no command syntax errors
			state = new_state;
		}

		// Unknown command
		else {
			std::cerr << "[!] Unknown command: " << input[0] << std::endl;
		}
		
	}
}