#pragma once

#include <array>
#include <optional>
#include <string>

std::vector<std::string> split_by_spaces(std::string str);

std::optional<std::array<std::array<char, 8>, 8>> fen_to_position(const std::string& fen);