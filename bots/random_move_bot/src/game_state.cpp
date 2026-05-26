#include <iostream>
#include <vector>
#include <string>
#include <array>

#include "game_state.hpp"
#include "util.hpp"


void make_move_unsafe(GameState& state, const Move& move) {
	auto& starting_piece = state.board[move.start_position.rank][move.start_position.file];
	if (!starting_piece) {
		throw ParseError{};
	}
	auto& ending_piece = state.board[move.end_position.rank][move.end_position.file];

	// En passant target square
	if (starting_piece->color == Player::White && starting_piece->type == PieceType::Pawn && move.start_position.rank == 6 && move.end_position.rank == 4) {
		state.en_passant_target = Position{
			5,
			move.start_position.file
		};
	}
	else if (starting_piece->color == Player::Black && starting_piece->type == PieceType::Pawn && move.start_position.rank == 1 && move.end_position.rank == 3) {
		state.en_passant_target = Position{
			2,
			move.start_position.file
		};
	}
	else {
		state.en_passant_target.reset();
	}

	// Promote a pawn
	if (move.promotion_piece) {
		if (starting_piece->type != PieceType::Pawn) {
			throw ParseError{};
		}
		ending_piece = Piece{
			move.promotion_piece.value(), // Type
			starting_piece->color  // Color
		};
	}
	// All other moves
	else {
		// King moves
		CastlingAvailabilty& this_player_castling_rights = state.castling_rights[static_cast<size_t>(starting_piece->color)];
		if (starting_piece->type == PieceType::King) {
			// Castling moves
			if (move.start_position.file == 4 && (move.end_position.file == 6 || move.end_position.file == 2)) {
				// This is a castling move: Attempt to castle
				const bool isKingside = move.end_position.file == 6;
				const int starting_rook_file = isKingside ? 7 : 0;
				const int ending_rook_file = isKingside ? 5 : 3;
				const int castling_rank = starting_piece->color == Player::White ? 7 : 0;
				bool& can_castle = isKingside ? this_player_castling_rights.kingside : this_player_castling_rights.queenside;
				// Validate position
				if (
					// Make sure castling is available
					!can_castle
					// Make sure it happens on the right rank
					|| move.start_position.rank != castling_rank
					|| move.end_position.rank != castling_rank
					// Make sure there's a rook in the corner
					|| !state.board[castling_rank][starting_rook_file]
					|| !(state.board[castling_rank][starting_rook_file]->type == PieceType::Rook)
					|| !(state.board[castling_rank][starting_rook_file]->color == starting_piece->color)
				) {
					throw ParseError{};
				}
				// Move the rook
				state.board[castling_rank][ending_rook_file] = state.board[castling_rank][starting_rook_file];
				state.board[castling_rank][starting_rook_file].reset();
			}
			// King moves take away castling rights
			this_player_castling_rights.kingside = false;
			this_player_castling_rights.queenside = false;
		}

		// Any moves in corners (usually rooks) take away castling rights
		const int rook_rank = starting_piece->color == Player::White ? 7 : 0;
		if (move.start_position.rank == rook_rank) {
			if (move.start_position.file == 0) {
				this_player_castling_rights.queenside = false;
			} else if (move.start_position.file == BOARD_SIZE - 1) {
				this_player_castling_rights.kingside = false;
			}
		}
		if (move.end_position.rank == rook_rank) {
			if (move.end_position.file == 0) {
				this_player_castling_rights.queenside = false;
			} else if (move.end_position.file == BOARD_SIZE - 1) {
				this_player_castling_rights.kingside = false;
			}
		}
		
		ending_piece = starting_piece;
	}

	// The starting square is now empty
	starting_piece.reset();

	// Switch the turn
	state.to_move = other_player(state.to_move);
}


bool is_check(const GameState& state, const Player king_color) {

	const Player attacking_pieces_color = other_player(king_color);

	auto at = [&](Position pos) {
		return state.board[pos.rank][pos.file];
	};

	// First, find the king
	Position king_pos;
	bool king_found = false;
	for (int rank = 0; rank < BOARD_SIZE; rank++) {
		for (int file = 0; file < BOARD_SIZE; file++) {
			const Position check_pos = {rank, file};
			if (
				at(check_pos)
				&& at(check_pos)->color == king_color
				&& at(check_pos)->type == PieceType::King
			) {
				if (king_found) {
					// There are two kings on the board
					throw ParseError{};
				} else {
					king_pos = check_pos;
					king_found = true;
				}
			}
		}
	}
	if (!king_found) {
		throw ParseError{};
	}

	// From the perspective of the king, look for possible attacks
	// Pawn
	const int pawn_rank_advance = attacking_pieces_color == Player::White ? -1 : 1;
	const std::optional<Piece> pawn_to_left = at(Position{king_pos.rank - pawn_rank_advance, king_pos.file - 1});
	const std::optional<Piece> pawn_to_right = at(Position{king_pos.rank - pawn_rank_advance, king_pos.file + 1});
	if (
		// Rank is within bounds
		in_bounds(Position{king_pos.rank - pawn_rank_advance, king_pos.file})
		&& (
			// Check for a pawn to the left
			(
				king_pos.file > 0
				&& pawn_to_left
				&& pawn_to_left->color == attacking_pieces_color
				&& pawn_to_left->type == PieceType::Pawn
			)
			// Check for a pawn to the right
			|| (
				king_pos.file < BOARD_SIZE - 1
				&& pawn_to_right
				&& pawn_to_right->color == attacking_pieces_color
				&& pawn_to_right->type == PieceType::Pawn
			)
		)
	) {
		// King is in check
		return true;
	}

	// Knight
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
		const Position new_pos {
			king_pos.rank + knight_move[0],
			king_pos.rank + knight_move[1]
		};
		if (
			in_bounds(new_pos)
			&& at(new_pos)
			&& at(new_pos)->color == attacking_pieces_color
			&& at(new_pos)->type == PieceType::Knight
		) {
			// King is attacked by knight
			return true;
		}
	}

	// Bishop
	// Rook
	// Queen
	auto check_in_direction = [&](const int rank_direction, const int file_direction, const PieceType type) {
		Position new_pos = king_pos;
		while (true) {
			new_pos.rank += rank_direction;
			new_pos.file += file_direction;
			// Check for out of bounds
			if (!in_bounds(new_pos)) {
				return false;
			}
			if (at(new_pos)) {
				// The type and color of the piece determines whether the King is in check from that angle
				return (at(new_pos)->type == type || at(new_pos)->type == PieceType::Queen) && at(new_pos)->color == attacking_pieces_color;
			}
		}
	};

	if (
		   check_in_direction(1, 1, PieceType::Bishop)
		|| check_in_direction(1, -1, PieceType::Bishop)
		|| check_in_direction(-1, 1, PieceType::Bishop)
		|| check_in_direction(-1, -1, PieceType::Bishop)
		|| check_in_direction(0, 1, PieceType::Rook)
		|| check_in_direction(1, 0, PieceType::Rook)
		|| check_in_direction(-1, 0, PieceType::Rook)
		|| check_in_direction(0, -1, PieceType::Rook)
	) {
		return true;
	}

	// Queen is handled in function

	// Opposing king
	for (int rank_direction = -1; rank_direction <= 1; rank_direction++) {
		for (int file_direction = -1; file_direction <= 1; file_direction++) {
			const Position new_pos {
				king_pos.rank + rank_direction,
				king_pos.file + file_direction
			};
			if (
				in_bounds(new_pos)
				&& at(new_pos)
				&& at(new_pos)->type == PieceType::King
				&& at(new_pos)->color == attacking_pieces_color
			) {
				return true;
			}
		}
	}

	return false;
}


std::vector<Move> get_all_moves(const GameState& state) {

	std::vector<Move> moves;

	for (int rank = 0; rank < BOARD_SIZE; rank++) {
		for (int file = 0; file < BOARD_SIZE; file++) {

			auto at = [&](Position pos) {
				return state.board[pos.rank][pos.file];
			};

			// Piece must be color of player to move
			if (at(Position{rank, file}) && at(Position{rank, file})->color == state.to_move) {

				/*
				This is the meat-and-potatoes of the entire program, where the rules of Chess are
				explicitly expressed. Here we're looping through all the pieces on the board and
				generating a list of moves for each of them.

				Important note: "up" the chessboard corresponds to a lesser `rank` value.

				Variable Value       0  1  2  3  4  5  6  7
				Rank On Chessboard   8  7  6  5  4  3  2  1
				File On Chessboard   a  b  c  d  e  f  g  h
				*/

				Piece piece = at(Position{rank, file}).value();

				auto add_move = [&](const Position& ending_pos, const std::optional<PieceType>& promotion = std::nullopt) {
					Move new_move = Move{
						Position{rank, file},
						ending_pos,
						promotion
					};
					// make sure we're not in check
					GameState game_state_copy = state;
					make_move_unsafe(game_state_copy, new_move);
					// The move will be reversed during make_move_unsafe, which is necessary for the is_check function
					if (is_check(game_state_copy, state.to_move)) {
						return false;
					}
					// move is valid, so add to list
					moves.push_back(new_move);
					// DEBUG
					std::cerr << "Valid move: " << char('a' + file) << (8 - rank) << " to " << char('a' + ending_pos.file) << (8 - ending_pos.rank);
					if (promotion) {
						std::cerr << " promoting to " << (promotion == PieceType::Knight ? 'N' : promotion == PieceType::Bishop ? 'B' : promotion == PieceType::Rook ? 'R' : promotion == PieceType::Queen ? 'Q' : '?');
					}
					std::cerr << std::endl;
					// END DEBUG
					return true;
				};

				auto move_search_in_direction = [&](const int rank_direction, const int file_direction) {
					Position new_pos {
						rank,
						file
					};
					while (true) {
						new_pos.rank += rank_direction;
						new_pos.file += file_direction;
						if (!in_bounds(new_pos)) {
							return;
						}
						if (at(new_pos)) {
							if (at(new_pos)->color != piece.color) {
								// Piece may capture
								add_move(new_pos);
							}
							// Piece is blocked by another and has no further moves in this direction
							return;
						} else {
							// Square is empty
							add_move(new_pos);
						}
					}
				};

				switch (piece.type) {

					case PieceType::Pawn:
						{
							// Pawns have opposite move directions depending on color
							const int pawn_rank_advance = piece.color == Player::White ? -1 : 1;
							const int pawn_promotion_rank = piece.color == Player::White ? 0 : 7;
							// Check for invalid position
							if (rank == pawn_promotion_rank) {
								// This should never happen in a normal chess game: these pawns would have been promoted
								continue;
							}
							// Pawns may move up one square if the square is empty
							if (!at(Position{rank + pawn_rank_advance, file})) {
								if (rank + pawn_rank_advance == pawn_promotion_rank) {
									// If it's moving to the eighth rank, the pawn promotes
									add_move(Position{pawn_promotion_rank, file}, PieceType::Knight);
									add_move(Position{pawn_promotion_rank, file}, PieceType::Bishop);
									add_move(Position{pawn_promotion_rank, file}, PieceType::Rook);
									add_move(Position{pawn_promotion_rank, file}, PieceType::Queen);
								} else {
									add_move(Position{rank + pawn_rank_advance, file});
								}
							}
							// Pawns may move up two if they are on the starting rank
							if (
								rank == pawn_promotion_rank - pawn_rank_advance * (BOARD_SIZE - 2) // starting rank
								&& !at(Position{rank + pawn_rank_advance, file})
								&& !at(Position{rank + pawn_rank_advance + pawn_rank_advance, file})
							) {
								add_move(Position{rank + pawn_rank_advance + pawn_rank_advance, file});
							}
							// Pawns may capture diagonally up and to the left
							const Position left_capture_square {rank + pawn_rank_advance, file - 1};
							if (
								file > 0
								&& at(left_capture_square)
								// The pieces are of opposite color
								&& at(left_capture_square)->color != piece.color
							) {
								add_move(left_capture_square);
							}
							// Pawns may capture diagonally up and to the right
							const Position right_capture_square {rank + pawn_rank_advance, file + 1};
							if (
								file < BOARD_SIZE - 1
								&& at(right_capture_square)
								// The pieces are of opposite color
								&& at(right_capture_square)->color != piece.color
							) {
								add_move(right_capture_square);
							}
							// Pawns may capture en passant
							if (state.en_passant_target) {
								// The target square must be diagonally forward from the pawn
								if (
									state.en_passant_target->rank == rank + pawn_rank_advance
									&& (
										state.en_passant_target->file == file + 1
										|| state.en_passant_target->file == file - 1
									)
								) {
									add_move(state.en_passant_target.value());
								}
							}
						}
						break;
					
					case PieceType::Knight:
						{
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
								const Position new_pos {
									rank + knight_move[0],
									file + knight_move[1]
								};
								if (
									in_bounds(new_pos)
									// You can't move onto your own piece
									&& !(at(new_pos) && at(new_pos)->color == piece.color)
								) {
									add_move(new_pos);
								}
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
						break;
						
					case PieceType::King:
						{
							const bool check = is_check(state, state.to_move);
							// Normal King moves
							bool kingside_castle_unblocked, queenside_castle_unblocked;
							for (int rank_direction = -1; rank_direction <= 1; rank_direction++) {
								for (int file_direction = -1; file_direction <= 1; file_direction++) {
									const Position new_pos {
										rank + rank_direction,
										file + file_direction
									};
									if (
										(rank_direction == 0 && file_direction == 0)
										|| !in_bounds(new_pos)
										|| (at(new_pos) && at(new_pos)->color == piece.color)
									) {
										// May not stay still
										// May not move off edge of board
										// May not move into own piece
										continue;
									}
									bool success = add_move(new_pos);
									if (rank_direction == 0 && file_direction == 1) {
										kingside_castle_unblocked = success;
									} else if (rank_direction == 0 && file_direction == -1) {
										queenside_castle_unblocked = success;
									}
								}
							}
							const CastlingAvailabilty& this_player_castling_rights = state.castling_rights[static_cast<size_t>(state.to_move)];
							// Kingside castling
							if (
								this_player_castling_rights.kingside // Must have castling rights
								&& !check // Can't castle out of check
								&& kingside_castle_unblocked // Ensures there's no check there
								&& !at(Position{rank, 5}) // Squares between king and rook must be empty
								&& !at(Position{rank, 6})
							) {
								add_move(Position{rank, 6});
							}
							// Queenside castling
							if (
								this_player_castling_rights.queenside // Must have castling rights
								&& queenside_castle_unblocked // Ensures there's no check there
								&& !check // Can't castle out of check
								&& !at(Position{rank, 1}) // Squares between king and rook must be empty
								&& !at(Position{rank, 2})
								&& !at(Position{rank, 3})
							) {
								add_move(Position{rank, 2});
							}
						}
						break;
					
				}

			}
		}
	}

	return moves;
}