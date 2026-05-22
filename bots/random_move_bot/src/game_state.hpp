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
	unsigned short rank; // 0-7  =>  8-1
	unsigned short file; // 0-7  =>  A-H
};

struct CastlingAvailabilty {
	bool kingside;
	bool queenside;
};

const int BOARD_SIZE = 8;

struct GameState {
	std::array<std::array<std::optional<Piece>, BOARD_SIZE>, BOARD_SIZE> board;
	std::optional<Position> en_passant_target;
	Player to_move;
	std::array<CastlingAvailabilty, 2> castling_rights;
};

Position string_to_position(const std::string& str);

std::vector<std::string> split_string(std::string str, char delimiter);

GameState create_starting_state();

GameState fen_to_gamestate(const std::vector<std::string>& fen);

// Moves

struct Move {
	Position start_position;
	Position end_position;
	std::optional<PieceType> promotion_piece;
};

Move string_to_move(const std::string& str);

void make_move_unsafe(GameState& state, const Move& move);

std::vector<Move> get_all_moves(const GameState& state);