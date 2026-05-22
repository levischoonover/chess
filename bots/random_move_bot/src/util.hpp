#pragma once

#include "game_state.hpp"

bool in_bounds(Position square);

Player other_player(const Player player);

PieceType letter_to_piece_type(const char letter);

Position string_to_position(const std::string& str);

std::vector<std::string> split_string(std::string str, char delimiter);

Move string_to_move(const std::string& str);