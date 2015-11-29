/* ===========================================================================

	Project: Beam AI player for Blokus

	Description:
	  Move list structure for managing lists of moves for a given board state
	  across simulations.

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
#ifndef MOVE_LISTS_H
#define MOVE_LISTS_H

// Maximum fighting distance
#define FIGHT_DIST    5	

// Dependacy values
#define NUM_MOVES  126
#define D FIGHT_DIST
#define UTSZ 2*D*(D+1)+1

// Move list structure
class MoveList 
{ 
	friend class MoveLists;
public:
	// Doubly-linked list traversal
	__forceinline MoveList* getNext( ) { return next; }
	__forceinline MoveList* getPrev( ) { return prev; }

	// Move List and mode accessors
	__forceinline int getValidPieces( ) { return validPieces; }
	__forceinline int isFighting( ) { return isAwake; }
	__forceinline int isLeak( ) { return isLeakLbty; }
	__forceinline int getPositionX( ) { return x; }
	__forceinline int getPositionY( ) { return y; }
	__forceinline int getAngle( ) { return angle; }
	__forceinline int getNumMoves( ) 
	{
		int moves = 0;
		for( int i = 0; i < NUM_MOVES; i++ )
			if( isValid[i] ) moves++;
		return moves;
	}

	// Compute liberty free space
	// :TODO: Keep count variable if 
	//  this proves to be a good heuristic
	//  element.
	__forceinline int getFreeSpace( ) 
	{
		int space = 0;
		for( int i = 0; i < UTSZ; i++ )
			if( updateTable[i].paths ) space++;
		return space;
	}

	// Debug mode display
	void displayInfo( );

private:
	struct PathEntry { char pathlen; char paths; };
	MoveList( ) { } // Privatized constructor

	MoveList *next, *prev;				// List connectors
	PathEntry updateTable[UTSZ];		// Path cost table
	int x, y, angle, isAwake;			// Liberty information
	Move moves[NUM_MOVES];				// List of possible moves
	int isValid[NUM_MOVES];				// List of valid moves
	int validPieces;					// Piece update table
	int isLeakLbty;						// Leak liberty flag
	int generated;						// Move list generated flag
	int update;							// Update on regen flag
};

// Move Lists class
class MoveLists
{
public:
	// Contruction and Initialization
	MoveLists( ) { m_memoryPool = NULL; 
		for( int i = 0; i < NUM_PLAYERS; i++ ) 
			m_moveList[i] = NULL; }

	// Memory pool allocation and deallocation
	void allocateMemoryPool( int nChunks );
	void deallocateMemoryChunks( );
	void deallocateMemoryPool( );

	// Copy constructor for move simulation
	void copy( const MoveLists* original );

	// Initial board state move generation
	void generateMoves( short grid[][14], int pieces[] );

	// Liberty list management procedures
	void markUnsafeTile( int bx, int by, int player );
	void makeLiberty( int bx, int by, int a, int player, 
		short grid[][14], int pieces[] );

	// Updates liberty mode settings
	void clearLibertyModeSettings( );
	void detectFightingLiberties( );

	// Update liberty move lists
	void updateLiberties( short grid[][14], 
		int pieces[], int player );

	// No moves detection
	int isMoveAvailable( int player )
	{ return m_moveList[player] ? TRUE : FALSE; }

	// Move list iterators
	const Move* getFirstMove( int player, int pieces );
	const Move* getNextMove( );

	// List accessors
	MoveList* getList( int player )
	{ return m_moveList[player]; }

protected:
	// Update table entry indices
	struct TableIndex { int x, y, i; };

	// Pointer to memory pool
	MemoryPool *__restrict m_memoryPool;

	// Pointer to move lists
	MoveList *__restrict m_moveList[NUM_PLAYERS];

	// Liberty updating data
	static int m_pieceRange[PIECE_COUNT+1];
	static int m_pieceSizes[PIECE_COUNT];
	static Move m_moves[NUM_MOVES];

	// Internal iterator pointers
	MoveList *__restrict m_nextMoveList;
	int m_nextPieceIndex, m_nextMoveIndex;
	int m_validPieces;

	// Generates the possible moves table
	void generateReferenceMoveList( );

	// Removes a liberty from the lists
	__forceinline MoveList* killLiberty( MoveList* liberty, int player );

	// Generates the liberty update table
	__forceinline void initLibertyUpdateTable( MoveList* liberty, int playerBit, short grid[][14] );

	// Updates the leak flag for a liberty
	__forceinline void updateLeakFlag( MoveList* liberty, short grid[][14] );

	// Updates the liberties move lists
	__forceinline int updateLiberty( MoveList* liberty, int player, 
		int dist, short grid[][14], int pieces[] );

	// Move validation helper
	bool isValidMove( Move &move, 
		short (*__restrict grid)[14], short player ); 
};

// End definition 
#endif