/* ===========================================================================

	Project: Minimax AI player for Blokus

	Description:
	 Uses a minimax algorithm with alpha-beta pruning to select a move.

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
	Move makeMove( char grid[][20], bool pieces[][21], int score[], int player, std::vector<Move>& moveHistory );

	// Shutdown AI player
	void shutdown( ) { }

private:
	// Multi-threading game state structure
	struct MtGameState { short grid[14][14]; int pieces[2][3]; int score[2]; int player; 
						 int completed; float utility; int depth; float alpha; float beta; 
						 int moveIndex; };

	// Piece layout structures
	struct Piece { int sizeX, sizeY; int rot; int flip; char layout[7][6]; };
	struct Liberty { int x, y, a; bool flipped; };

	// Formatting function for reprocessing the board
	__forceinline static void reformatBoard( char boardIn[][20], short boardOut[][14],
						bool piecesIn[][21], int piecesOut[][3] );

	// Move selection function
	__forceinline static Move getMinimaxMove( short (*__restrict grid)[14], 
		int (*__restrict pieces)[3], int (*__restrict score), int player, int depth );
	__forceinline static Move getMinimaxMoveMultiThreaded( short (*__restrict grid)[14], 
		int (*__restrict pieces)[3], int (*__restrict score), int player, int depth );

	// Threaded move selection function
	static void getMinimaxUtility( void* dataOut );

	// Minimax function
	static float minimax( short (*__restrict grid)[14], int (*__restrict pieces)[3], int (*__restrict score), int player,
		int depth, float alpha, float beta );

	// Move enumeration functions
	__forceinline static int getMoveList( Move* __restrict moves, short (*__restrict grid)[14], int (*__restrict pieces)[3], int player );
	__forceinline static bool isValidMove( Move &move, short (*__restrict grid)[14], int player ); 
	__forceinline static int getMoveList_5pieces( Move* __restrict moves, short (*__restrict grid)[14], int (*__restrict pieces)[3], int player );

	// Move simulation function
	__forceinline static bool isMoveAvailable( short (*__restrict grid)[14], int (*__restrict pieces)[3], int player );
	__forceinline static void applyPiecePattern( Move move, short (*__restrict grid)[14], int playerBit, int i, int j, int gx, int gy );
	__forceinline static void simulateMove( Move &move, short (*__restrict grid)[14], int (*__restrict pieces)[3], int (*__restrict score), int player,
		short (*__restrict gridOut)[14], int (*__restrict piecesOut)[3], int (*__restrict scoreOut), int* __restrict playerOut );

	// Debugging helper functions 
	static void displayState( short grid[][14], int pieces[][3], int score[], int player );
	static void displayProfilerResults( float searchTime, int maxSearchDepth );

	// Profiler data members
	static unsigned int m_leavesSearched;
	static unsigned int m_nodesSearched;
	static __int64 m_timeCosts[10];

	// Minimax evaluation function
	static int m_evalFunction;

	// Game piece layout patterns
	static std::vector<Liberty> m_pieceLiberties[21];
	static void getPieceLiberties( );
	static void loadPieceConfigs( );

	// Match settings data
	static Timer m_matchTimer;
	static int m_nPlayers, m_boardSize;    
	static int m_startTile[4][2];
	static Piece m_piece[21];

	// Opening book
	static OpeningBook m_book;
};

// End definition
#endif


