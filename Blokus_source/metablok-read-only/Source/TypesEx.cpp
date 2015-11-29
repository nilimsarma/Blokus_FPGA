
/* ===========================================================================

Project: Blokus AI

	Includes some extended definitions available for use by AI players
	as well as optimized board reasoning functions.

File:      BoardEx.cpp
Author:    Lucas Sherman

=========================================================================== */

// Standard includes
#include <vector>

// Include header
#include "TypesEx.h"
#include "Types.h"

// --------------------------------------------------------
// Takes a standard board as input and converts it to 
// extended form stored into board out.
// --------------------------------------------------------
void normalBoardToExtended( char boardIn[][20], short boardOut[][20], int startTile[][2], int boardSize )
{
	for( int i = 0; i < boardSize; i++ )
	for( int j = 0; j < boardSize; j++ )
	{
		if( boardIn[i][j] != GRID_COVER_NONE ) boardOut[i][j] = (short)boardIn[i][j]+1;
		else { boardOut[i][j] = 0;
			// Check for starting liberty
			for( int k = 0; k < 2; k++ )
			if( i == startTile[k][0] && 
				j == startTile[k][1] ) {
				boardOut[i][j] = ((k+1)<<8);
				continue; }

			// Check for adjacent covered tile
			if( i>0 && boardIn[i-1][j]!=GRID_COVER_NONE ) 
				boardOut[i][j] |= ((boardIn[i-1][j]+1)<<4);
			if( j<boardSize-1 && boardIn[i][j+1]!=GRID_COVER_NONE )
				boardOut[i][j] |= ((boardIn[i][j+1]+1)<<4);
			if( i<boardSize-1 && boardIn[i+1][j]!=GRID_COVER_NONE )
				boardOut[i][j] |= ((boardIn[i+1][j]+1)<<4);
			if( j>0 && boardIn[i][j-1]!=GRID_COVER_NONE )
				boardOut[i][j] |= ((boardIn[i][j-1]+1)<<4);
			
			// Check if this tile is a liberty for any player
			if( (i>0		   && j>0           && boardIn[i-1][j-1]!=GRID_COVER_NONE) && !((boardOut[i][j])&((boardIn[i-1][j-1]+1)<<4)) ) 
				boardOut[i][j] |= ((boardIn[i-1][j-1]+1) << 8);
			if( (i>0		   && j<boardSize-1 && boardIn[i-1][j+1]!=GRID_COVER_NONE) && !((boardOut[i][j])&((boardIn[i-1][j+1]+1)<<4)) ) 
				boardOut[i][j] |= ((boardIn[i-1][j+1]+1) << 8);
			if( (i<boardSize-1 && j<boardSize-1 && boardIn[i+1][j+1]!=GRID_COVER_NONE) && !((boardOut[i][j])&((boardIn[i+1][j+1]+1)<<4)) ) 
				boardOut[i][j] |= ((boardIn[i+1][j+1]+1) << 8);
			if( (i<boardSize-1 && j>0			&& boardIn[i+1][j-1]!=GRID_COVER_NONE) && !((boardOut[i][j])&((boardIn[i+1][j-1]+1)<<4)) ) 
				boardOut[i][j] |= ((boardIn[i+1][j-1]+1) << 8);
		}
	}
}

