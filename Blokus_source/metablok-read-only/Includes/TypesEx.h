/* ===========================================================================

	Project: AI player for Blokus

	Description:
	 Define some extended board formatting options used by the Minimax and
	 Beam AI players for more efficient move enumeration.

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

// Begin definitions
#ifndef TYPES_EX_H
#define TYPES_EX_H

// Minimax Player Types
#define PLAYER_MAX PLAYER_BLUE
#define PLAYER_MIN PLAYER_RED

// Minimax settings
#define BOARD_SIZE  14
#define NUM_PLAYERS  2

// Extended board format accessor
#define EX_GRID_IS( g, t, p )  (g & ((p+1)<<t))
#define EX_PIECE_IS( p, t )	   (p == t)

// Extended board format 
#define EX_GRID_COVERED     0
#define EX_GRID_NOT_SAFE    2
#define EX_GRID_LBTY_UR     4
#define EX_GRID_LBTY_UL     6
#define EX_GRID_LBTY_LL     8
#define EX_GRID_LBTY_LR    10
#define EX_GRID_LBTY_LK	   12

// Liberty by angle accessor
#define EX_LBTY_ANGLE( a ) (EX_GRID_LBTY_UR+2*a)

// Extended piece format
#define EX_MATCH_DONT_CARE    0
#define EX_MATCH_NOT_PLAYERS  1
#define EX_MATCH_NOT_COVERED  2
#define EX_MATCH_LIBERTY      3
#define EX_MATCH_LBTY_UR	  4
#define EX_MATCH_LBTY_UL	  5
#define EX_MATCH_LBTY_LL	  6
#define EX_MATCH_LBTY_LR	  7

// End definition
#endif