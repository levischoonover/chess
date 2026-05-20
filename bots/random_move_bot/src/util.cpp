#include <iostream>
#include <string>
#include <optional>
#include <array>

std::optional<std::array<std::array<char, 8>, 8>> fen_to_position(std::string fen) {
	int pos_of_first_space;
	while ((pos_of_first_space = fen.find(" ")) != std::string::npos) {
		std::string subsection = fen.substr(0, pos_of_first_space);
		fen.erase(0, pos_of_first_space + 1);
		std::cout << "|" << subsection << "|" << std::endl;
	}
	return std::nullopt;
}