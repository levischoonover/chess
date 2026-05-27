#include <random>

#include "game_state.hpp"
#include "util.hpp"


GameState create_starting_state() {
	GameState state{};

	state.board[0] = {
		Piece{PieceType::Rook,   Player::Black},
		Piece{PieceType::Knight, Player::Black},
		Piece{PieceType::Bishop, Player::Black},
		Piece{PieceType::Queen,  Player::Black},
		Piece{PieceType::King,   Player::Black},
		Piece{PieceType::Bishop, Player::Black},
		Piece{PieceType::Knight, Player::Black},
		Piece{PieceType::Rook,   Player::Black},
	};
	state.board[1].fill(Piece{PieceType::Pawn, Player::Black});
	state.board[6].fill(Piece{PieceType::Pawn, Player::White});
	state.board[7] = {
		Piece{PieceType::Rook,   Player::White},
		Piece{PieceType::Knight, Player::White},
		Piece{PieceType::Bishop, Player::White},
		Piece{PieceType::Queen,  Player::White},
		Piece{PieceType::King,   Player::White},
		Piece{PieceType::Bishop, Player::White},
		Piece{PieceType::Knight, Player::White},
		Piece{PieceType::Rook,   Player::White},
	};
	
	state.castling_rights = {
		CastlingAvailabilty{true, true},
		CastlingAvailabilty{true, true}
	};

	return state;
}

GameState fen_to_gamestate(const std::vector<std::string>& fen) {
	if (fen.size() != 6) {
		throw ParseError{};
	}

	GameState state{};
	
	// Process the actual position of pieces: fen[0]
	std::vector<std::string> ranks = split_string(fen[0], '/');
	if (ranks.size() != BOARD_SIZE) {
		// Returning null means error
		throw ParseError{};
	}
	for (int rank = 0; rank < BOARD_SIZE; rank++) {
		int file = 0;
		for (char letter : ranks[rank]) {
			if (std::isdigit(letter)) {
				// Skip this many squares
				file += letter - '0';
				if (file > BOARD_SIZE) {
					throw ParseError{};
				}
			} else {
				state.board[rank][file] = Piece{
					letter_to_piece_type(letter),
					std::isupper(letter) ? Player::White : Player::Black
				};
				if (++file > BOARD_SIZE) {
					throw ParseError{};
				}
			}
		}

		if (file < BOARD_SIZE) {
			throw ParseError{};
		}
	}

	// Player to move: fen[1]
	if (fen[1] == "w") {
		state.to_move = Player::White;
	} else if (fen[1] == "b") {
		state.to_move = Player::Black;
	} else {
		throw ParseError{};
	}

	// Castling rights: fen[2]
	if (fen[2] != "-") {
		for (char x : fen[2]) {
			switch (x) {
				case 'K':
					// White may castle Kingside
					state.castling_rights[static_cast<size_t>(Player::White)].kingside = true;
					break;
				case 'Q':
					// White may castle Queenside
					state.castling_rights[static_cast<size_t>(Player::White)].queenside = true;
					break;
				case 'k':
					// Black may castle Kingside
					state.castling_rights[static_cast<size_t>(Player::Black)].kingside = true;
					break;
				case 'q':
					// Black may castle Queenside
					state.castling_rights[static_cast<size_t>(Player::Black)].queenside = true;
					break;
				default:
					throw ParseError{};
			}
		}
	}

	// En Passant target square: fen[3]
	if (fen[3] != "-") {
		state.en_passant_target = string_to_position(fen[3]);
		// May throw a ParseError, which will not be caught here
	}

	// Halfmove clock: fen[4]
	// Just check if it's a digit, but it's unused
	for (char x : fen[4]) {
		if (!std::isdigit(x)) {
			throw ParseError{};
		}
	}

	// Fullmove Counter: fen[5]
	// Just check if it's a digit, but it's unused

	for (char x : fen[5]) {
		if (!std::isdigit(x)) {
			throw ParseError{};
		}
	}
	

	return state;
}

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

int random_from_zero(int max) {
	// Thread local is used so the generator is not initialized every time
	thread_local std::random_device rd;
	thread_local std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, max - 1);
	return dist(gen);
}