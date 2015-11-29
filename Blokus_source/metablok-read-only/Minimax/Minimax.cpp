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

// Include headers
#include "Includes.h"

// Minimax Search Settings
#define FORCE_EVAL		  2  //< Forces the specified eval funct
#define MAX_THREADS       4   //< Maximum minimax thread count
#define MIN_DEPTH	      3   //< Minimum minimax search depth
#define MAX_DEPTH         3   //< Maximum minimax search depth
#define PROFILE		   TRUE   //< Imbeds profile code in build

// Opening book filename
#define BOOK_FNAME	NULL   //< Opening book filename, NULL for none

// Profiler segments
enum ProfilerFunctions {
	tTotal, tMinimax, tReformatBoard, tEnumerateMoves, tEvaluateBoards, 
	tCheckValidMoves, tSimulateMoves, tMoveValidation, tEnd };

// Static member declarations
std::vector<Minimax::Liberty> Minimax::m_pieceLiberties[21];
Minimax::Piece Minimax::m_piece[21];
int Minimax::m_startTile[4][2];
int Minimax::m_evalFunction;
int Minimax::m_boardSize;    
int Minimax::m_nPlayers;
Timer Minimax::m_matchTimer;
OpeningBook Minimax::m_book;

// Profiler data members
__int64 Minimax::m_timeCosts[10];
unsigned int Minimax::m_nodesSearched;
unsigned int Minimax::m_leavesSearched;

// --------------------------------------------------------
//	Store match settings data and load piece configuration
//	data from file. Also loads in optional settings data.
// --------------------------------------------------------
void Minimax::startup( int boardSize, int startTile[][2], int nPlayers ) 
{
	// Generate a new random seed
	srand( (unsigned int)time(NULL) );

	// Initiate the timer
	m_matchTimer.start( );

	// Load piece data
	loadPieceConfigs( ); getPieceLiberties( );

	// Store player/board data
	m_boardSize = boardSize; m_nPlayers = nPlayers;

	// Store starting liberty tiles
	for( int i = 0; i < nPlayers; i++ ) {
		m_startTile[i][0] = startTile[i][0];
		m_startTile[i][1] = startTile[i][1]; }

	// Load opening book
	if( BOOK_FNAME ) { try { m_book.openBook( BOOK_FNAME );
			std::cout << "Opening book " << BOOK_FNAME << " loaded\n";
		} catch( const char *s ) {
			std::cerr << "Error with opening book:\n	" << s << "\n\n"; } 
	} else std::cout << "No opening book loaded\n";

	// Print settings to standard io
	std::cout << "Max Thread Count: " << MAX_THREADS << "\n";
	std::cout << "Min Search Depth: " << MIN_DEPTH << "\n";
	std::cout << "Max Search Depth: " << MAX_DEPTH << "\n";

	// Select an evaluation function
	if( FORCE_EVAL != -1 ) m_evalFunction = FORCE_EVAL; else 
	{
		// Print available functions
		std::cout << "\nAvailable Heuristic Functions:\n";
		for( int i = 0; i < Heuristic::nEvaluationFunctions; i++ )
			std::cout << "	(" << i << ") " << Heuristic::evalFunctionName[i] << "\n";

		// Select a function
		int attempts = 0, success = FALSE;
		while( attempts <= 5 && !success ) 
		{
			// Display prompt
			std::cout << "Select a heuristic: ";

			// Get function number
			std::cin >> m_evalFunction;
			if( !std::cin.fail( ) && m_evalFunction >= 0 &&
				m_evalFunction < Heuristic::nEvaluationFunctions )
				success = TRUE;

			// Record attempt
			attempts = attempts+1;
        } 

		// Autoselect a function
		if( !success ) m_evalFunction = 0;
    }

	// Print ready message
	std::cout << "Ready to Move!!!\n";
} 
//
// --------------------------------------------------------
//	Converts the format of the board from a single byte
//	cover map to a 4 byte cover, adjacent, diagonal_1-4
//	map used for speeding up the board reasoning and move
//  enumeration algorithms. Also reformats piece arrays.
// --------------------------------------------------------
void Minimax::reformatBoard( char boardIn[][20], short boardOut[][14],
							 bool piecesIn[][21], int piecesOut[][3] )
{
	// Get the current time
	LARGE_INTEGER temp; __int64 startTime;
	if( PROFILE ) { QueryPerformanceCounter( &temp );
				  startTime = temp.QuadPart; }

	// Zero piece memory
	for( int p = 0; p < 2; p++ )
	for( int i = 0; i < 3; i++ )
		piecesOut[p][i] = 0;

	// Zero board memory
	for( int i = 0; i < m_boardSize; i++ )
	for( int j = 0; j < m_boardSize; j++ )
		boardOut[i][j] = 0;

	// Pack pieces array
	for( int p = 0; p < 2; p++ )
	for( int i = 0; i < 3; i++ )
	for( int j = 0; j < 8; j++ )
	if( 8*i+j >= 21 ) continue;
	else piecesOut[p][i] |= (piecesIn[p][8*i+j] << j);

	// Convert board to extended format
	for( int i = 0; i < m_boardSize; i++ )
	for( int j = 0; j < m_boardSize; j++ )
	{
		// Check if grid square is covered
		if( boardIn[i][j] != GRID_COVER_NONE )
		{
			// Mark tile as unsafe for all
			boardOut[i][j] = (short)boardIn[i][j]+1;
			boardOut[i][j] |= (0x3<<EX_GRID_NOT_SAFE);
		}

		// Check for liberties and safeties
		else  
		{ 
			// Check for starting liberty
			for( int k = 0; k < m_nPlayers; k++ )
			if( i == m_startTile[k][0] && j == m_startTile[k][1] ) {
				if( k == 0 ) boardOut[i][j] = (k+1)<<EX_GRID_LBTY_LR;
				if( k == 1 ) boardOut[i][j] = (k+1)<<EX_GRID_LBTY_UL;
				continue; }

			// Check for adjacent covered tile
			if( j<m_boardSize-1 && boardIn[i][j+1]!=GRID_COVER_NONE )
				boardOut[i][j] |= ((boardIn[i][j+1]+1)<<EX_GRID_NOT_SAFE);
			if( i<m_boardSize-1 && boardIn[i+1][j]!=GRID_COVER_NONE )
				boardOut[i][j] |= ((boardIn[i+1][j]+1)<<EX_GRID_NOT_SAFE);
			if( j>0 && boardIn[i][j-1]!=GRID_COVER_NONE )
				boardOut[i][j] |= ((boardIn[i][j-1]+1)<<EX_GRID_NOT_SAFE);
			if( i>0 && boardIn[i-1][j]!=GRID_COVER_NONE ) 
				boardOut[i][j] |= ((boardIn[i-1][j]+1)<<EX_GRID_NOT_SAFE);
			
			// Check if this tile is a liberty for any players
			if( (i>0 && j<m_boardSize-1 && boardIn[i-1][j+1]!=GRID_COVER_NONE) && 
				!((boardOut[i][j])&((boardIn[i-1][j+1]+1)<<EX_GRID_NOT_SAFE)) ) 
				boardOut[i][j] |= ((boardIn[i-1][j+1]+1) << EX_GRID_LBTY_UR);
			if( (i<m_boardSize-1 && j<m_boardSize-1 && boardIn[i+1][j+1]!=GRID_COVER_NONE) && 
				!((boardOut[i][j])&((boardIn[i+1][j+1]+1)<<EX_GRID_NOT_SAFE)) ) 
				boardOut[i][j] |= ((boardIn[i+1][j+1]+1) << EX_GRID_LBTY_UL);
			if( (i<m_boardSize-1 && j>0 && boardIn[i+1][j-1]!=GRID_COVER_NONE) && 
				!((boardOut[i][j])&((boardIn[i+1][j-1]+1)<<EX_GRID_NOT_SAFE)) ) 
				boardOut[i][j] |= ((boardIn[i+1][j-1]+1) << EX_GRID_LBTY_LL);
			if( (i>0 && j>0 && boardIn[i-1][j-1]!=GRID_COVER_NONE) && 
				!((boardOut[i][j])&((boardIn[i-1][j-1]+1)<<EX_GRID_NOT_SAFE)) ) 
				boardOut[i][j] |= ((boardIn[i-1][j-1]+1) << EX_GRID_LBTY_LR);
		}
	}

	// Get the current time
	if( PROFILE ) { QueryPerformanceCounter( &temp );
		m_timeCosts[tReformatBoard] += temp.QuadPart - startTime; } 
}
//
// --------------------------------------------------------
//	Returns a move selected by the minimax algorithm.
// --------------------------------------------------------
Move Minimax::makeMove( char grid[][20], bool pieces[][21], int score[], int player, std::vector<Move>& moveHistory )
{
	// Set the minimum search depth
	int maxSearchDepth = MIN_DEPTH;

	// First check if position is in opening book
	if(m_book.isInBook(moveHistory)) {
		try { return m_book.makeMove(moveHistory);
		} catch(const char *s) {
			std::cerr << "Error with opening book " << s << std::endl; } }

	// Iterative deepening loop
	while( TRUE )
	{
		// Clear profiler data
		if( PROFILE ) { for( int i = 0; i < tEnd; i++ ) m_timeCosts[i] = 0; 
				m_nodesSearched = 0; m_leavesSearched = 0; }

		// Get the current time
		LARGE_INTEGER temp; __int64 startTimeTotal;
		if( PROFILE ) { QueryPerformanceCounter( &temp );
					  startTimeTotal = temp.QuadPart; }

		// Update timer and store starting data
		m_matchTimer.update( ); float startTime = m_matchTimer.getRunningTime( );

		// Reformat game board for optimized move searches
		short newGrid[14][14]; int newPieces[2][3];
		reformatBoard( grid, newGrid, pieces, newPieces );

		// Uses minimax algorithm to select the best move
		Move move = getMinimaxMoveMultiThreaded( newGrid, newPieces, score, player, maxSearchDepth-1 );

		// Update timer for comparison with remaining match time
		m_matchTimer.update( ); float endTime = m_matchTimer.getRunningTime( );
		float searchTime = endTime - startTime; 

		// Acquire total run time costs
		if( PROFILE ) { QueryPerformanceCounter( &temp );
			m_timeCosts[tTotal] += temp.QuadPart - startTimeTotal; }

		// Display statistical data gathered by profiler
		if( PROFILE ) displayProfilerResults( searchTime, maxSearchDepth );

		// Increment max search depth
		maxSearchDepth = maxSearchDepth + 1;
		
		// Check for terminal condition
		if( searchTime > 3.0f || maxSearchDepth > MAX_DEPTH )
			return move;
	}
}
// 
// --------------------------------------------------------
//  Cycles through available moves and returns the one with
//  the best utility rating.
// --------------------------------------------------------
Move Minimax::getMinimaxMove( short grid[][14], 
	int pieces[][3], int score[], int player, int depth )
{
	// Get the current time
	LARGE_INTEGER temp; __int64 startTime;
	if( PROFILE ) { QueryPerformanceCounter( &temp );
				  startTime = temp.QuadPart; }

	// Get available moves list
	Move moves[1200];
	int movesFound;
	if (score[player]<30){
		movesFound = getMoveList_5pieces( moves, grid, pieces, player );
	}
	else {
		movesFound = getMoveList( moves, grid, pieces, player );
	}

	// Number of possible moves output
	std::cout << "\n\nNumber of possible moves:" << movesFound << "\n";

	// Recursively perform minimax on each move
	int move; float alpha = -FLT_MAX, beta = FLT_MAX; 
	for( int i = 0; i < movesFound; i++ )
	{
		// Simulate the selected move on the board for minimax evaluation
		short mGrid[14][14]; int mPieces[2][3]; int mScore[4]; int mPlayer;
		simulateMove( moves[i], grid, pieces, score, player, 
						mGrid, mPieces, mScore, &mPlayer );

		// Perform minimax on the new board state
		float newUtility = minimax( mGrid, mPieces, mScore, mPlayer, depth, alpha, beta );

		// Update alpha-beta parameters
		if( player == PLAYER_MAX ) { if( newUtility > alpha ) { alpha = newUtility; move = i; } }
		else if( newUtility < beta ) { beta = newUtility; move = i; }
	}

	// Get the current time
	if( PROFILE ) { QueryPerformanceCounter( &temp );
		m_timeCosts[tMinimax] += temp.QuadPart - startTime; } 

	// Return index
	std::cout << "Player " << player << " selects a move with utility " << ( (player == PLAYER_MAX) ? alpha : beta) << "\n";
	return moves[move];
}
// 
// --------------------------------------------------------
//  Cycles through available moves and returns the one with
//  the best utility value. Uses a number of additional
//  threads equal to MAX_THREADS to search potential game
//	states simultaneously.
// --------------------------------------------------------
Move Minimax::getMinimaxMoveMultiThreaded( short grid[][14], 
			int pieces[][3], int score[], int player, int depth )
{
	// Get the current time
	LARGE_INTEGER temp; __int64 startTime;
	if( PROFILE ) { QueryPerformanceCounter( &temp );
				  startTime = temp.QuadPart; }

	// Create thread state arrays
	MtGameState threadStates[MAX_THREADS];
	int nextMoveIndex = 0, move = 0;

	// Get available moves list
	Move moves[1200];
	int maxMoveIndex;
	if (score[player]<30){
		maxMoveIndex = getMoveList_5pieces( moves, grid, pieces, player );
	}
	else {
		maxMoveIndex = getMoveList( moves, grid, pieces, player );
	}

	//Number of possible moves output
	std::cout << "\n\nNumber of possible moves:" << maxMoveIndex << "\n";

	// Set thread count limit 
	int nThreads = MAX_THREADS;
	if( nThreads > maxMoveIndex )
		nThreads = maxMoveIndex;

	// Initialize thread data
	float alpha = -FLT_MAX, beta = FLT_MAX; 
	for( int i = 0; i < nThreads; i++ ) {
		threadStates[i].completed = TRUE;
		threadStates[i].depth = depth;
		threadStates[i].utility = 
			(player == PLAYER_MAX) ? alpha : beta; }

	// Get utility values for each move
	while( nextMoveIndex < maxMoveIndex+nThreads )
	{
		// Check for a completed thread
		for( int i = 0; i < nThreads; i++ )
		if( threadStates[i].completed )
		{
			// Mark completed thread
			threadStates[i].completed = FALSE;

			// Update alpha-beta parameters
			if( player == PLAYER_MAX ) { if( threadStates[i].utility > alpha ) 
				{ alpha = threadStates[i].utility; move = threadStates[i].moveIndex; } }
			else if( threadStates[i].utility < beta ) 
				{ beta = threadStates[i].utility; move = threadStates[i].moveIndex; }

			// Check for all threads initiated
			if( nextMoveIndex < maxMoveIndex )
			{
				// Store the new threads move index
				threadStates[i].moveIndex = nextMoveIndex;

				// Set new alpha-beta parameters
				threadStates[i].alpha = alpha; threadStates[i].beta = beta;

				// Simulate the selected move on the board for minimax 
				simulateMove( moves[nextMoveIndex], grid, pieces, score, player, 
					threadStates[i].grid, threadStates[i].pieces, 
					threadStates[i].score, &threadStates[i].player );

				// Begin the utility ranking thread
				if( _beginthread( &getMinimaxUtility, 0, threadStates+i ) == -1 )
				{ std::cout << "Failed to create thread"; system("pause"); exit(1); }
			}

			// Increment index
			nextMoveIndex++; 
		} 

		// Yield CPU
		Sleep( 5 );
	}

	// Get the current time
	if( PROFILE ) { QueryPerformanceCounter( &temp );
		m_timeCosts[tMinimax] += temp.QuadPart - startTime; } 

	// Return index
	std::cout << "Player " << player << " selects a move with utility " << beta << "\n";
	return moves[move];
}
//
// --------------------------------------------------------
//	Starting address of search threads. Launches a minimax
//  evaluation of the specified game state.
// --------------------------------------------------------
void Minimax::getMinimaxUtility( void* dataOut )
{
	// Recast output pointer
	MtGameState* state = (MtGameState*)dataOut;

	// Perform minimax search
	state->utility = minimax( state->grid, state->pieces, state->score, state->player,
		state->depth, state->alpha, state->beta );

	// Mark the thread done
	state->completed = true;
}
//
// --------------------------------------------------------
//	Uses the minimax algorithm with alpha-beta pruning to
//	compute the utility value of a board position.
// --------------------------------------------------------
float Minimax::minimax( short grid[][14], int pieces[][3], int score[], int player,
						 int depth, float alpha, float beta )
{	
	// Check current depth for search tree cut-off
	if( depth == 0 ) 
	{
		// Begin profiling function
		LARGE_INTEGER temp; __int64 startTime;
		if( PROFILE ) { QueryPerformanceCounter( &temp );
					  startTime = temp.QuadPart; }

		// Compute board utility
		float utility = Heuristic::evalFunction[m_evalFunction]
			( grid, pieces, score, player );

		// Increment function runtime costs
		if( PROFILE ) { QueryPerformanceCounter( &temp );
			m_timeCosts[tEvaluateBoards] += temp.QuadPart - startTime; } 

		// Increment leaf node count
		if( PROFILE ) m_leavesSearched++;
		
		return utility;
	}

	// Enumerate available moves
	Move moves[1200]; int nMoves = 
		getMoveList( moves, grid, pieces, player );

	// Get the current time
	LARGE_INTEGER temp; __int64 startTime;
	if( PROFILE ) { QueryPerformanceCounter( &temp );
				  startTime = temp.QuadPart; }

	// Terminal board state detection and evaluation
	// :TODO: Seperate minimax function for player who is packing pieces
	//			ie. piece packing search to avoid move enumeration calls
	if( nMoves == 0 ) 
	{
		if( score[player] < score[1-player] ) {
			if( PROFILE ) { QueryPerformanceCounter( &temp );
			m_timeCosts[tCheckValidMoves] += temp.QuadPart - startTime; } 
			return (player==PLAYER_MAX) ? (-FLT_MAX) : FLT_MAX; }
		else if( isMoveAvailable( grid, pieces, 1-player ) ) {
			if( PROFILE ) { QueryPerformanceCounter( &temp );
			m_timeCosts[tCheckValidMoves] += temp.QuadPart - startTime; } 
			return minimax( grid, pieces, score, 1-player, depth-1, alpha, beta ); }
		else if( score[player] == score[1-player] ) {
			if( PROFILE ) { QueryPerformanceCounter( &temp );
			m_timeCosts[tCheckValidMoves] += temp.QuadPart - startTime; } 
			return 0.0f; }
		else {
			if( PROFILE ) { QueryPerformanceCounter( &temp );
			m_timeCosts[tCheckValidMoves] += temp.QuadPart - startTime; } 
			return (player==PLAYER_MAX) ? FLT_MAX : -FLT_MAX;; }
	}

	// Get the current time
	if( PROFILE ) { QueryPerformanceCounter( &temp );
		m_timeCosts[tCheckValidMoves] += temp.QuadPart - startTime; } 

	// Recursively perform minimax on each move
	for( int i = 0; i < nMoves; i++ )
	{
		// Simulate the selected move on the board for minimax evaluation
		short newGrid[14][14]; int newPieces[2][3]; int newScore[4]; int newPlayer;
		simulateMove( moves[i], grid, pieces, score, player, newGrid, 
			newPieces, newScore, &newPlayer );

		// Perform minimax on the new board state
		float newUtility = minimax( newGrid, newPieces, newScore, 
			newPlayer, depth-1, alpha, beta );

		// Update alpha-beta bounds
		if( player == PLAYER_MAX ) {
			if( newUtility > alpha ) alpha = newUtility; }
		else if( newUtility < beta ) beta = newUtility;

		// Check for alpha-beta cut-off
		if( beta <= alpha ) break;
	}

	// Return the appropriate utility bound
	return (player==PLAYER_MAX) ? alpha : beta;
}
//
// --------------------------------------------------------
//	Enumerates all available moves by searching through all
//  possible matches between piece and board liberties and
//	checking whether the move is valid or not.
// --------------------------------------------------------
int Minimax::getMoveList( Move moves[], short grid[][14], int pieces[][3], int player )
{
	// Get the current time
	LARGE_INTEGER temp; __int64 startTime;
	if( PROFILE ) { QueryPerformanceCounter( &temp );
				  startTime = temp.QuadPart; }

	// Total moves count
	int movesFound = 0;

	// Get player bit for demasking
	int playerBit = 1 << (player+EX_GRID_NOT_SAFE);

	// Cycle through pieces
	for( int p = 20; p >= 0; p-- ) 
	if( pieces[player][p/8] & (1<<(p%8)) ) 
	{
		// Construct partial move object
		Move move; move.pieceNumber = p;
		int w = m_piece[p].sizeX - 1;
		int h = m_piece[p].sizeY - 1;

		// Look for board liberties
		for( int bx = 0; bx < m_boardSize; bx++ )
		for( int by = 0; by < m_boardSize; by++ )
		for( int i = 0; i < 4; i++ )
		if( grid[bx][by]&(1<<(player+2*i+4)) )
		{
			// Cycle through piece liberties
			for( int j = 0; j < m_pieceLiberties[p].size( ); j++ )
			{
				move.flipped = m_pieceLiberties[p][j].flipped; 
				int rotation = (i - m_pieceLiberties[p][j].a + 6)%4;
				if( move.flipped ) move.rotated = (4 - rotation)%4;
				else move.rotated = rotation;

				int sx, x, px = m_pieceLiberties[p][j].x;
				int sy, y, py = m_pieceLiberties[p][j].y;

					 if( m_pieceLiberties[p][j].a == 0 ) { px--; py++; }
				else if( m_pieceLiberties[p][j].a == 1 ) { px++; py++; }
				else if( m_pieceLiberties[p][j].a == 2 ) { px++; py--; }
				else if( m_pieceLiberties[p][j].a == 3 ) { px--; py--; }

					 if( rotation == 0 ) { x = px; y = py; sx = w; sy = h; }
				else if( rotation == 1 ) { x = py; y = w-px; sx = h; sy = w; }
				else if( rotation == 2 ) { x = w-px; y = h-py; sx = w; sy = h; }
				else if( rotation == 3 ) { x = h-py; y = px; sx = h; sy = w; }

				move.gridX = bx - x; move.gridY = by - y;
				if( move.gridX < -1 || move.gridY < -1  || 
					move.gridX > m_boardSize - sx || 
					move.gridY > m_boardSize - sy ) continue;

				// Get the current time
				LARGE_INTEGER temp; __int64 startTime;
				if( PROFILE ) { QueryPerformanceCounter( &temp );
							  startTime = temp.QuadPart; }

				// Check validity of move
				int isValid = isValidMove( move, grid, playerBit );

				// Get the current time
				if( PROFILE ) { QueryPerformanceCounter( &temp );
					m_timeCosts[tMoveValidation] += temp.QuadPart - startTime; } 

				// Add the move to the list of moves found
				if( isValid ) { moves[movesFound] = move; movesFound++; }
			}
		}
	}

	// Moves searched
	if( PROFILE ) m_nodesSearched += movesFound;

	// Get the current time
	if( PROFILE ) { QueryPerformanceCounter( &temp );
		m_timeCosts[tEnumerateMoves] += temp.QuadPart - startTime; }

	return movesFound;
}

//
// --------------------------------------------------------
//	Enumerates all available moves by searching through all
//  possible matches between piece and board liberties and
//	checking whether the move is valid or not.
// --------------------------------------------------------
int Minimax::getMoveList_5pieces( Move moves[], short grid[][14], int pieces[][3], int player )
{
	// Get the current time
	LARGE_INTEGER temp; __int64 startTime;
	if( PROFILE ) { QueryPerformanceCounter( &temp );
				  startTime = temp.QuadPart; }

	// Total moves count
	int movesFound = 0;

	// Get player bit for demasking
	int playerBit = 1 << (player+EX_GRID_NOT_SAFE);

	// Cycle through pieces
	for( int p = 20; p >= 9; p-- ) 
	if( pieces[player][p/8] & (1<<(p%8)) ) 
	{
		// Construct partial move object
		Move move; move.pieceNumber = p;
		int w = m_piece[p].sizeX - 1;
		int h = m_piece[p].sizeY - 1;

		// Look for board liberties
		for( int bx = 0; bx < m_boardSize; bx++ )
		for( int by = 0; by < m_boardSize; by++ )
		for( int i = 0; i < 4; i++ )
		if( grid[bx][by]&(1<<(player+2*i+4)) )
		{
			// Cycle through piece liberties
			for( int j = 0; j < m_pieceLiberties[p].size( ); j++ )
			{
				move.flipped = m_pieceLiberties[p][j].flipped; 
				int rotation = (i - m_pieceLiberties[p][j].a + 6)%4;
				if( move.flipped ) move.rotated = (4 - rotation)%4;
				else move.rotated = rotation;

				int sx, x, px = m_pieceLiberties[p][j].x;
				int sy, y, py = m_pieceLiberties[p][j].y;

					 if( m_pieceLiberties[p][j].a == 0 ) { px--; py++; }
				else if( m_pieceLiberties[p][j].a == 1 ) { px++; py++; }
				else if( m_pieceLiberties[p][j].a == 2 ) { px++; py--; }
				else if( m_pieceLiberties[p][j].a == 3 ) { px--; py--; }

					 if( rotation == 0 ) { x = px; y = py; sx = w; sy = h; }
				else if( rotation == 1 ) { x = py; y = w-px; sx = h; sy = w; }
				else if( rotation == 2 ) { x = w-px; y = h-py; sx = w; sy = h; }
				else if( rotation == 3 ) { x = h-py; y = px; sx = h; sy = w; }

				move.gridX = bx - x; move.gridY = by - y;
				if( move.gridX < -1 || move.gridY < -1  || 
					move.gridX > m_boardSize - sx || 
					move.gridY > m_boardSize - sy ) continue;

				// Get the current time
				LARGE_INTEGER temp; __int64 startTime;
				if( PROFILE ) { QueryPerformanceCounter( &temp );
							  startTime = temp.QuadPart; }

				// Check validity of move
				int isValid = isValidMove( move, grid, playerBit );

				// Get the current time
				if( PROFILE ) { QueryPerformanceCounter( &temp );
					m_timeCosts[tMoveValidation] += temp.QuadPart - startTime; } 

				// Add the move to the list of moves found
				if( isValid ) { moves[movesFound] = move; movesFound++; }
			}
		}
	}

	// Moves searched
	if( PROFILE ) m_nodesSearched += movesFound;

	// Get the current time
	if( PROFILE ) { QueryPerformanceCounter( &temp );
		m_timeCosts[tEnumerateMoves] += temp.QuadPart - startTime; }

	return movesFound;
}

//
// --------------------------------------------------------
//	Checks if the given move is valid on the preconditions
//	that the move will cover a either a liberty or the 
//  player's starting tile. Returns true if the move is
//  valid, false otherwise.
// --------------------------------------------------------
bool Minimax::isValidMove( Move &move, short grid[][14], int player )
{
	// Run pattern analysis between grid and piece
	int x = m_piece[move.pieceNumber].sizeX-1;
	int y = m_piece[move.pieceNumber].sizeY-1;
	if( move.flipped == PIECE_UNFLIPPED )
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j = 1, gy = move.gridY+1; j < y; j++,gy++ )
			for( int i = 1, gx = move.gridX+1; i < x; i++,gx++ )
				if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY+1; i > 0; i--,gy++ )
			for( int j =   1, gx = move.gridX+1; j < y; j++,gx++ )
				if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY+1; j > 0; j--,gy++ )
			for( int i = x-1, gx = move.gridX+1; i > 0; i--,gx++ )
				if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i =   1, gy = move.gridY+1; i < x; i++,gy++ )
			for( int j = y-1, gx = move.gridX+1; j > 0; j--,gx++ )
				if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }
	} else 
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j =   1, gy = move.gridY+1; j < y; j++,gy++ )
			for( int i = x-1, gx = move.gridX+1; i > 0; i--,gx++ )
				if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY+1; i > 0; i--,gy++ )
			for( int j = y-1, gx = move.gridX+1; j > 0; j--,gx++ )
				if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY+1; j > 0; j--,gy++ )
			for( int i =   1, gx = move.gridX+1; i < x; i++,gx++ )
				if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i = 1, gy = move.gridY+1; i < x; i++,gy++ )
			for( int j = 1, gx = move.gridX+1; j < y; j++,gx++ )
				if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_NOT_COVERED
					&& ( player&(grid[gx][gy]) ) ) return false; }
	}

	// Return valid
	return true;
}
//
// --------------------------------------------------------
//	Simulates the given move on the input board and stores
//	the resulting game states in the output.
// --------------------------------------------------------
void Minimax::simulateMove( Move &move, short grid[][14], int pieces[][3], int score[], int player,
	short gridOut[][14], int piecesOut[][3], int scoreOut[], int* playerOut )
{
	// Get the current time
	LARGE_INTEGER temp; __int64 startTime;
	if( PROFILE ) { QueryPerformanceCounter( &temp );
				  startTime = temp.QuadPart; }

	// Copy board data to output
	for( int i = 0; i < m_boardSize; i++ )
	for( int j = 0; j < m_boardSize; j++ )
		gridOut[i][j] = grid[i][j];

	// Copy piece data to output
	for( int i = 0; i < 2; i++ )
	for( int j = 0; j < 3; j++ )
		piecesOut[i][j] = pieces[i][j];

	// Update piece registry
	piecesOut[player][move.pieceNumber/8] &= ~(1<<(move.pieceNumber%8));

	// Copy piece data to output array
	for( int i = 0; i < 4; i++ ) scoreOut[i] = score[i];

	// Update player score variable
		 if( move.pieceNumber > 8 ) scoreOut[player] += 5;
	else if( move.pieceNumber > 3 ) scoreOut[player] += 4;
	else if( move.pieceNumber > 1 ) scoreOut[player] += 3;
	else if( move.pieceNumber > 0 ) scoreOut[player] += 2;
	else scoreOut[player] += 1;

	// Update game grid
	int playerBit = (1 << player);
	int x = m_piece[move.pieceNumber].sizeX;
	int y = m_piece[move.pieceNumber].sizeY;
	if( move.flipped == PIECE_UNFLIPPED )
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j = 0, gy = move.gridY; j < y; j++,gy++ )
			for( int i = 0, gx = move.gridX; i < x; i++,gx++ ) 
				applyPiecePattern( move, gridOut, playerBit, i, j, gx, gy ); }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j =   0, gx = move.gridX; j <  y; j++,gx++ )
				applyPiecePattern( move, gridOut, playerBit, i, j, gx, gy ); }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				applyPiecePattern( move, gridOut, playerBit, i, j, gx, gy ); }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i =   0, gy = move.gridY; i <  x; i++,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				applyPiecePattern( move, gridOut, playerBit, i, j, gx, gy ); }
	} else 
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j =   0, gy = move.gridY; j <  y; j++,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				applyPiecePattern( move, gridOut, playerBit, i, j, gx, gy ); }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				applyPiecePattern( move, gridOut, playerBit, i, j, gx, gy ); }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i =   0, gx = move.gridX; i <  x; i++,gx++ )
				applyPiecePattern( move, gridOut, playerBit, i, j, gx, gy ); }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i = 0, gy = move.gridY; i < x; i++,gy++ )
			for( int j = 0, gx = move.gridX; j < y; j++,gx++ )
				applyPiecePattern( move, gridOut, playerBit, i, j, gx, gy ); }
	}

	// Switch player to move
	*playerOut = 1 - player;

	// Get the current time
	if( PROFILE ) { QueryPerformanceCounter( &temp );
		m_timeCosts[tSimulateMoves] += temp.QuadPart - startTime; } 
}
//
// --------------------------------------------------------
//  Applies a piece pattern to the grid at the specified
//  piece and grid coordinates.
// --------------------------------------------------------
void Minimax::applyPiecePattern( Move move, short gridOut[][14], int playerBit, int i, int j, int gx, int gy )
{
	// Ignore off grid tiles
	if( gx < 0 || gx >= m_boardSize || gy < 0 || gy >= m_boardSize ) return; 

	// Convert newly covered tiles to unsafe for all players
	if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_NOT_COVERED )
		gridOut[gx][gy] = (playerBit | (0x3<<EX_GRID_NOT_SAFE));

	// Mark tiles adjacent to covered tiles as unsafe for player
	else if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_NOT_PLAYERS ) {
			gridOut[gx][gy] &= ~( (playerBit<<EX_GRID_LBTY_UR)&(playerBit<<EX_GRID_LBTY_UL)&
								  (playerBit<<EX_GRID_LBTY_LL)&(playerBit<<EX_GRID_LBTY_LR) );
			gridOut[gx][gy] |= ( playerBit<<EX_GRID_NOT_SAFE ); }
	
	// Continue on the more commonly found ignore tiles
	else if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_DONT_CARE ) return;

	// Mark newly found liberties if the location is safe
	else if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_LBTY_UR ) {
		if( !(gridOut[gx][gy] & (playerBit<<EX_GRID_NOT_SAFE)) )
			gridOut[gx][gy] |= (playerBit<<EX_GRID_LBTY_LL); }
	else if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_LBTY_UL ) {
		if( !(gridOut[gx][gy] & (playerBit<<EX_GRID_NOT_SAFE)) )
			gridOut[gx][gy] |= (playerBit<<EX_GRID_LBTY_LR); }
	else if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_LBTY_LL ) {
		if( !(gridOut[gx][gy] & (playerBit<<EX_GRID_NOT_SAFE)) )
			gridOut[gx][gy] |= (playerBit<<EX_GRID_LBTY_UR); }
	else if( m_piece[move.pieceNumber].layout[i][j] == EX_MATCH_LBTY_LR ) {
		if( !(gridOut[gx][gy] & (playerBit<<EX_GRID_NOT_SAFE)) )
			gridOut[gx][gy] |= (playerBit<<EX_GRID_LBTY_UL); }
}
//
// --------------------------------------------------------
//	Searches for any available moves and returns true if
//	a move is available. Returns false otherwise.
// --------------------------------------------------------
bool Minimax::isMoveAvailable( short grid[][14], int pieces[][3], int player )
{
	// Get player bit for demasking
	int playerBit = 1 << (player+EX_GRID_NOT_SAFE);

	// Look for board liberties
	for( int p = 20; p >= 0; p-- ) 
	if( pieces[player][p/8] & (1<<(p%8)) )
	for( int bx = 0; bx < m_boardSize; bx++ )
	for( int by = 0; by < m_boardSize; by++ )
	for( int i = 0; i < 4; i++ )
	if( grid[bx][by]&(1<<(player+2*i+4)) )
	{
		Move move; move.pieceNumber = p;
		int w = m_piece[p].sizeX - 1;
		int h = m_piece[p].sizeY - 1;

		// Cycle through piece liberties
		for( int j = 0; j < m_pieceLiberties[p].size( ); j++ )
		{
			move.flipped = m_pieceLiberties[p][j].flipped; 
			int rotation = (i - m_pieceLiberties[p][j].a + 6)%4;
			if( move.flipped ) move.rotated = (4 - rotation)%4;
			else move.rotated = rotation;

			int sx, x, px = m_pieceLiberties[p][j].x;
			int sy, y, py = m_pieceLiberties[p][j].y;

				 if( m_pieceLiberties[p][j].a == 0 ) { px--; py++; }
			else if( m_pieceLiberties[p][j].a == 1 ) { px++; py++; }
			else if( m_pieceLiberties[p][j].a == 2 ) { px++; py--; }
			else if( m_pieceLiberties[p][j].a == 3 ) { px--; py--; }

				 if( rotation == 0 ) { x = px; y = py; sx = w; sy = h; }
			else if( rotation == 1 ) { x = py; y = w-px; sx = h; sy = w; }
			else if( rotation == 2 ) { x = w-px; y = h-py; sx = w; sy = h; }
			else if( rotation == 3 ) { x = h-py; y = px; sx = h; sy = w; }

			move.gridX = bx - x; move.gridY = by - y;
			if( move.gridX < -1 || move.gridY < -1  || 
				move.gridX > m_boardSize - sx || 
				move.gridY > m_boardSize - sy ) continue;

			// Get the current time
			LARGE_INTEGER temp; __int64 startTime;
			if( PROFILE ) { QueryPerformanceCounter( &temp );
						  startTime = temp.QuadPart; }

			int isValid = isValidMove( move, grid, playerBit );

			// Get the current time
			if( PROFILE ) { QueryPerformanceCounter( &temp );
				m_timeCosts[tMoveValidation] += temp.QuadPart - startTime; } 

			if( isValid ) return true; 
		}
	}

	return false;
}
//
// --------------------------------------------------------
//	Initializes the piece information structures.
// --------------------------------------------------------
void Minimax::loadPieceConfigs( )
{
	// Open piece structure file for reading
	std::wstring filename = std::wstring( L"Pieces.txt" );
	std::fstream file( filename.c_str( ), std::ios::in );

	// Check for file load failure
	if( !file.is_open( ) ) {
		std::cout << "File not found: \"Pieces.txt\"";
		system("pause"); exit( 1 ); }

	// Parse file for piece layouts
	for( int i = 0; i < 21; i++ ) 
	{
		// Read in dimensions
		file >> m_piece[i].sizeX;
		file >> m_piece[i].sizeY;
		file >> m_piece[i].rot;
		file >> m_piece[i].flip;

		// Read in piece layout
		for( int k = 0; k < m_piece[i].sizeY; k++ )
		for( int j = 0; j < m_piece[i].sizeX; j++ )
		{
			// Get piece format
			file >> m_piece[i].layout[j][k];

			// Convert to extended format
			switch( m_piece[i].layout[j][k] ) {
				case MATCH_NOT_PLAYERS: m_piece[i].layout[j][k] = EX_MATCH_NOT_PLAYERS; break;
				case MATCH_NOT_COVERED: m_piece[i].layout[j][k] = EX_MATCH_NOT_COVERED; break; 
				case MATCH_DONT_CARE: m_piece[i].layout[j][k] = EX_MATCH_DONT_CARE; break; 
				case MATCH_LIBERTY: m_piece[i].layout[j][k] = EX_MATCH_LIBERTY; break; }
		}
	}
}
//
// --------------------------------------------------------
//	Stores piece liberties in member variables for use in
//	move enumeration. Also converts piece pattern liberties
//  to extended format to aide in move simulation.
// --------------------------------------------------------
void Minimax::getPieceLiberties( )
{
	Liberty liberty;

	// Cycle through available pieces for liberties
	for( int i = 0; i < 21; i++ ) 
	{
		// Cycle through piece pattern and mark liberties
		for( int x = 0; x < m_piece[i].sizeX; x++ )
		for( int y = 0; y < m_piece[i].sizeY; y++ )
		if( m_piece[i].layout[x][y] == EX_MATCH_LIBERTY )
		{
			liberty.x = x; liberty.y = y; liberty.flipped = false;

			// Unflipped liberties
			if( x<m_piece[i].sizeX-1 && y<m_piece[i].sizeY-1 && m_piece[i].layout[x+1][y+1] == EX_MATCH_NOT_COVERED ) {
				if( m_piece[i].rot > 1 ) { liberty.a = 1; m_pieceLiberties[i].push_back( liberty ); }
				m_piece[i].layout[x][y] = EX_MATCH_LBTY_UL; }

			if( x>0 && y<m_piece[i].sizeY-1 && m_piece[i].layout[x-1][y+1] == EX_MATCH_NOT_COVERED ) 
				{ liberty.a = 0; m_pieceLiberties[i].push_back( liberty ); m_piece[i].layout[x][y] = EX_MATCH_LBTY_UR; }

			if( x>0 && y>0 && m_piece[i].layout[x-1][y-1] == EX_MATCH_NOT_COVERED ) {
				if( m_piece[i].rot > 3 ) { liberty.a = 3; m_pieceLiberties[i].push_back( liberty ); }
				m_piece[i].layout[x][y] = EX_MATCH_LBTY_LR; }

			if( x<m_piece[i].sizeX-1 && y>0 && m_piece[i].layout[x+1][y-1] == EX_MATCH_NOT_COVERED ) {
				if( m_piece[i].rot > 2 ) { liberty.a = 2; m_pieceLiberties[i].push_back( liberty ); }
				m_piece[i].layout[x][y] = EX_MATCH_LBTY_LL; }

			// Flipped liberties
			if( m_piece[i].flip )
			{
				liberty.flipped = true; liberty.x = m_piece[i].sizeX - 1 - liberty.x;
				
				if( x<m_piece[i].sizeX-1 && y<m_piece[i].sizeY-1 && m_piece[i].layout[x+1][y+1] == EX_MATCH_NOT_COVERED ) {
					if( m_piece[i].rot > 1 ) { liberty.a = 0; m_pieceLiberties[i].push_back( liberty ); } 
					m_piece[i].layout[x][y] = EX_MATCH_LBTY_UR; }

				if( x>0 && y<m_piece[i].sizeY-1 && m_piece[i].layout[x-1][y+1] == EX_MATCH_NOT_COVERED ) 
					{ liberty.a = 1; m_pieceLiberties[i].push_back( liberty ); m_piece[i].layout[x][y] = EX_MATCH_LBTY_UL; }

				if( x>0 && y>0 && m_piece[i].layout[x-1][y-1] == EX_MATCH_NOT_COVERED ) {
					if( m_piece[i].rot > 3 ) { liberty.a = 2; m_pieceLiberties[i].push_back( liberty ); }
					m_piece[i].layout[x][y] = EX_MATCH_LBTY_LL; }

				if( x<m_piece[i].sizeX-1 && y>0 && m_piece[i].layout[x+1][y-1] == EX_MATCH_NOT_COVERED ) {
					if( m_piece[i].rot > 2 ) { liberty.a = 3; m_pieceLiberties[i].push_back( liberty ); }
					m_piece[i].layout[x][y] = EX_MATCH_LBTY_LR; }
			}
		}
	}
}
//
// --------------------------------------------------------
//	Displays the information collected by the profiler in
//	a human readable fashion to standard output.
// --------------------------------------------------------
void Minimax::displayProfilerResults( float searchTime, int maxSearchDepth )
{
	std::cout << "\n-- Move Selection Statistics --\n"; 
	std::cout << searchTime << "s at Ply " << maxSearchDepth << "\n";
	std::cout << "Searched Nodes: " << m_nodesSearched << "\n";
	std::cout << "Searched Leafs: " << m_leavesSearched << "\n";
	std::cout << "Total Time " << (int)(100.0*(double)m_timeCosts[tTotal] 
		/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
	std::cout << "  - Reformat Board: " << (int)(100.0*(double)m_timeCosts[tReformatBoard] 
		/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
	std::cout << "  - Minimax: " << (int)(100.0*(double)m_timeCosts[tMinimax] 
		/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
	std::cout << "      Move Enumeration: " << (int)(100.0*(double)m_timeCosts[tEnumerateMoves] 
		/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
	std::cout << "        - Move Validation(shared): " << (int)(100.0*(double)m_timeCosts[tMoveValidation] 
		/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
	std::cout << "      Board Evaluation: " << (int)(100.0*(double)m_timeCosts[tEvaluateBoards] 
		/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
	std::cout << "      Move Simulation: " << (int)(100.0*(double)m_timeCosts[tSimulateMoves] 
		/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
	std::cout << "      Terminal Checks: " << (int)(100.0*(double)m_timeCosts[tCheckValidMoves] 
		/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
	std::cout << "        - Move Validation(shared): " << (int)(100.0*(double)m_timeCosts[tMoveValidation] 
		/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
}
//
// --------------------------------------------------------
//	Outputs the specified game state to the console. Very
//  useful for debugging purposes. :TODO: Make thread safe
// --------------------------------------------------------
void Minimax::displayState( short grid[][14], int pieces[][3], int score[], int player )
{
	// Temp buffer for output
	std::stringstream buffer;

	// Debug board display
	std::cout << "\n\n*** Board Layout ***";
	for( int i = 0; i < m_boardSize; i++ ) { buffer << "\n";
	for( int j = 0; j < m_boardSize; j++ ) {
			 if( ((grid[j][i]>>(PLAYER_BLUE+EX_GRID_COVERED))&1) ) buffer << "B ";
		else if( ((grid[j][i]>>(PLAYER_RED +EX_GRID_COVERED))&1) ) buffer << "R ";
		else buffer << "- ";
	} } 

	// Debug piece display
	buffer << "\n\n*** Pieces ***";
	for( int i = 0; i <  2; i++ ) { buffer << "\n";
	for( int j = 0; j < 21; j++ ) {
		buffer << ((pieces[i][j/8]>>(j%8))&1);
	} } 

	// Get move data
	Move moves[1200]; int movesFound = 
		getMoveList( moves, grid, pieces, player );
	buffer << "\nMoves: " << movesFound;
	buffer << "\nScore Blue: " << score[PLAYER_BLUE];
	buffer << "\nScore Red: " << score[PLAYER_RED];
	buffer << "\nPlayer: " << player; 
	buffer << "\nUtility: " << 
		Heuristic::evalFunction[m_evalFunction]
			( grid, pieces, score, player );

	// Write buffer to stdout
	std::cout << buffer.str( ).c_str( );
}
