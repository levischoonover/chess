#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

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
	int rank; // 0-7  =>  8-1
	int file; // 0-7  =>  A-H
};

struct CastlingAvailabilty {
	bool kingside;
	bool queenside;
};

const int BOARD_SIZE = 8;

// This struct is ultimately what gets passed around between the various functions in game_state.cpp.
// It can completely represent a snapshot of any chess game at a point in time.
struct GameState {
	std::array<std::array<std::optional<Piece>, BOARD_SIZE>, BOARD_SIZE> board;
	std::optional<Position> en_passant_target;
	Player to_move;
	std::array<CastlingAvailabilty, 2> castling_rights;
	bool operator==(const GameState& other) const = default;
};

// Moves

struct Move {
	Position start_position;
	Position end_position;
	std::optional<PieceType> promotion_piece;
};

void make_move_unsafe(GameState& state, const Move& move);

std::vector<Move> get_all_moves(const GameState& state);