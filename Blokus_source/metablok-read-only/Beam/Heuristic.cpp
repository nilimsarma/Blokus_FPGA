/* ===========================================================================

	Project: Beam AI player for Blokus

	Description:
	  Contains a series of evaluation functions for rating Blokus Duo board
	  positions in the extended board format.

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

// Standard Includes
#include "Includes.h"

// Include header
#include "Heuristic.h"

// Board evaluation heuristic function prototypes
float random( MoveLists* moves, short grid[][14], int pieces[], int score[], int player );
float simple( MoveLists* moves, short grid[][14], int pieces[], int score[], int player );
float weight( MoveLists* moves, short grid[][14], int pieces[], int score[], int player );
float region( MoveLists* moves, short grid[][14], int pieces[], int score[], int player );
float spaces( MoveLists* moves, short grid[][14], int pieces[], int score[], int player );

// Board evaluation function count
const int Heuristic::nEvaluationFunctions = 5;

// Board evaluation function descriptions
const char* Heuristic::evalFunctionName[nEvaluationFunctions] = {
	"Random utility evaluation heuristic.",
	"Simple score based evaluation heuristic.",
	"Simple score based on weighted liberties.",
	"Combined ranking of score and player influence."
	"Combined ranking of score and liberty influence."
};

// Board evaluation function pointers
const EvalFunction Heuristic::evalFunction[nEvaluationFunctions] = {
	&random,    // "Random utility evaluation heuristic."
	&simple,    // "Simple score based evaluation heuristic"
	&weight,    // "Simple score based on weighted liberties."
	&region,	// "Combined ranking of score and player influence."
	&spaces,	// "Combined ranking of score and liberty influence."
};

// --------------------------------------------------------
//	Random board evaluation heuristic. Returns a random
//  utility value between FLT_MAX and FLT_MIN.
// --------------------------------------------------------
float random( MoveLists* moves, short grid[][14], int pieces[], 
			 int score[], int player ) 
{
	return (float)( rand( ) - RAND_MAX/2 ) ;
}
//
// --------------------------------------------------------
//	Simple board evaluation heuristic. Returns the total
//  difference in score between max player and min player.
// --------------------------------------------------------
float simple( MoveLists* moves, short grid[][14], int pieces[], 
			 int score[], int player ) 
{
	return (float)( score[PLAYER_MAX] - score[PLAYER_MIN] );
}
//
// --------------------------------------------------------
//	Simple board evaluation heuristic. Weights liberties
//  using data from the MoveLists for each player. This is
//  an excellent heuristic to attempt neural network 
//  integration with at some point.
// --------------------------------------------------------
float weight( MoveLists* moves, short grid[][14], int pieces[],
			 int score[], int player )
{
	// Static liberty weight values
	static const int WEIGHT_SCORE	 =  100;
	static const int WEIGHT_ATMOVE	 =  200;
	static const int WEIGHT_DEFAULT  =  100;
	static const int WEIGHT_FIGHTING =    0;
	static const int WEIGHT_PIECE	 =   10;
	static const int WEIGHT_MOVE	 =    0;
	static const int WEIGHT_DIST	 =   33;
	static const int WEIGHT_ANG_GOOD =   33;
	static const int WEIGHT_ANG_BAD  =    0;
	static const int WEIGHT_LEAK	 = 1000;

	// Initialize weights with player to move advantage
	int moveAdv = (player == PLAYER_MAX) ? WEIGHT_ATMOVE : -WEIGHT_ATMOVE;
	int weights[NUM_PLAYERS] = { 0, 0 };

	// Compute weights for individual players
	for( int p = 0; p < NUM_PLAYERS; p++ )
	{
		MoveList* iter = moves->getList( p );
		if( moves->isMoveAvailable( p ) ) do
		{
			// Weights for available pieces
			int validPieces = iter->getValidPieces( );
			for( int i = 0; i < PIECE_COUNT; i++ )
			if( validPieces & (1<<i) ) 
				weights[p] += WEIGHT_PIECE;

			// Weights for available moves
			weights[p] += iter->getNumMoves( ) * WEIGHT_MOVE;

			// Weights for fighting liberties
			if( iter->isFighting( ) ) weights[p] += WEIGHT_FIGHTING;

			// Weight for center-ness ( Technically incorrect )
			int x = abs( iter->getPositionX( ) - BOARD_SIZE/2 );
			int y = abs( iter->getPositionY( ) - BOARD_SIZE/2 );
			weights[p] += ( BOARD_SIZE/2 - max( x, y ) ) * WEIGHT_DIST;

			// Weight for different liberty angles
			if( iter->getAngle( ) == (3-p*2) ) weights[p] += WEIGHT_ANG_GOOD;
			else if( iter->getAngle( ) == (p*2+1) ) weights[p] += WEIGHT_ANG_BAD;
			
			// Weight for potential leaks ( ignore 1 & 2 piece leaks )
			if( iter->isLeak( ) && (iter->getValidPieces( )&0x1FFFFC) )
				weights[p] += WEIGHT_LEAK;

			// Default weight for active liberty
			weights[p] += WEIGHT_DEFAULT;
		} 
		while( iter = iter->getNext( ) ); 
	}

	// Compute total utility value
	int utility = weights[PLAYER_MAX] - weights[PLAYER_MIN]
		+ WEIGHT_SCORE * ( score[PLAYER_MAX] - score[PLAYER_MIN] );

    // Give player to move advantage
	return (float)( utility + moveAdv );
}
//
// --------------------------------------------------------
//	Simple board evaluation heuristic. Adds a fraction of
//  the player's influence territory to their score.
// --------------------------------------------------------
float region( MoveLists* moves, short grid[][14], int pieces[],
			 int score[], int player ) 
{
	// Static weight values
	static const int WEIGHT_AREAS = 1;
	static const int WEIGHT_SCORE = 3;

	// Territory influence map
	InfluenceMap influenceMap;
	int areas[NUM_PLAYERS];

	// Generate influence map and compute influence regions
	influenceMap.generate(moves, grid, pieces, score, player );

	// Returned ranking
	areas[PLAYER_MAX] = influenceMap.getPlayerInfluence( PLAYER_MAX );
	areas[PLAYER_MIN] = influenceMap.getPlayerInfluence( PLAYER_MIN );

	// Return weighted board utility
	return ( float )
		   ( WEIGHT_SCORE * (score[PLAYER_MAX] - score[PLAYER_MIN]) +
		     WEIGHT_AREAS * (areas[PLAYER_MAX] - areas[PLAYER_MIN]) );
}
//
// --------------------------------------------------------
//	Simple board evaluation heuristic. Adds a fraction of
//  the player's libertie's open tiles to their score.
// --------------------------------------------------------
float spaces( MoveLists* moves, short grid[][14], int pieces[],
			 int score[], int player ) 
{
	// Static weight values
	static const int WEIGHT_AREAS = 3;
	static const int WEIGHT_SCORE = 1;

	// Total free area / player
	int areas[2] = { 0, 0 };

	// Compute weights for individual players
	for( int p = 0; p < NUM_PLAYERS; p++ ) {
		MoveList* iter = moves->getList( p );
		if( moves->isMoveAvailable( p ) ) do
			areas[p] += iter->getFreeSpace( );
		while( iter = iter->getNext( ) ); }

	// Return weighted board utility
	return ( float )
		   ( WEIGHT_SCORE * (score[PLAYER_MAX] - score[PLAYER_MIN]) +
		     WEIGHT_AREAS * (areas[PLAYER_MAX] - areas[PLAYER_MIN]) );
}