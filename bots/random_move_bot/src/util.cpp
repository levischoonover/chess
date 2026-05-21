#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <array>

#include "common.hpp"

std::optional<Position> string_to_position(const std::string& str) {
	if (str.size() != 2) {
		return std::nullopt;
	}
	Position square {
		static_cast<unsigned short>(8 - (str[1] - '0')),
		static_cast<unsigned short>(str[0] - 'a')
	};
	if (square.file >= 8 || square.rank >= 8) { // Unsigned so no need to compare below 0
		return std::nullopt;
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

std::optional<GameState> fen_to_gamestate(const std::vector<std::string>& fen) {
	if (fen.size() != 6) {
		return std::nullopt;
	}

	GameState state{};
	
	// Process the actual position of pieces: fen[0]
	std::vector<std::string> ranks = split_string(fen[0], '/');
	if (ranks.size() != state.board.size()) {
		// Returning null means error
		return std::nullopt;
	}
	for (unsigned short rank = 0; rank < 8; rank++) {
		unsigned short file = 0;
		for (char letter : ranks[rank]) {
			if (std::isdigit(letter)) {
				unsigned short ending_file = file + letter - '0';
				if (ending_file >= 8) {
					return std::nullopt;
				}
				while (file < ending_file) {
					state.board[rank][file] = std::nullopt;
				}
			} else {
				Piece new_piece;
				new_piece.color = std::isupper(letter) ? Player::White : Player::Black;
				switch (std::tolower(letter)) {
					case 'p':
						new_piece.type = PieceType::Pawn;
						break;
					case 'n':
						new_piece.type = PieceType::Knight;
						break;
					case 'b':
						new_piece.type = PieceType::Bishop;
						break;
					case 'r':
						new_piece.type = PieceType::Rook;
						break;
					case 'q':
						new_piece.type = PieceType::Queen;
						break;
					case 'k':
						new_piece.type = PieceType::King;
						break;
					default:
						return std::nullopt;
				}
				state.board[rank][file] = new_piece;
				if (++file >= 8) {
					return std::nullopt;
				}
			}
		}
	}

	// Player to move: fen[1]
	if (fen[1] == "w") {
		state.to_move = Player::White;
	} else if (fen[1] == "b") {
		state.to_move = Player::Black;
	} else {
		return std::nullopt;
	}

	// Castling rights: fen[2]
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
		}
	}

	// En Passant target square: fen[3]
	if (fen[3] != "-") {
		state.en_passant_target = string_to_position(fen[3]);
		if (!state.en_passant_target) {
			return std::nullopt;
		}
	}

	// Halfmove clock: fen[4]
	// Just check if it's a digit, but it's unused
	for (char x : fen[4]) {
		if (!std::isdigit(x)) {
			return std::nullopt;
		}
	}

	// Fullmove Counter: fen[5]
	// Just check if it's a digit, but it's unused

	for (char x : fen[5]) {
		if (!std::isdigit(x)) {
			return std::nullopt;
		}
	}
	

	return state;
}