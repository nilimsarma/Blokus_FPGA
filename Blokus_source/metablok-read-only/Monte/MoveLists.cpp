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

// Standard includes
#include "Includes.h"

// Include header
#include "MoveLists.h"

// Piece cost table index computation
#define INDEX_OF( x, y ) ( D*(D+1) + y*(2*D+1) - abs(y)*y + x )

// Static data members
int MoveLists::m_pieceRange[PIECE_COUNT+1];
int MoveLists::m_pieceSizes[PIECE_COUNT];
Move MoveLists::m_moves[NUM_MOVES];

// --------------------------------------------------------
//  Copy - Creates a copy of the specified MoveList using
//  the calling MoveList. If the MoveList does not have its
//  own memory pool it will inherit from the original.
// --------------------------------------------------------
void MoveLists::copy( const MoveLists* original )
{
	// Begin profiling list copy time
	__int64 listsCopyTimeID = Profiler::startProfile( );

	// Inherit original memory pool
	if( !m_memoryPool ) m_memoryPool = original->m_memoryPool;

	// Set iterator variables
	m_nextMoveList = NULL;
	m_nextMoveIndex = 0;

	// Copy list structures
	for( int p = 0; p < NUM_PLAYERS; p++ )
	{
		// Check for empty list
		if( !original->m_moveList[p] ) {
			m_moveList[p] = NULL;
			continue; }

		// Copy list heads
		MoveList* head = original->m_moveList[p];
		MoveList* newHead = (MoveList*)m_memoryPool->getChunk( );
		memcpy( newHead, head, sizeof(MoveList) );
		m_moveList[p] = newHead;
		newHead->prev = NULL;

		// Copy list internals
		MoveList* iterator = head->next;
		MoveList* previous = newHead;
		while( iterator )
		{
			// Allocate a new chunk
			MoveList* copy = (MoveList*)m_memoryPool->getChunk( );
			memcpy( copy, iterator, sizeof(MoveList) );

			// Repair list structure
			copy->prev = previous;
			copy->prev->next = copy;

			// Increment iterator
			iterator = iterator->next;
			previous = copy;
		}

		// Append null terminator
		previous->next = NULL;
	}
	
	// Stop profiling copy time
	Profiler::endProfile( tCopyLibs, listsCopyTimeID );
}
// 
// --------------------------------------------------------
//  AllocateMemoryPool - Allocates a memory pool for this
//  MoveLists. Deallocate memory must be explicitly called 
//  to free the pool.
// --------------------------------------------------------
void MoveLists::allocateMemoryPool( int nChunks )
{
	// Create a new memory pool
	m_memoryPool = new MemoryPool( );

	// Allocate a small amount of memory for pool
	m_memoryPool->allocateMemory( sizeof(MoveList), nChunks );
}
//
// --------------------------------------------------------
//  DeallocateMemory - Frees the memory allocated by the
//  internal memory pool.
// --------------------------------------------------------
void MoveLists::deallocateMemoryPool( )
{
	if( m_memoryPool )
	{
		m_memoryPool->deallocateMemory( );
		delete m_memoryPool;
		m_memoryPool = NULL;
	}
}
//
// --------------------------------------------------------
//  DeallocateMemoryChunks - Returns allocated chunks of
//  memory to the internal memory pool.
// --------------------------------------------------------
void MoveLists::deallocateMemoryChunks( )
{
	for( int i = 0; i < NUM_PLAYERS; i++ )
	{
		MoveList* iterator = m_moveList[i];
		while( iterator )
		{
			MoveList* next = iterator->next;
			m_memoryPool->freeChunk( iterator );
			iterator = next;
		}
	}
}
//
// --------------------------------------------------------
//  GenerateMoves - Generates initial move lists for the 
//  specified board configuration. Updates the reformatted
//  board liberty squares for any dead liberties.
// --------------------------------------------------------
void MoveLists::generateMoves( short grid[][14], int pieces[] )
{
	// Generate reference moves
	generateReferenceMoveList( );

	// Initialize move list pointers to NULL
	m_nextMoveList = NULL; m_moveList[0] = NULL; m_moveList[1] = NULL;

	// Find any possible live board liberties
	for( int p = 0; p < NUM_PLAYERS; p++ )       //< Cycle through players
	for( int bx = 0; bx < BOARD_SIZE; bx++ )     //< Cycle through x tiles
	for( int by = 0; by < BOARD_SIZE; by++ )     //< Cycle through y tiles
	for( int a = 0; a < 4; a++ )			     //< Cycle through orients
	if( grid[bx][by]&(1<<(p+EX_LBTY_ANGLE(a))) ) //< Check for the liberty
	{
		// Dead liberty flag
		bool deadLiberty = TRUE;

		// Get player bit for demasking
		int playerBit = 1 << (p+EX_GRID_NOT_SAFE);

		// Get a list block for the liberty data
		MoveList* nextLib = (MoveList*)m_memoryPool->getChunk( );
		
		// Copy the piece move information into the block
		memcpy( (void*)nextLib->moves, (void*)m_moves, sizeof(Move)*NUM_MOVES );

		// Set liberty's valid piece table
		nextLib->validPieces = pieces[p];

		// Cycle through available pieces
		for( int i = 0; i < PIECE_COUNT; i++ ) 
		{
			// Get handle to piece object
			Piece* piece = PieceSet::getPiece( i );

			// Get dimensions of piece
			int w = piece->getSizeX( ) - 1;
			int h = piece->getSizeY( ) - 1;

			// Dead piece identifier
			int deadPiece = TRUE;

			// Cycle through the pieces range
			for( int j = m_pieceRange[i+1]; 
					 j < m_pieceRange[i]; 
					 j++ )
			{
				// Get reference to move
				Move& move = nextLib->moves[j];

				// Compute rotation angle
				int rotation = (a - move.rotated + 6)%4;
				if( move.flipped ) move.rotated = (4 - rotation)%4;
				else move.rotated = rotation;

				// Piece offsets
				int sx, x, sy, y;

					 // Compute grid offsets based on the angle and dimensions
					 if( rotation == 0 ) { x = move.gridX; y = move.gridY; sx = w; sy = h;     }
				else if( rotation == 1 ) { x = move.gridY; y = w-move.gridX; sx = h; sy = w;   }
				else if( rotation == 2 ) { x = w-move.gridX; y = h-move.gridY; sx = w; sy = h; }
				else if( rotation == 3 ) { x = h-move.gridY; y = move.gridX; sx = h; sy = w;   }

				// Check if the piece is valid
				if( !(pieces[p] & (1<<i)) ) {
					nextLib->isValid[j] = FALSE;
					continue; }

				// Check for invalid grid locations
				move.gridX = bx - x; move.gridY = by - y;
				if( move.gridX < -1 || move.gridY < -1  || 
					move.gridX > BOARD_SIZE - sx || 
					move.gridY > BOARD_SIZE - sy ) {
					nextLib->isValid[j] = FALSE;
					continue; }

				// Profile move validation time
				__int64 validationTimeID = Profiler::startProfile( );
				int isValid = isValidMove( move, grid, playerBit );
				Profiler::endProfile( tMoveValidation, validationTimeID );

				// Check if the move is valid
				if( !isValid )nextLib->isValid[j] = FALSE;
				else {
					nextLib->isValid[j] = TRUE;
					deadLiberty = FALSE; 
					deadPiece = FALSE; }
			}
		
			// Check for dead piece identifier still flagged
			if( deadPiece ) nextLib->validPieces &= ~(1<<i);
		}

		// Initialize liberty settings
		if( !deadLiberty ) 
		{ 
			// Store the liberty coordinates
			nextLib->x = bx; nextLib->y = by;
			nextLib->angle = a; 

			// Check for a leak opportunity
			updateLeakFlag( nextLib, grid );

			// Insert the liberty into the list
			if( m_moveList[p] ) m_moveList[p]->prev = nextLib;
			nextLib->next = m_moveList[p];
			m_moveList[p] = nextLib;
			nextLib->prev = NULL;

			// Generate the liberty update table
			initLibertyUpdateTable( nextLib, 1<<p<<EX_GRID_NOT_SAFE, grid );

			// Break from the angle loop
			// (Only 1 liberty per square)
			break;
		} 
		else 
		{
			// Get general mask bit
			int playerMask = 1 << p;

			// Return the chunk to the memory pool
			m_memoryPool->freeChunk( nextLib );

			// Mask off any active liberty bits on the grid tile
			grid[bx][by] &= ~( (playerMask<<EX_GRID_LBTY_UR)|(playerMask<<EX_GRID_LBTY_UL)|
						   	   (playerMask<<EX_GRID_LBTY_LL)|(playerMask<<EX_GRID_LBTY_LR) );
		}
	}

	// Detect initial fighting set
	detectFightingLiberties( );
}
//
// --------------------------------------------------------
//  GenerateReferenceMoveList - Generates a reference table
//  with all possible moves for a single liberty in order
//  from smallest to largest number of tiles. The rotation
//  component of the move corresponds to an UR liberty.
// --------------------------------------------------------
void MoveLists::generateReferenceMoveList( )
{
	int nextMoveIndex = 0;

	// Cycle through available pieces
	for( int i = PIECE_COUNT-1; i >= 0; i-- )
	{
		// Set piece start index
		m_pieceRange[i+1] = nextMoveIndex;

		// Get handle to piece object
		Piece* piece = PieceSet::getPiece( i );

		// Cycle through piece liberties
		for( int j = 0; j < piece->getNumOfLiberties( ); j++ )
		{
			// Get liberty object handle
			Liberty* liberty = piece->getLiberty( j );

			// Initialize fields of new move object
			Move& move = m_moves[nextMoveIndex];
			move.flipped = liberty->isFlipped( );
			move.rotated = liberty->getAngle( );
			move.pieceNumber = i;

			// Compute base position offset
			int px = liberty->getPosX( );
			int py = liberty->getPosY( );

				 // Compute position of solid piece square
				 if( move.rotated == 0 ) { px--; py++; }
			else if( move.rotated == 1 ) { px++; py++; }
			else if( move.rotated == 2 ) { px++; py--; }
			else if( move.rotated == 3 ) { px--; py--; }

			// Initialize relative move position
			move.gridX = px; move.gridY = py;

			// Increment counter
			nextMoveIndex++;
		}
	}

	// Verify expected move count
	ASSERT( nextMoveIndex == NUM_MOVES )

	// Set terminal index
	m_pieceRange[0] = nextMoveIndex;

	// Piece extension widths
	m_pieceSizes[PIECE_1]  = 0;
	m_pieceSizes[PIECE_2]  = 1;
	m_pieceSizes[PIECE_I3] = 2;
	m_pieceSizes[PIECE_V3] = 2;
	m_pieceSizes[PIECE_T4] = 3;
	m_pieceSizes[PIECE_Z4] = 3;
	m_pieceSizes[PIECE_I4] = 3;
	m_pieceSizes[PIECE_L4] = 3;
	m_pieceSizes[PIECE_O]  = 2;
	m_pieceSizes[PIECE_P]  = 3;
	m_pieceSizes[PIECE_U]  = 3;
	m_pieceSizes[PIECE_F]  = 3;
	m_pieceSizes[PIECE_I5] = 4;
	m_pieceSizes[PIECE_L5] = 4;
	m_pieceSizes[PIECE_V5] = 4;
	m_pieceSizes[PIECE_N]  = 4;
	m_pieceSizes[PIECE_W]  = 4;
	m_pieceSizes[PIECE_Z5] = 4;
	m_pieceSizes[PIECE_Y]  = 3;
	m_pieceSizes[PIECE_T]  = 3;
	m_pieceSizes[PIECE_X]  = 2;
}
//
// --------------------------------------------------------
//	ClearLibertyModeSettings - Initializes the mode settings
//  of all liberties to fighting.
// --------------------------------------------------------
void MoveLists::clearLibertyModeSettings( )
{
	for( int i = 0; i < NUM_PLAYERS; i++ )
	{
		MoveList* iterator;
		for( iterator = m_moveList[i]; 
			iterator != NULL; 
			iterator = iterator->next )
		{
			iterator->isAwake = TRUE;
		}
	}
}
//
// --------------------------------------------------------
//	IsValidMove - Checks if the given move is valid on the 
//  preconditions that the move will cover a libery. Returns 
//  true if the move is valid, false otherwise.
// --------------------------------------------------------
bool MoveLists::isValidMove( Move &move, short grid[][14], short player )
{
	// Get handle to piece object
	Piece* piece = PieceSet::getPiece( move.pieceNumber );

	// Get piece iteration bounds
	int x = piece->getSizeX( ) - 1;
	int y = piece->getSizeY( ) - 1;

	// Run pattern analysis between grid and piece
	if( move.flipped == PIECE_UNFLIPPED )
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j = 1, gy = move.gridY+1; j < y; j++,gy++ )
			for( int i = 1, gx = move.gridX+1; i < x; i++,gx++ )
				if( piece->getLayout( i, j ) == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY+1; i > 0; i--,gy++ )
			for( int j =   1, gx = move.gridX+1; j < y; j++,gx++ )
				if( piece->getLayout( i, j ) == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY+1; j > 0; j--,gy++ )
			for( int i = x-1, gx = move.gridX+1; i > 0; i--,gx++ )
				if( piece->getLayout( i, j ) == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i =   1, gy = move.gridY+1; i < x; i++,gy++ )
			for( int j = y-1, gx = move.gridX+1; j > 0; j--,gx++ )
				if( piece->getLayout( i, j ) == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }
	} else 
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j =   1, gy = move.gridY+1; j < y; j++,gy++ )
			for( int i = x-1, gx = move.gridX+1; i > 0; i--,gx++ )
				if( piece->getLayout( i, j ) == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY+1; i > 0; i--,gy++ )
			for( int j = y-1, gx = move.gridX+1; j > 0; j--,gx++ )
				if( piece->getLayout( i, j ) == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY+1; j > 0; j--,gy++ )
			for( int i =   1, gx = move.gridX+1; i < x; i++,gx++ )
				if( piece->getLayout( i, j ) == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i = 1, gy = move.gridY+1; i < x; i++,gy++ )
			for( int j = 1, gx = move.gridX+1; j < y; j++,gx++ )
				if( piece->getLayout( i, j ) == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }
	}

	// Return valid
	return true;
}
//
// --------------------------------------------------------
//  GetFirst - Returns the first available move in the move
//  list based on the ordering heuristic. If no moves 
//  exist, return value is NULL.
// --------------------------------------------------------
const Move* MoveLists::getFirstMove( int player, int pieces )
{
	//////////////////////////////////////////
	//       MOVE ORDERING PRIORITIES		//
	//////////////////////////////////////////
	//										//
	// (1) Liberties with the greatest		//
	//     x-coordinate are selected		//
	// (2) Liberties with the greatest		//
	//	   y-coordinate are selected		//
	// (3) Ignore sleeping liberties		//
	// (4) Pieces with higher indexes		//
	//     are selected						//
	// (5) Moves which do not represent a 	//
	//     piece flipped from default   	//
	//     orientation are selected			//
	// (6) Moves with the least rotations	//
	//     from default orientation are		//
	//     selected							//
    //										//
	//////////////////////////////////////////

	// (1) Liberties with the greatest
	//     x-coordinate are selected
	// (2) Liberties with the greatest
	//     x-coordinate are selected
	m_nextMoveList = m_moveList[player]; 

	// (3) Ignore sleeping liberties
	while( !m_nextMoveList->isAwake )
	if( !(m_nextMoveList = m_nextMoveList->next ) ) 
		return NULL;

	// (4) Pieces with higher indexes	
	//     are selected	
	m_nextPieceIndex = PIECE_COUNT-1;

	// Get bitset of pieces available for selection
	int validPieces = pieces & m_nextMoveList->validPieces;
	m_validPieces = pieces;

	// Select the first valid piece found when searching
	while( !( (1 << m_nextPieceIndex) & validPieces ) )
	{ m_nextPieceIndex--; ASSERT( m_nextPieceIndex >= 0 ) }
	m_nextMoveIndex = m_pieceRange[m_nextPieceIndex+1];

	// (5) Moves which do not represent the piece flipped from its standard 
	//     orientation are selected	
	// (6) Moves with the least rotations from default orientation are selected
	while( m_nextMoveIndex < NUM_MOVES && !m_nextMoveList->isValid[m_nextMoveIndex] )
	{ m_nextMoveIndex++; ASSERT( m_nextMoveIndex < m_pieceRange[m_nextPieceIndex] ) }

	// Return the index of the selected move
	ASSERT( m_nextMoveList->isValid[m_nextMoveIndex] )
	return m_nextMoveList->moves + m_nextMoveIndex;
}
//
// --------------------------------------------------------
//  GetNext - Returns the next available move in the move
//  list. If no more moves exist, return value is NULL.
//  The returned move will always be the next best move 
//  according to the ordering specified in GetFirstMove.
// --------------------------------------------------------
const Move* MoveLists::getNextMove( )
{
	// (6) Moves with the least rotations from default orientation are selected
	// (5) Moves which do not represent the piece flipped from its standard 
	//     orientation are selected	
	while( (++m_nextMoveIndex) != m_pieceRange[m_nextPieceIndex] &&
		!m_nextMoveList->isValid[m_nextMoveIndex] ) ;
	if( m_nextMoveIndex == m_pieceRange[m_nextPieceIndex] ) {
		
		// (4) Pieces with higher indexes are selected	
		int validPieces = m_validPieces & m_nextMoveList->validPieces;
		do m_nextPieceIndex--; while( m_nextPieceIndex >= 0 && 
		!((1<<m_nextPieceIndex)&validPieces) ); if( m_nextPieceIndex < 0 ) {

			// (3) (2) (1)

			// (1) Liberties with the greatest
			//     x-coordinate are selected
			// (2) Liberties with the greatest
			//     x-coordinate are selected
			// (3) Ignore sleeping liberties
			do if( !(m_nextMoveList = m_nextMoveList->next) ) return NULL;
				while( !m_nextMoveList->isAwake );

			// (4) Pieces with higher indexes are selected 
			m_nextPieceIndex = PIECE_COUNT-1;

			// Get bitset of pieces available for selection
			int validPieces = m_validPieces & m_nextMoveList->validPieces;

			// Select the first valid piece found when searching
			while( !( (1 << m_nextPieceIndex) & validPieces ) )
			{ m_nextPieceIndex--; ASSERT( m_nextPieceIndex >= 0 ) }

		} // (4 )
	
		// Initialize move index back to initial move in piece
		m_nextMoveIndex = m_pieceRange[m_nextPieceIndex+1];

		// (5) Moves which do not represent the piece flipped from its standard 
		//     orientation are selected	
		// (6) Moves with the least rotations from default orientation are selected
		while( !m_nextMoveList->isValid[m_nextMoveIndex] )
		{ m_nextMoveIndex++; ASSERT( m_nextMoveIndex < m_pieceRange[m_nextPieceIndex] ) }

	} // (5) & (6)

	// Return the index of the selected move
	ASSERT( m_nextMoveList->isValid[m_nextMoveIndex] )
	return m_nextMoveList->moves + m_nextMoveIndex;
}
//
// --------------------------------------------------------
// MakeLiberty - Creates a new liberty and generates a 
// move list for the liberty if it is valid.
// --------------------------------------------------------
void MoveLists::makeLiberty( int bx, int by, int a, int player, 
							short grid[][14], int pieces[] )
{
	// Dead liberty flag
	bool deadLiberty = TRUE;

	// Get player bit for demasking
	short playerBit = 1 << (player+EX_GRID_NOT_SAFE);

	// Get a list block for the liberty data
	MoveList* nextLib = (MoveList*)m_memoryPool->getChunk( );
	
	// Copy the piece move information into the block
	memcpy( (void*)nextLib->moves, (void*)m_moves, sizeof(Move)*NUM_MOVES );

	// Set liberty's valid piece table
	nextLib->validPieces = pieces[player];

	// Cycle through available pieces
	for( int i = 0; i < PIECE_COUNT; i++ )
	if( pieces[player] & (1<<i) )
	{
		// Get handle to piece object
		Piece* piece = PieceSet::getPiece( i );

		// Get dimensions of piece
		int w = piece->getSizeX( ) - 1;
		int h = piece->getSizeY( ) - 1;

		// Dead piece identifier
		int deadPiece = TRUE;

		// Cycle through the pieces range
		for( int j = m_pieceRange[i+1]; 
				 j < m_pieceRange[i]; 
				 j++ )
		{
			// Get reference to move
			Move& move = nextLib->moves[j];
			//move = m_moves[j];

			// Compute rotation angle
			int rotation = (a - move.rotated + 6)%4;
			if( move.flipped ) move.rotated = (4 - rotation)%4;
			else move.rotated = rotation;

			// Piece offsets
			int sx, x, sy, y;

				 // Compute grid offsets based on the angle and dimensions
				 if( rotation == 0 ) { x = move.gridX; y = move.gridY; sx = w; sy = h; }
			else if( rotation == 1 ) { x = move.gridY; y = w-move.gridX; sx = h; sy = w; }
			else if( rotation == 2 ) { x = w-move.gridX; y = h-move.gridY; sx = w; sy = h; }
			else if( rotation == 3 ) { x = h-move.gridY; y = move.gridX; sx = h; sy = w; }

			// Check for invalid grid locations
			move.gridX = bx - x; move.gridY = by - y;
			if( move.gridX < -1 || move.gridY < -1  || 
				move.gridX > BOARD_SIZE - sx || 
				move.gridY > BOARD_SIZE - sy ) {
				nextLib->isValid[j] = FALSE;
				continue; }

			// Profile move validation time
			__int64 validationTimeID = Profiler::startProfile( );
			int isValid = isValidMove( move, grid, playerBit );
			Profiler::endProfile( tMoveValidation, validationTimeID );

			// Check if the move is valid
			if( !isValid )
				nextLib->isValid[j] = FALSE;
			else { 
				nextLib->isValid[j] = TRUE;
				deadLiberty = FALSE; 
				deadPiece = FALSE; }
		}

		// Check for dead piece identifier still flagged
		if( deadPiece ) nextLib->validPieces &= ~(1<<i);
	}

	// Check for dead liberty
	if( deadLiberty ) 
	{
		// Get general mask bit
		int playerMask = 1 << player;

		// Return the chunk to the memory pool
		m_memoryPool->freeChunk( nextLib );

		return; 
	}

	// Store the liberty coordinates
	nextLib->x = bx; nextLib->y = by;
	nextLib->angle = a; nextLib->isAwake = true;
	
	// Check for a leak opportunity
	updateLeakFlag( nextLib, grid );

	// Generate the path table
	initLibertyUpdateTable( nextLib, 1<<player<<EX_GRID_NOT_SAFE, grid );

	// Add the liberty to the grid
	grid[bx][by] |= 1 << (player+EX_LBTY_ANGLE(a)); 

	// Get handle of first liberty entry
	MoveList* iterator = m_moveList[player];

	// Check for empty list 
	if( iterator == NULL )
	{
		m_moveList[player] = nextLib;
		nextLib->prev = nextLib->next = NULL;
	}
	else
	{
		// Check if the liberty is a new head
		if( iterator->x < bx || (iterator->x == bx 
			&& iterator->y < by) ) 
		{
			// Insert the new list head
			m_moveList[player] = nextLib;
			iterator->prev = nextLib;
			nextLib->next = iterator;

			return; 
		}

		// Otherwise, insert the liberty into the list
		while( iterator->next && iterator->next->x > bx ) iterator = iterator->next;
		while( iterator->next && iterator->next->x == bx && iterator->next->y > by ) iterator = iterator->next;
		ASSERT( !iterator->next || iterator->next->x != bx || iterator->next->y != by )

		// Update the linked list connectors
		if( iterator->next ) iterator->next->prev = nextLib;
		nextLib->prev = iterator; nextLib->next = iterator->next;
		iterator->next = nextLib;

		// Runtime test for malformed list error
		MoveList* movelist = m_moveList[player];
		while( (movelist = movelist->next) )
		ASSERT( movelist->prev->x > movelist->x ||
			(movelist->prev->x == movelist->x && 
			 movelist->prev->y > movelist->y) )
	}
}
//
// --------------------------------------------------------
// InitLibertyUpdateTable - Initializes the paths table for 
// the specified liberty.
// --------------------------------------------------------
void MoveLists::initLibertyUpdateTable( MoveList* liberty, int playerBit, short grid[][14] )
{
	// Begin profiling path cost table generation
	__int64 makeTableTimeID = Profiler::startProfile( );

	// Reset update flag
	liberty->update = INT_MAX;

	// Zero path table memory 
	MoveList::PathEntry* pathEntry = liberty->updateTable;
	ZeroMemory( pathEntry, sizeof( liberty->updateTable ) );

	// Initialize root node
	pathEntry[ D*D+D ].pathlen = 0; 
	pathEntry[ D*D+D ].paths = 1;

	// Initialize expansion queue
	TableIndex expansionQueue[UTSZ];
	int queueRead = 0, queueWrite = 1;
	expansionQueue[0].x = 0;
	expansionQueue[0].y = 0;

	// Expand root path
	while( queueRead != queueWrite )
	{
		// Neighbor tile modifiers
		static const int NUM_NEIGHBORS = 4;
		static const int nx[4] = { -1, 1,  0, 0 };
		static const int ny[4] = {  0, 0, -1, 1 };

		// Queued tile data
		int tx = expansionQueue[queueRead].x;
		int ty = expansionQueue[queueRead].y;

		// Neighbor square Expansion
		for( int n = 0; n < NUM_NEIGHBORS; n++ )
		{
			// Compute neighbors
			int cx = tx + nx[n], cy = ty + ny[n];
			int i = INDEX_OF( cx, cy );
			int bx = liberty->x + cx;
			int by = liberty->y + cy;

			// Check tile access bounds
			if( bx >= 0 && bx < BOARD_SIZE &&
				by >= 0 && by < BOARD_SIZE &&
				abs(cx) + abs(cy) <= D ) 
			{
				// Check grid tile safety
				if( !(grid[bx][by]&playerBit) )
				{
					// Check for newly found path 
					int d = liberty->updateTable[INDEX_OF(tx,ty)].pathlen + 1;
					if( pathEntry[i].paths == 0 )
					{
						// Create table entry for path
						pathEntry[i].pathlen = d;
						pathEntry[i].paths = 1;

						// Check max pathlen
						if( d != D )
						{
							expansionQueue[queueWrite].x = cx;
							expansionQueue[queueWrite].y = cy;
							expansionQueue[queueWrite].i = i;
							queueWrite++;
						}
					}

					// Increment unique paths count
					else if( pathEntry[i].pathlen == d )
						pathEntry[i].paths++;
				}
			}
		}

		queueRead++;
	}

	// Stop profiling make table time
	Profiler::endProfile( tMakeTable, makeTableTimeID );
}
//
// --------------------------------------------------------
// MarkUnsafeTile - Marks unsafe tiles in all relevant 
// liberty path cost tables.
// --------------------------------------------------------
void MoveLists::markUnsafeTile( int bx, int by, int player )
{
	// Begin profiling update table time
	__int64 markUnsafeTimeID = Profiler::startProfile( );

	// Update liberty path cost table
	MoveList* iter = m_moveList[player]; 
	while( iter != NULL )
	{
		int x = bx - iter->x;
		int y = by - iter->y;
		int d = abs(x) + abs(y);

		if( d < D )
		{
			// Create handle to liberty path costs table
			MoveList::PathEntry* pathEntry = iter->updateTable;

			// Get associated path table index
			int index = INDEX_OF( x, y ); 
			ASSERT( index >= 0 && index < UTSZ );

			// Check for relevant tile covered
			if( pathEntry[index].paths != 0 ) 
			{
				// Check for new minimum update range
				int mdist = pathEntry[index].pathlen;
				iter->update = min( mdist, iter->update );
				
				// Check for dead liberty flag
				if( iter->update == 0 ) {
					iter = killLiberty( iter, player );
					continue; }

				// Neighbor tile modifiers
				static const int NUM_NEIGHBORS = 4;
				static const int nx[4] = { -1, 1,  0, 0 };
				static const int ny[4] = {  0, 0, -1, 1 };

				// Erase covered path entry
				pathEntry[index].pathlen = 0;
				pathEntry[index].paths = 0;
				int cdist = mdist + 1;

				// Initialize expansion queues
				TableIndex constructQueue[UTSZ];
				TableIndex expansionQueue[UTSZ];
				int queueRead = 0, queueWrite = 0;
				int cqueueRead = 0, cqueueWrite = 0;
				for( int n = 0; n < NUM_NEIGHBORS; n++ )
				{
					// Compute neighbor coordinates
					int cx = x+nx[n], cy = y+ny[n];
					int i = INDEX_OF( cx, cy );
					
					// Check neighbor dist valid
					if( abs(cx) + abs(cy) <= D &&
						pathEntry[i].paths &&
						pathEntry[i].pathlen == cdist )
					{
						// Check if the neighbor's path cost is affected
						if( pathEntry[i].paths > 1 ) pathEntry[i].paths--;
						else {
							expansionQueue[queueWrite].x = cx;
							expansionQueue[queueWrite].y = cy;
							expansionQueue[queueWrite].i = i;
							// queueWrite++;
						}
					}
				}

				// Propogate path deconstruction
				while( queueRead != queueWrite )
				{
					// Queued tile data
					int tx = expansionQueue[queueRead].x;
					int ty = expansionQueue[queueRead].y;
					int d = pathEntry[INDEX_OF( tx, ty )].pathlen+1;

					// Neighbor square Expansion
					for( int n = 0; n < NUM_NEIGHBORS; n++ )
					{
						// Compute neighbors
						int cx = tx + nx[n], cy = ty + ny[n];
						int i = INDEX_OF( cx, cy );

						// Check tile access bounds
						if( abs(cx) + abs(cy) <= D ) 
						{
							// Check tile distance and valid multiplicity
							if( pathEntry[i].paths != 0 )
							if( pathEntry[i].pathlen == d )
							{
								// Check for invalidated path length
								if( pathEntry[i].paths == 1 )
								{
									expansionQueue[queueWrite].x = cx;
									expansionQueue[queueWrite].y = cy;
									expansionQueue[queueWrite].i = i;
									queueWrite++;
								}
								else pathEntry[i].paths--;
							}
						}
					}

					queueRead++;
				}

				// Propogate path reconstruction
				while( cqueueRead != cqueueWrite )
				{
					// Queued tile data
					int tx = expansionQueue[queueRead].x;
					int ty = expansionQueue[queueRead].y;
					int d = pathEntry[INDEX_OF( tx, ty )].pathlen+1;

					// Neighbor square Expansion
					for( int n = 0; n < NUM_NEIGHBORS; n++ )
					{
						// Compute neighbors
						int cx = tx + nx[n], cy = ty + ny[n];
						int i = INDEX_OF( cx, cy );
						int d = pathEntry[i].pathlen;

						// Check tile access bounds
						if( abs(cx) + abs(cy) <= D ) 
						{
							// Check tile distance and valid multiplicity
							if( pathEntry[i].paths == 0 && /*grid*/TRUE )
							{
								// Check for invalidated path length
								if( pathEntry[i].paths == 1 )
								{
									constructQueue[queueWrite].x = cx;
									constructQueue[queueWrite].y = cy;
									constructQueue[queueWrite].i = i;
									cqueueWrite++;
								}
								else pathEntry[i].paths--;
							}
						}
					}

					cqueueRead++;
				}
			}
		}

		iter = iter->next;
	}

	// Stop profiling update table time
	Profiler::endProfile( tMarkUnsafe, markUnsafeTimeID );
}
//
// --------------------------------------------------------
// UpdateLiberties - Scans the liberties and updates any
// which have had tiles affected within their distance.
// --------------------------------------------------------
void MoveLists::updateLiberties( short grid[][14], int pieces[], int player )
{
	// Begin profiling update time
	__int64 updateLibsTimeID = Profiler::startProfile( );

	// Update recently affected active liberties
	MoveList* iterator = m_moveList[player];
	while( iterator ) 
	{
		// Update the liberty and proceed along the list
		if( iterator->isAwake && iterator->update < 5 )
		{
			// :TODO: Finish mark unsafe tile method for path costs table
			initLibertyUpdateTable( iterator, 1<<(player+EX_GRID_NOT_SAFE), grid );
			//iterator->displayInfo( ); system( "pause" ); // :DEBUG:
			
			// Update the liberty to the specified depth
			MoveList* next = iterator->next; iterator->update = INT_MAX; 
			updateLiberty( iterator, player, /*:TODO:*/0, grid, pieces );
			iterator = next;
		}
		else iterator = iterator->next;
	}

	// Stop profiling update table time
	Profiler::endProfile( tUpdateLibs, updateLibsTimeID );
}
//
// --------------------------------------------------------
//  DetectFightingLiberties - Scans through the liberty
//  lists and updates the isAwake flag.
// --------------------------------------------------------
void MoveLists::detectFightingLiberties( )
{
	// Begin profiling update time
	__int64 fightDetectionTimeID = Profiler::startProfile( );

	// Update fighting liberties 
	if( m_moveList[PLAYER_MIN] == NULL )
	{
		MoveList* iterator = m_moveList[PLAYER_MAX];
		while( iterator != NULL )
		{
			iterator->isAwake = FALSE;
			iterator = iterator->next;
		}
	} 
	else if( m_moveList[PLAYER_MAX] == NULL )
	{
		MoveList* iterator = m_moveList[PLAYER_MIN];
		while( iterator != NULL )
		{
			iterator->isAwake = FALSE;
			iterator = iterator->next;
		}
	} 
	else
	{
		MoveList* player = m_moveList[PLAYER_MIN];
		MoveList* others = m_moveList[PLAYER_MAX];

		for( int p = 0; p < NUM_PLAYERS; p++ )
		for( MoveList* iter = m_moveList[p]; iter; 
			iter = iter->next ) iter->isAwake = FALSE;

		do { 
			for( MoveList* iter = m_moveList[PLAYER_MAX]; iter; iter = iter->next )
			{
				int x = iter->x - player->x;
				int y = iter->y - player->y;
				if( abs(x) + abs(y) < D )
				{
					if( !player->isAwake )
					if( player->updateTable[INDEX_OF(x,y)].paths )
						player->isAwake = TRUE;

					if( !iter->isAwake )
					if( iter->updateTable[INDEX_OF(-x,-y)].paths )
						iter->isAwake = TRUE;
				}
			}
		} while( player = player->next );
	}

	// Stop profiling fighting detection time
	Profiler::endProfile( tFightDetection, fightDetectionTimeID );
}
// 
// --------------------------------------------------------
// UpdateLiberty - Updates the valid flag for any moves 
// invalidated by previous board operations. Only moves 
// which could extend a piece out by a distance greater 
// than or equal to dist are updated. If the liberty is 
// found to be dead, the return value is TRUE, otherwise 
// FALSE is returned.
// --------------------------------------------------------
int MoveLists::updateLiberty( MoveList* liberty, int player, 
		int dist, short grid[][14], int pieces[] )
{
	// Dead liberty flag
	bool deadLiberty = TRUE;

	// Get player bit for masking grid
	short playerBit = 1 << (player+EX_GRID_NOT_SAFE);

	// Update valid piece table
	liberty->validPieces &= pieces[player];

	// :TODO: Check move list generated
	//if( !liberty->generated ) generateMoves( liberty );

	// Cycle through available pieces
	for( int i = 0; i < PIECE_COUNT; i++ )
	if( m_pieceSizes[i] >= dist )
	if( pieces[player] & (1<<i) )
	{
		// Get handle to piece object
		Piece* piece = PieceSet::getPiece( i );

		// Dead piece identifier
		int deadPiece = TRUE;

		// Cycle through the pieces range
		for( int j = m_pieceRange[i+1]; 
			j < m_pieceRange[i]; j++ )
		if( liberty->isValid[j] )
		{
			// Profile move validation time
			__int64 validationTimeID = Profiler::startProfile( );
			int isValid = isValidMove( liberty->moves[j], grid, playerBit );
			Profiler::endProfile( tMoveValidation, validationTimeID );

			if( !isValid ) liberty->isValid[j] = FALSE;
			else { deadLiberty = FALSE; deadPiece = FALSE; }
		}

		// Check for dead piece identifier still flagged
		if( deadPiece ) liberty->validPieces &= ~(1<<i);
	}

	// Check for a dead liberty
	if( deadLiberty ) 
	{
		// Mask off any active liberty bits on the grid tile
		short playerMask = 1 << player;
		grid[liberty->x][liberty->y] &= 
			~( (playerMask<<EX_GRID_LBTY_UR)|(playerMask<<EX_GRID_LBTY_UL)|
			   (playerMask<<EX_GRID_LBTY_LL)|(playerMask<<EX_GRID_LBTY_LR) );
		
		// Remove the liberty from the lists
		killLiberty( liberty, player ); return TRUE;
	}

	// Check for a leak opportunity
	updateLeakFlag( liberty, grid );

	// Return no death
	return FALSE;
}
//
// --------------------------------------------------------
//  KillLiberty - Removes a liberty from the internal list.
// --------------------------------------------------------
MoveList* MoveLists::killLiberty( MoveList* liberty, int player )
{
	// Begin profiling liberty deletion
	__int64 killLibTimeID = Profiler::startProfile( );

	// Remove the liberty from the list structure
	if( liberty == m_moveList[player] ) m_moveList[player] = liberty->next;
	else liberty->prev->next = liberty->next;
	if( liberty->next ) liberty->next->prev = liberty->prev;

	// Free the liberty chunk
	MoveList* nextLiberty = liberty->next;
	m_memoryPool->freeChunk( liberty );
	
	// Stop profiling make time
	Profiler::endProfile( tKillLibs, killLibTimeID );

	// Return next handle
	return nextLiberty;
}
// 
// --------------------------------------------------------
//  UpdateLeakFlag - Checks if a given liberty represents a 
//  potential leak and updates the leak flag.
// --------------------------------------------------------
void MoveLists::updateLeakFlag( MoveList* liberty, short grid[][14] )
{
	// Generic cover mask
	static const short COVERED = (1<<EX_GRID_COVERED) | (2<<EX_GRID_COVERED);

	// Begin profiling liberty updates
	__int64 leakDetectionTimeID = Profiler::startProfile( );

	// Detect possible leaks
	switch( liberty->angle )
	{
		case PIECE_ROTATE_0:   
			if( (grid[liberty->x-1][liberty->y] & COVERED) &&
				(grid[liberty->x][liberty->y+1] & COVERED) )
				liberty->isLeakLbty = TRUE;
			else liberty->isLeakLbty = FALSE;
			break;	

		case PIECE_ROTATE_90: 
			if( (grid[liberty->x+1][liberty->y] & COVERED) &&
				(grid[liberty->x][liberty->y+1] & COVERED) )
				liberty->isLeakLbty = TRUE;
			else liberty->isLeakLbty = FALSE;
			break;

		case PIECE_ROTATE_180: 
			if( (grid[liberty->x+1][liberty->y] & COVERED) &&
				(grid[liberty->x][liberty->y-1] & COVERED) )
				liberty->isLeakLbty = TRUE;
			else liberty->isLeakLbty = FALSE;
			break;

		case PIECE_ROTATE_270: 
			if( (grid[liberty->x-1][liberty->y] & COVERED) &&
				(grid[liberty->x][liberty->y-1] & COVERED) )
				liberty->isLeakLbty = TRUE;
			else liberty->isLeakLbty = FALSE;
			break;
	}
	
	// Stop profiling make time
	Profiler::endProfile( tLeakDetection, leakDetectionTimeID );
}
//
// --------------------------------------------------------
//  DisplayLibertyInfo - Displays information associated 
//  with the specified liberty. 
// --------------------------------------------------------
void MoveList::displayInfo( )
{
	// Initialize a static pointer to a mutex object
	static HANDLE mutex = CreateMutex( NULL, FALSE, NULL );
    
	// Wait for ownership of the mutex object
	unsigned long waitResult = WaitForSingleObject( mutex, INFINITE );
 
	// Check for access
    if( waitResult == WAIT_ABANDONED ) return; 

	// Display path cost table
	std::cout << "\n Path Cost Table";
	for( int ty = -D; ty <= D; ty++ ) { std::cout << "\n\n";
	for( int tx = -D; tx <= D; tx++ ) { std::cout << " ";
		if( abs(tx) + abs(ty) <= D ) 
			if( updateTable[INDEX_OF(tx,ty)].paths == 0 )
				std::cout << "X";
			else std::cout << (int)updateTable[INDEX_OF(tx,ty)].pathlen; 
		else std::cout << " "; } }

	// Display path multiplicity table
	std::cout << "\n Path Multiplicity Table";
	for( int ty = -D; ty <= D; ty++ ) { std::cout << "\n\n";
	for( int tx = -D; tx <= D; tx++ ) { std::cout << " ";
		if( abs(tx) + abs(ty) <= D ) 
			std::cout << (int)updateTable[INDEX_OF(tx,ty)].paths; 
		else std::cout << " "; } }

	// Display liberty statistics
	std::cout << "\nPosition (x,y): ( " << x << " " << y << " )";
	std::cout << "\nDirty Distance: " << update;
	std::cout << "\nLiberty Angle: " << getAngle( );
	std::cout << "\nIs a Leak?: " << isLeak( );
	std::cout << "\nIs Fighting?: " << isFighting( );
	std::cout << "\nNum Moves: " << getNumMoves( );

	// Release mutex ownership
	ReleaseMutex( mutex );
}