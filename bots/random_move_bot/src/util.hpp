#pragma once

#include "game_state.hpp"

struct ParseError{};

GameState create_starting_state();

GameState fen_to_gamestate(const std::vector<std::string>& fen);

bool in_bounds(Position square);

Player other_player(const Player player);

PieceType letter_to_piece_type(const char letter);

Position string_to_position(const std::string& str);

std::vector<std::string> split_string(std::string str, char delimiter);

Move string_to_move(const std::string& str);

int random_from_zero(int max);