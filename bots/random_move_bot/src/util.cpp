#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <array>

std::vector<std::string> split_by_spaces(std::string str) {
	std::vector<std::string> result;
	size_t pos;
	while (true) {
		pos = str.find_first_not_of(' ');
		if (pos == std::string::npos) {
			// There are only spaces left, or nothing
			break;
		}
		str.erase(0, pos); // Remove all leading spaces
		pos = str.find_first_of(' ');
		result.push_back(str.substr(0, pos));
		str.erase(0, pos); // Remove the word
	}
	return result;
}

std::optional<std::array<std::array<char, 8>, 8>> fen_to_position(const std::string& fen) {
	std::vector<std::string> fen_sections = split_by_spaces(fen);
	return std::nullopt;
}