
/* ===========================================================================

Project: Blokus AI

	Includes an extended board definition available for use by AI players
	in performing fast board analysis.

File:      TypesEx.h
Author:    Lucas Sherman

=========================================================================== */

// Begin definitions
#ifndef TYPES_EX_H
#define TYPES_EX_H

// Standard includes
#include "Types.h"

// Grid flags (Extended Format)
#define GRID_EX_IS_COVERED( g, x, y, p ) (g[x][y]&((p+1)<<0))
#define GRID_EX_IS_BLOCKED( g, x, y, p ) (g[x][y]&((p+1)<<4))
#define GRID_EX_IS_LIBERTY( g, x, y, p ) (g[x][y]&((p+1)<<8))

// Grid definitions (Extended Format)
#define GRID_EX_COVER_BLUE	 0x0001
#define GRID_EX_COVER_RED	 0x0002
#define GRID_EX_COVER_GREEN	 0x0004
#define GRID_EX_COVER_YELLOW 0x0008

#define GRID_EX_BLOCK_BLUE   0x0010
#define GRID_EX_BLOCK_RED    0x0020
#define GRID_EX_BLOCK_GREEN  0x0040
#define GRID_EX_BLOCK_YELLOW 0x0080

#define GRID_EX_DIAG_BLUE	 0x0100
#define GRID_EX_DIAG_RED	 0x0200
#define GRID_EX_DIAG_GREEN	 0x0400
#define GRID_EX_DIAG_YELLOW  0x0800

// Takes a standard board as input and converts it to extended form
void normalBoardToExtended( char boardIn[][20], short boardOut[][20], int startTile[][2], int boardSize );

// Executes a move on the extended board
void makeMoveEx( Move move, short boardIn[][20], short boardOut[][20] ); 

// End definition
#endif