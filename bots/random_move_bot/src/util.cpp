#include "game_state.hpp"

bool in_bounds(Position square) {
	return (
		square.rank >= 0
		&& square.file >= 0
		&& square.rank < BOARD_SIZE
		&& square.file < BOARD_SIZE
	);
}

Player other_player(const Player player) {
	return player == Player::White ? Player::Black : Player::White;
}

PieceType letter_to_piece_type(const char letter) {
	switch (std::tolower(letter)) {
		case 'p': return PieceType::Pawn;
		case 'n': return PieceType::Knight;
		case 'b': return PieceType::Bishop;
		case 'r': return PieceType::Rook;
		case 'q': return PieceType::Queen;
		case 'k': return PieceType::King;
		default:
			throw ParseError{};
	}
}

Position string_to_position(const std::string& str) {
	if (str.size() != 2) {
		throw ParseError{};
	}
	Position square {
		BOARD_SIZE - (str[1] - '0'),
		str[0] - 'a'
	};
	if (!in_bounds(square)) {
		// TODO search for Position
		throw ParseError{};
	}
	return square;
}

std::vector<std::string> split_string(std::string str, char delimiter) {
	std::vector<std::string> result;
	size_t pos;
	while (true) {
		pos = str.find_first_not_of(delimiter);
		if (pos == std::string::npos) {
			// There are only spaces left, or nothing
			break;
		}
		str.erase(0, pos); // Remove all leading spaces
		pos = str.find_first_of(delimiter);
		result.push_back(str.substr(0, pos));
		str.erase(0, pos); // Remove the word
	}
	return result;
}

Move string_to_move(const std::string& str) {
	if (str.size() < 4 || str.size() > 5) {
		throw ParseError{};
	}
	Move move{
		string_to_position(str.substr(0, 2)),
		string_to_position(str.substr(2, 2))
	};
	if (str.size() == 5) {
		move.promotion_piece = letter_to_piece_type(str[4]);
	}
	return move;
}