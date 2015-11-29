/* ===========================================================================

	Project: Beam AI player for Blokus

	Description:
	  Simulates a move in a game state by updating all game state data used
	  by the minimax search algorithm. This includes board and piece state,
	  and the move list structure.

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

// Begin definition
#ifndef MOVE_SIMULATOR_H
#define MOVE_SIMULATOR_H

// Move simulator class
class MoveSimulator
{
public:
	// Reformats a board into extended format
	static void reformatBoard( char boardIn[][20], short boardOut[][14],
		bool piecesIn[][21], int piecesOut[], int startTile[][2] );

	// Simulates a move on an extended format board
	static void simulateMove( const Move* move, 
		short grid[][14], int pieces[], int score[], int player,
		short gridOut[][14], int piecesOut[], int scoreOut[], int* playerOut,
		MoveLists* movelists, MoveLists* movelistsOut );

protected:
	// Liberty location structure
	struct GridLiberty { int x, y, angle; };

	// Pattern applyer helper
	__forceinline static void applyPiecePattern( Piece* piece, MoveLists* moveList,
		short gridOut[][14], int player, int playerBit, int i, int j, int gx, int gy,
		int& nNewLiberties, GridLiberty newLiberties[] );
};

// End definition 
#endif