#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

struct ParseError{};

enum class PieceType {
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

enum class Player {
	White,
	Black
};

struct Piece {
	PieceType type;
	Player color;
};

struct Position {
	unsigned short rank; // 8-1
	unsigned short file; // A-H
};

struct CastlingAvailabilty {
	bool kingside;
	bool queenside;
};

struct GameState {
	std::array<std::array<std::optional<Piece>, 8>, 8> board;
	std::optional<Position> en_passant_target;
	Player to_move;
	std::array<CastlingAvailabilty, 2> castling_rights;
};

Position string_to_position(const std::string& str);

std::vector<std::string> split_string(std::string str, char delimiter);

GameState create_starting_state();

GameState fen_to_gamestate(const std::vector<std::string>& fen);