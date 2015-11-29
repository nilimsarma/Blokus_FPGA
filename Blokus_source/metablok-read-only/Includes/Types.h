/* ===========================================================================

	Project: AI player for Blokus

	Description:
	 Define the basic communication structures used by AI players to 
	 communicate with the simulator.

    Copyright (C) 2011 Lucas Sherman, David Gloe, Mary Southern, Tobias Gulden

	Lucas Sherman, email: LucasASherman@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================== */

// Begin definitions
#ifndef TYPES_H
#define TYPES_H

// Move selection structure with simple construct/compare ops
struct Move { int pieceNumber, gridX, gridY, flipped, rotated; 
Move(int p, int x, int y, int f, int r) :
pieceNumber(p), gridX(x), gridY(y), flipped(f), rotated(r) { }
Move( ) { } 
			  bool operator==(const Move &b)
			  { return ( (pieceNumber==b.pieceNumber) &&
						 (gridX==b.gridX) &&
						 (gridY==b.gridY) &&
						 (flipped==b.flipped) &&
						 (rotated==b.rotated) ); } };

// AI memory map
struct GameData 
{ 
	// Match config
	int startTile[4][2];	//< Starting tiles
	int nPlayers;			//< Number of players
	int boardSize;			//< Size of the board 
	int player;				//< AIs player ID

	// Board state
	Move moveHistory[42];   //< Previous moves
	char board[20][20];		//< Board array
	bool pieces[4][21];		//< Pieces array
	int score[4];			//< Array of scores
	int ply;				//< Current ply
	
	// Return data
	float timeout;	//< End time for AI move
	int matchOver;  //< Match over indicator
	int moveReady;	//< AI move selected 
	int turnReady;	//< AI turn started
	Move move;		//< Selected move
};

// Game modes
#define MODE_CLASSIC	0
#define MODE_DUO		1

// Player definitions
#define PLAYER_BLUE   0
#define PLAYER_RED    1
#define PLAYER_GREEN  2
#define PLAYER_YELLOW 3
#define PLAYER_NONE   4

// Grid definitions
#define GRID_COVER_BLUE   0
#define GRID_COVER_RED    1
#define GRID_COVER_GREEN  2
#define GRID_COVER_YELLOW 3
#define GRID_COVER_NONE	  4

// Rotation angles (ccw)
#define PIECE_ROTATE_0	  0
#define PIECE_ROTATE_90	  1
#define PIECE_ROTATE_180  2
#define PIECE_ROTATE_270  3

// Flip property
#define PIECE_UNFLIPPED 0
#define PIECE_FLIPPED   1

// Piece pattern layouts
#define MATCH_DONT_CARE   '0'
#define MATCH_NOT_PLAYERS '1'
#define MATCH_NOT_COVERED '2'
#define MATCH_LIBERTY     '3'

// End definition
#endif