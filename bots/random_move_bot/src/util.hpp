#pragma once

#include <array>
#include <optional>
#include <string>

std::optional<std::array<std::array<char, 8>, 8>> fen_to_position(std::string fen);