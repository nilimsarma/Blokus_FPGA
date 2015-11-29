/* ===========================================================================

	Project: AI player for Blokus

	Description:
	  Provides an interface for loading the piece configuration file supplied
	  to AI players at runtime. Automatically converts the pieces to an extended
	  format for fast AI reasoning.

      Copyright (C) 2011 Lucas Sherman

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
#ifndef PIECE_H
#define PIECE_H

// Piece names from pids
enum PieceType {
	PIECE_1		=  0, 
	PIECE_2		=  1,
	PIECE_I3	=  2,
	PIECE_V3	=  3,
	PIECE_T4	=  4,
	PIECE_Z4	=  5,
	PIECE_I4	=  6,
	PIECE_L4	=  7,
	PIECE_O		=  8,
	PIECE_P		=  9,
	PIECE_U		= 10,
	PIECE_F		= 11,
	PIECE_I5	= 12,
	PIECE_L5	= 13,
	PIECE_V5	= 14,
	PIECE_N		= 15,
	PIECE_W		= 16,
	PIECE_Z5	= 17,
	PIECE_Y		= 18,
	PIECE_T		= 19,
	PIECE_X		= 20,
	PIECE_COUNT = 21
};

// Max liberties per piece
#define MAX_LIBERTIES 14

// Piece liberty structure
class Liberty {
	friend class PieceSet;
	friend class Piece;

public:
	int isFlipped( ) { return flipped; }
	int getAngle( ) { return angle; }
	int getPosX( ) { return x; }
	int getPosY( ) { return y; }

private:
	int x, y;		//< Position of the liberty on the piece
	int angle;		//< Orient of liberty ( UL, UR, LL, or LR )
	bool flipped;	//< Does the liberty appear when flipped?
};

// Piece structure
class Piece { 
	friend class PieceSet;

public:
	int getSizeX( ) { return sizeX; }
	int getSizeY( ) { return sizeY; }
	int getNumOfRots( ) { return rot; }
	int isFlippable( ) { return flip;  }
	short getLayout( int x, int y )
		{ return layout[x][y]; }

	int getNumOfLiberties( ) { return numOfLiberties; }
	Liberty* getLiberty( int lid )
	{ return liberties+lid;}

private:
	int sizeX, sizeY;		//< Dimensions of piece
	int flip; int rot;		//< Number of unique orients
	short layout[7][6];		//< Layout of piece pattern
	
	Liberty liberties[MAX_LIBERTIES];
	int numOfLiberties;
};

// Piece set
class PieceSet 
{
public:
	// Piece data initialization function
	static void initPieceConfigurations( );

	// Debug piece data display
	static void displayPiece( int pid );

	// Piece object accessor
	static Piece* getPiece( int pid ) 
	{ return m_piece+pid; }

private:	
	PieceSet( );

	// Piece extension
	static void extendPieces( );

	// Piece data objects
	static bool m_initialized;
	static Piece m_piece[21];
};

// End definition
#endif