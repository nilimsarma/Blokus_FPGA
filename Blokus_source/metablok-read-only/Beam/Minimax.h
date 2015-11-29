/* ===========================================================================

	Project: Beam AI player for Blokus

	Description:
	  Uses a minimax algorithm to select a move within a given time limit.

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
#ifndef MINIMAX_H
#define MINIMAX_H

// Define player
class Minimax
{
public:
	// Initialize the AI players settings data
	void startup( int boardSize, int startTile[][2], int nPlayers );

	// Uses the standard minimax algorithm with alpha-beta pruning to make a move
	Move makeMove( char grid[][20], bool pieces[][21], int score[], 
		int player, int ply, Move moves[42] );

	// Shutdown AI player
	void shutdown( ) { }

private:
	// Multi-threading game state communication structure
	struct MtGameState { short grid[14][14]; int pieces[2]; int score[2]; int player; 
						 int completed; float utility; int depth; float alpha; float beta; 
						 const Move* moveIndex; MoveLists moveLists; int ply; };

	// Move selection function
	__forceinline static Move getMinimaxMove( short (*__restrict grid)[14], 
		int (*__restrict pieces), int (*__restrict score), int player, int depth, int ply );

	// Threaded move selection function
	static unsigned int __stdcall getMinimaxUtility( void* dataOut );

	// Minimax function
	static float minimax( MoveLists* moveLists, short (*__restrict grid)[14], int (*__restrict pieces), int (*__restrict score), int player,
		int depth, int ply, float alpha, float beta );

	// Debugging helper functions 
	static void displayState( MoveLists* moves, short grid[][14], 
		int pieces[], int score[], int player );

	// Minimax evaluation function
	static int m_evalFunction[2];

	// Minimax cut-off timer
	static Timer m_matchTimer;  
	static int m_startTile[4][2];

	// Opening book
	static OpeningBook m_book;
};

// End definition
#endif


