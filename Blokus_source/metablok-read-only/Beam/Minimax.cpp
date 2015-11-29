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

// Include headers
#include "Includes.h"

// Minimax Search Settings
#define FORCE_EVAL_STD	  2   //< Forces the specified eval funct
#define FORCE_EVAL_END	  2   //< Forces the specified eval funct
#define MAX_THREADS       4   //< Maximum minimax thread count
#define MIN_DEPTH	      2   //< Minimum minimax search depth
#define MAX_DEPTH         2   //< Maximum minimax search depth

// Opening book filename
#define BOOK_FNAME	NULL   //< Opening book filename, NULL for none

// Evaluation functions
#define EVAL_STD  0		   //< Index of eval function used by default in board evaluation
#define EVAL_END  1		   //< Index of Eval function used when no liberties are awake

// Static member declarations
int Minimax::m_startTile[4][2];
int Minimax::m_evalFunction[2];
Timer Minimax::m_matchTimer;
OpeningBook Minimax::m_book;

// --------------------------------------------------------
//	Startup - Store match settings data and load piece 
//  configuration data from file into the piece set. Also 
//  loads in optional settings data through the console.
// --------------------------------------------------------
void Minimax::startup( int boardSize, int startTile[][2], int nPlayers ) 
{
	// Initiate the timer
	m_matchTimer.start( );

	// Generate a new random seed
	srand( (unsigned int)time(NULL) );

	// Load piece configurations
	PieceSet::initPieceConfigurations( );

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
	for( int j = 0; j < 2; j++ )
	if( j == EVAL_STD && FORCE_EVAL_STD != -1 ) m_evalFunction[EVAL_STD] = FORCE_EVAL_STD; else 
	if( j == EVAL_END && FORCE_EVAL_END != -1 ) m_evalFunction[EVAL_END] = FORCE_EVAL_END; else
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
			std::cout << "\nSelect a" << 
				(j ? "n endgame" : " standard")
				<< " heuristic: ";

			// Get function number
			std::cin >> m_evalFunction[j];
			if( !std::cin.fail( ) && m_evalFunction[j] >= 0 &&
				m_evalFunction[j] < Heuristic::nEvaluationFunctions )
				success = TRUE;

			// Record attempt
			attempts = attempts+1;
        } 

		// Autoselect a function
		if( !success ) m_evalFunction[j] = 0;
    }

	// Print ready message
	std::cout << "Ready to Move!!!\n";
} 
//
// --------------------------------------------------------
//	MakeMove - Returns a move based on the current board
//  configuration. If a response appears in the loaded
//  opening book, then it is used. Otherwise the move is
//  selected using a multithreaded minimax search.
// --------------------------------------------------------
Move Minimax::makeMove( char grid[][20], bool pieces[][21], int score[], 
					   int player, int ply, Move moves[42] )
{
	// First check if position is in opening book
	std::vector<Move> moveHistory( 42 ); 
	for( int i = 0; i < ply; i++ ) 
		moveHistory.push_back( moves[i] );
	if( m_book.isInBook( moveHistory ) ) {
		try { return m_book.makeMove( moveHistory );
		} catch( const char *s ) { 
			std::cerr << "Error with opening book\n" << s << "\n"; } }

	// Set the minimum search depth
	int maxSearchDepth = MIN_DEPTH;

	// Iterative deepening loop
	while( TRUE )
	{
		// Clear profiler data
		Profiler::clear( );

		// Get the current time
		__int64 totalTimeID = Profiler::startProfile( );

		// Update timer and store starting data
		m_matchTimer.update( ); float startTime = m_matchTimer.getRunningTime( );

		// Reformat game board for optimized move searches
		short newGrid[14][14]; int newPieces[2];
		MoveSimulator::reformatBoard( grid, newGrid, pieces, newPieces, m_startTile );

		// Uses minimax algorithm to select the best move
		Move move = getMinimaxMove( newGrid, newPieces, score, player, maxSearchDepth, ply );

		// Update timer for comparison with remaining match time
		m_matchTimer.update( ); float endTime = m_matchTimer.getRunningTime( );
		float searchTime = endTime - startTime; 

		// Acquire total run time profile
		Profiler::endProfile( tTotal, totalTimeID );

		// Display statistical data gathered by profiler
		Profiler::displayResults( searchTime, maxSearchDepth );

		// Increment max search depth
		maxSearchDepth = maxSearchDepth + 1;
		
		// Check for terminal condition in search settings
		if( searchTime > 3.0f || maxSearchDepth > MAX_DEPTH )
			return move;
	}
}
// 
// --------------------------------------------------------
//  GetMinimaxMove - Cycles through available moves and 
//  returns the one with the best utility value. Uses a 
//  number of additional threads equal to MAX_THREADS to 
//  search potential game states simultaneously.
// --------------------------------------------------------
Move Minimax::getMinimaxMove( short grid[][14], int pieces[], 
		int score[], int player, int depth, int ply )
{
	// Get the current time
	__int64 minimaxTimeID = Profiler::startProfile( );

	// Create thread state arrays
	MtGameState threadStates[MAX_THREADS];
	HANDLE threadHandles[MAX_THREADS];

	// Generate base move lists for minimax searching
	MoveLists moveLists; moveLists.allocateMemoryPool( 50 );
	moveLists.generateMoves( grid, pieces );

	// :DEBUG: Display Influence Map
	//InfluenceMap imap; 
	//imap.generate( &moveLists, grid, pieces, score, player );
	//imap.displayMap( );

	// Initialize thread data
	float alpha = -FLT_MAX, beta = FLT_MAX; 
	for( int i = 0; i < MAX_THREADS; i++ ) {
		threadStates[i].moveLists.allocateMemoryPool( 250 );
		threadStates[i].completed = TRUE;
		threadStates[i].depth = depth-1;
		threadStates[i].ply = ply+1;
		threadHandles[i] = NULL;
		threadStates[i].utility = 
			(player == PLAYER_MAX) ? alpha : beta; }

	// Select subset of pieces to search
	int validPieces = pieces[player];
	if( ply < 0 ) validPieces &= 0x1FF000; // Opening piece selection

	// Get the first available move from an awake liberty
	const Move* move = moveLists.getFirstMove( player, validPieces );

	// If no liberties are awake, wake them all up for begin/end game
	if( move == NULL ) moveLists.clearLibertyModeSettings( );
	move = moveLists.getFirstMove( player, validPieces );

	// Get utility values for each move
	const Move* bestMove = move; int completedThreads = 0; 
	while( completedThreads < MAX_THREADS )
	{
		// Check for a completed thread
		for( int i = 0; i < MAX_THREADS; i++ )
		if( threadStates[i].completed )
		{
			// Mark completed thread
			threadStates[i].completed = FALSE;

			// Close the thread handle
			if( threadHandles[i] ) CloseHandle( threadHandles[i] );

			// Deallocate memory chunks
			threadStates[i].moveLists.deallocateMemoryChunks( );

			// Update alpha-beta parameters while adhering to move ordering
			if( player == PLAYER_MAX ) { if( threadStates[i].utility > alpha ) 
				{ alpha = threadStates[i].utility; bestMove = threadStates[i].moveIndex; } }
			else if( threadStates[i].utility < beta ) 
				{ beta = threadStates[i].utility; bestMove = threadStates[i].moveIndex; }

			// Check if all rankings have been
			// initiated. If so mark completed
			if( move )
			{
				// Store the new threads move index
				threadStates[i].moveIndex = move;

				// Set new alpha-beta parameters
				threadStates[i].alpha = alpha; threadStates[i].beta = beta;
		
				// Begin profiling move simulation
				__int64 simulationTimeID = Profiler::startProfile( );

				// Simulate the selected move on the board for minimax 
				MoveSimulator::simulateMove( move, grid, pieces, score, player, 
					threadStates[i].grid, threadStates[i].pieces, 
					threadStates[i].score, &threadStates[i].player,
					&moveLists, &threadStates[i].moveLists );

				// Increment function runtime costs
				Profiler::endProfile( tSimulateMoves, simulationTimeID );

				// Begin the utility ranking thread
				threadHandles[i] = (HANDLE) _beginthreadex( NULL, 0, 
					&getMinimaxUtility, threadStates+i, 0, NULL );
				
				// Get next available move
				__int64 moveEnumerationTimeID = Profiler::startProfile( );
				move = moveLists.getNextMove( );
				Profiler::endProfile( tMoveEnumeration, moveEnumerationTimeID );
			}
			else completedThreads++;
		} 

		// Wait for threads
		WaitForMultipleObjects(
			MAX_THREADS,
			threadHandles, 
			FALSE, INFINITE );
	} 

	// Get a copy of the move
	Move returnedMove = *bestMove; 

	// Deallocate memory pools
	moveLists.deallocateMemoryPool( );
	for( int i = 0; i < MAX_THREADS; i++ )
		threadStates[i].moveLists.deallocateMemoryPool( );

	// Get the current time
	Profiler::endProfile( tMinimaxSearch, minimaxTimeID );

	// Return selection
	return returnedMove;
}
//
// --------------------------------------------------------
//	GetMinimaxUtility - Starting address of search threads. 
//  Launches a minimax evaluation of the specified game 
//  state (Provided through the input MtGameState pointer)
// --------------------------------------------------------
unsigned int Minimax::getMinimaxUtility( void* dataOut )
{
	// Recast output pointer
	MtGameState* state = (MtGameState*)dataOut;

	// Perform minimax search
	state->utility = minimax( &state->moveLists, state->grid, state->pieces, state->score, 
		state->player, state->depth, state->ply, state->alpha, state->beta );

	// Mark the thread done
	state->completed = true;

	// Return exit code
	return 0;
}
//
// --------------------------------------------------------
//	Minimax - Uses the minimax algorithm with alpha-beta 
//  pruning to compute the utility value of a given board 
//  position. 
// --------------------------------------------------------
float Minimax::minimax( MoveLists* moveLists, short grid[][14], int pieces[], int score[], int player,
						 int depth, int ply, float alpha, float beta )
{	
	// Increment search count
	Profiler::addSearchNode( );

	// Uncomment the following to step through
	/*displayState( moveLists, grid, pieces, score, player ); 
	std::cout << "\n"; system( "pause" );*/

	// Check current depth for search tree cut-off
	if( depth == 0 ) 
	{
		// Begin profiling function
		__int64 evaluationTimeID = Profiler::startProfile( );

		// Compute board utility
		float utility = Heuristic::evalFunction[m_evalFunction[EVAL_STD]]
			( moveLists, grid, pieces, score, player );

		// Increment function runtime costs
		Profiler::endProfile( tStandard, evaluationTimeID );

		// Increment leaf node count
		Profiler::addLeafNode( );
		
		return utility;
	}

	// Check for empty move list
	if( moveLists->isMoveAvailable( player ) == FALSE )
	{
		// Player will inevitably lose
		if( score[player] < score[1-player] )
			return (player==PLAYER_MAX) ? (-FLT_MAX) : FLT_MAX;

		// Player's loss is not definitive, but player is out
		else if( moveLists->isMoveAvailable( 1-player ) == TRUE )
			return minimax( moveLists, grid, pieces, score, 1-player, depth-1, ply+1, alpha, beta );

		// Player has tied with other player
		else if( score[player] == score[1-player] )
			return 0.0f;

		// Player has won the match, all players out
		else return (player==PLAYER_MAX) ? FLT_MAX : -FLT_MAX;
	}

	// Select subset of pieces to search
	int validPieces = pieces[player];
	if( ply < 0 ) validPieces &= 0x1FF000; // Opening piece selection

	// Get the first available move from an awake liberty
	__int64 moveEnumerationTimeID = Profiler::startProfile( );
	const Move* move = moveLists->getFirstMove( player, validPieces );
	Profiler::endProfile( tMoveEnumeration, moveEnumerationTimeID );

	// If no liberties are awake eval territory
	if( move == NULL ) 
	{
		// Begin profiling function
		__int64 endEvaluationTimeID = Profiler::startProfile( );

		// Compute board utility
		float utility = Heuristic::evalFunction[m_evalFunction[EVAL_END]]
			( moveLists, grid, pieces, score, player );

		// Increment function runtime costs
		Profiler::endProfile( tEndGame, endEvaluationTimeID );

		// Increment leaf node count
		Profiler::addLeafNode( );
		
		return utility;
	}

	// Recursively perform minimax on each move
	while( move != NULL )
	{
		// Game state variables from move simulation output
		short newGrid[14][14]; int newPieces[2]; int newScore[4]; int newPlayer; 
		MoveLists newMoveLists;

		// Begin profiling move simulation
		__int64 simulationTimeID = Profiler::startProfile( );

		// Simulate the selected move on the board for minimax evaluation
		MoveSimulator::simulateMove( move, grid, pieces, score, player, 
									newGrid, newPieces, newScore, &newPlayer,
									moveLists, &newMoveLists );

		// Increment function runtime costs
		Profiler::endProfile( tSimulateMoves, simulationTimeID );

		// Perform minimax on the new board state
		float newUtility = minimax( &newMoveLists, newGrid, newPieces, 
			newScore, newPlayer, depth-1, ply+1, alpha, beta );

		// Return used memory chunks to pool
		newMoveLists.deallocateMemoryChunks( );

		// Update alpha-beta bounds
		if( player == PLAYER_MAX ) {
			if( newUtility > alpha ) alpha = newUtility; }
		else if( newUtility < beta ) beta = newUtility;

		// Check for alpha-beta cut-off
		if( beta <= alpha ) break;

		// Get next available move
		__int64 moveEnumerationTimeID = Profiler::startProfile( );
		move = moveLists->getNextMove( );
		Profiler::endProfile( tMoveEnumeration, moveEnumerationTimeID );
	}

	// Return the appropriate utility bound
	return (player==PLAYER_MAX) ? alpha : beta;
}
//
// --------------------------------------------------------
//	DisplayState - Outputs the specified game state to the 
//  console. Useful for debugging purposes. 
// --------------------------------------------------------
void Minimax::displayState( MoveLists* moves, short grid[][14], 
		int pieces[], int score[], int player )
{
	// Initialize a static pointer to a mutex object
	static HANDLE mutex = CreateMutex( NULL, FALSE, NULL );
    
	// Wait for ownership of the mutex object
	unsigned long waitResult = WaitForSingleObject( mutex, INFINITE );
 
	// Check for access
    if( waitResult == WAIT_ABANDONED ) return; 

	// Debug board display
	std::cout << "\n\n*** Board Layout ***";
	for( int i = 0; i < BOARD_SIZE; i++ ) { std::cout << "\n";
	for( int j = 0; j < BOARD_SIZE; j++ ) {
			 if( ((grid[j][i]>>(PLAYER_BLUE+EX_GRID_COVERED))&1) ) std::cout << "B ";
		else if( ((grid[j][i]>>(PLAYER_RED +EX_GRID_COVERED))&1) ) std::cout << "R ";
		else std::cout << "- ";
	} } 

	// Debug piece display
	std::cout << "\n\n*** Pieces ***";
	for( int i = 0; i < NUM_PLAYERS; i++ ) { std::cout << "\n";
	for( int j = 0; j < PIECE_COUNT; j++ ) {
		std::cout << ((pieces[i]>>j)&1);
	} } 

	// Get some additional settings data
	std::cout << "\nScore Blue: " << score[PLAYER_BLUE];
	std::cout << "\nScore Red: " << score[PLAYER_RED];
	std::cout << "\nPlayer: " << player; 
	std::cout << "\nUtility: " << 
		Heuristic::evalFunction[m_evalFunction[EVAL_STD]]
			( moves, grid, pieces, score, player );

	// Release mutex ownership
	ReleaseMutex( mutex );
}