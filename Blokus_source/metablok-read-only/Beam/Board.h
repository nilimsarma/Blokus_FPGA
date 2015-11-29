
/* ===========================================================================

Project: Blokus AI - Beam

	Represents a game state in a Blokus Match.

File:      Board.h
Author:    Lucas Sherman
Date:	   November 21th, 2011

=========================================================================== */

// Begin definition
#ifndef BOARD_H
#define BOARD_H

// Standard Includes
#include "Includes.h"

// Board state
struct Board
{
	short m_grid[BOARD_SIZE][BOARD_SIZE]; 
	int m_pieces[NUM_PLAYERS]; 
	int m_score[NUM_PLAYERS]; 
	int m_player;
};

// End definition 
#endif