#include <iostream>
#include <vector>
#include <string>
#include <array>

#include "game_state.hpp"

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
		static_cast<unsigned short>(8 - (str[1] - '0')),
		static_cast<unsigned short>(str[0] - 'a')
	};
	if (square.file >= 8 || square.rank >= 8) { // Unsigned so no need to compare below 0
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
	for (int rank = 0; rank < 8; rank++) {
		int file = 0;
		for (char letter : ranks[rank]) {
			if (std::isdigit(letter)) {
				// Skip this many squares
				file += letter - '0';
				if (file > 8) {
					throw ParseError{};
				}
			} else {
				state.board[rank][file] = Piece{
					letter_to_piece_type(letter),
					std::isupper(letter) ? Player::White : Player::Black
				};
				if (++file > 8) {
					throw ParseError{};
				}
			}
		}

		if (file < 8) {
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

// Moves

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

void make_move_unsafe(GameState& state, const Move& move) {
	// THIS IS UNSAFE so moves must be valid
	auto& starting_piece = state.board[move.start_position.rank][move.start_position.file];
	auto& ending_piece = state.board[move.end_position.rank][move.end_position.file];
	if (move.promotion_piece) {
		ending_piece = Piece{
			move.promotion_piece.value(),
			starting_piece.value().color
		};
	} else {
		ending_piece = starting_piece;
	}
	starting_piece = std::nullopt;
	// Switch the turn
	state.to_move = state.to_move == Player::White ? Player::Black : Player::White;
}

std::vector<Move> get_all_moves(const GameState& state, const bool get_controlling_squares = false) {

	// get_controlling_squares provides some modifications that are useful for determining where the enemy King can move

	std::vector<Move> moves;

	for (int rank = 0; rank < BOARD_SIZE; rank++) {
		for (int file = 0; file < BOARD_SIZE; file++) {
			// Piece must be color of player to move
			if (state.board[rank][file] && state.board[rank][file].value().color == state.to_move) {

				/*
				This is the meat-and-potatoes of the entire program, where the rules of Chess are
				explicitly expressed. Here we're looping through all the pieces on the board and
				generating a list of moves for each of them.

				Important note: "up" the chessboard corresponds to a lesser `rank` value.

				Variable Value       0  1  2  3  4  5  6  7
				Rank On Chessboard   8  7  6  5  4  3  2  1
				File On Chessboard   a  b  c  d  e  f  g  h
				*/

				Piece piece = state.board[rank][file].value();

				auto add_move = [&](int ending_rank, int ending_file, std::optional<PieceType> promotion = std::nullopt) {
					moves.push_back(Move{
						Position{
							static_cast<unsigned short>(rank),
							static_cast<unsigned short>(file)
						},
						Position{
							static_cast<unsigned short>(ending_rank),
							static_cast<unsigned short>(ending_file)
						},
						promotion
					});
				};

				auto move_search_in_direction = [&](const int rank_direction, const int file_direction) {
					int new_rank = rank;
					int new_file = file;
					while (0 < rank < BOARD_SIZE - 1 && 0 < file < BOARD_SIZE - 1) {
						new_rank += rank_direction;
						new_file += file_direction;
						auto square = state.board[new_rank][new_file];
						if (square) {
							if (square.value().type != piece.type) {
								// Piece may capture
								add_move(new_rank, new_file);
							}
							// Piece is blocked by another and has no further moves in this direction
							continue;
						} else {
							// Square is empty
							add_move(new_rank, new_file);
						}
					}
				};

				switch (piece.type) {

					case PieceType::Pawn:
					// TODO pawns of different colors
						if (rank == 0) {
							// This should never happen in a normal chess game
							continue;
						}
						// Pawns may move up one square if the square is empty
						if (!get_controlling_squares && !state.board[rank - 1][file]) {
							if (rank - 1 == 0) {
								// If it's moving to the eighth rank, the pawn promotes
								add_move(0, file, PieceType::Knight);
								add_move(0, file, PieceType::Bishop);
								add_move(0, file, PieceType::Rook);
								add_move(0, file, PieceType::Queen);
							} else {
								add_move(rank - 1, file);
							}
						}
						// Pawns may move up two if they are on the starting rank
						if (!get_controlling_squares && rank == 6 && !state.board[rank - 1][file] && !state.board[rank - 2][file]) {
							add_move(4, file);
						}
						// Pawns may capture diagonally up and to the left
						if (
							file > 0
							&& (
								get_controlling_squares
								|| (
									state.board[rank - 1][file - 1]
									// The pieces are of opposite color
									&& state.board[rank - 1][file - 1].value().color != piece.color
								)
							)
						) {
							add_move(rank - 1, file - 1);
						}
						// Pawns may capture diagonally up and to the right
						if (
							file < BOARD_SIZE - 1
							&& (
								get_controlling_squares
								|| (
									state.board[rank - 1][file + 1]
									// The pieces are of opposite color
									&& state.board[rank - 1][file + 1].value().color != piece.color
								)
							)
						) {
							add_move(rank - 1, file + 1);
						}
						break;
					
					case PieceType::Knight:
						const int knight_moves[8][2] = {
							{ 1,  2},
							{ 2,  1},
							{ 1, -2},
							{ 2, -1},
							{-1,  2},
							{-2,  1},
							{-1, -2},
							{-2, -1}
						};
						for (auto& knight_move : knight_moves) {
							const int new_rank = rank + knight_move[0];
							const int new_file = file + knight_move[1];
							if (
								0 <= new_rank < BOARD_SIZE
								&& 0 <= new_file < BOARD_SIZE
								// You can't move onto your own piece
								&& !(
									state.board[new_rank][new_file]
									&& state.board[new_rank][new_file].value().color == piece.color
								)
							) {
								add_move(new_rank, new_file);
							}
						}
						break;
					
					case PieceType::Bishop:
						// Diagonal moves
						move_search_in_direction(1, 1);
						move_search_in_direction(1, -1);
						move_search_in_direction(-1, 1);
						move_search_in_direction(-1, -1);
						break;

					case PieceType::Rook:
						// Horizontal/vertical moves
						move_search_in_direction(1, 0);
						move_search_in_direction(0, 1);
						move_search_in_direction(-1, 0);
						move_search_in_direction(0, -1);
						break;

					case PieceType::Queen:
						// Combined moves of rook and bishop
						move_search_in_direction(1, 1);
						move_search_in_direction(1, -1);
						move_search_in_direction(-1, 1);
						move_search_in_direction(-1, -1);
						move_search_in_direction(1, 0);
						move_search_in_direction(0, 1);
						move_search_in_direction(-1, 0);
						move_search_in_direction(0, -1);
					
					case PieceType::King:
						// Must obtain a list of squares where the King would be in check.
						std::vector<Position> squares_in_check;
						if (!get_controlling_squares) {
							GameState reversed_state = state;
							reversed_state.to_move = state.to_move == Player::White ? Player::Black : Player::White;
							// Must set get_controlling_squares = true to avoid infinite loop.
							for (Move enemy_move : get_all_moves(reversed_state, true)) {
								squares_in_check.push_back(enemy_move.end_position);
							}
						}
						for (int rank_direction = -1; rank_direction <= 1; rank_direction++) {
							for (int file_direction = -1; file_direction <= 1; file_direction++) {
								const int new_rank = rank + rank_direction;
								const int new_file = file + file_direction;
								if (
									(rank_direction == 0 && file_direction == 0)
									|| new_rank < 0
									|| new_file < 0
									|| new_rank >= BOARD_SIZE
									|| new_file >= BOARD_SIZE
									|| (state.board[new_rank][new_file] && state.board[new_rank][new_file].value().color == piece.color)
								) {
									// May not stay still
									// May not move off edge of board
									// May not move into own piece
									continue;
								}
								add_move(new_rank, new_file);
							}
						}
						break;
					
				}

			}
		}
	}
}