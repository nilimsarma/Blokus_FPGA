/* ===========================================================================

	Project: AI player for Blokus

	Description:
	 Loads the layouts for each of the 21 game pieces from the piece layout
	 file.

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
#ifndef PIECE_SET_H
#define PIECE_SET_H

// Singleton piece set
class PieceSet 
{
private:	
	// Piece layout structure
	struct Piece { int sizeX, sizeY; int rot; int flip; char layout[7][6]; };
	PieceSet( ); Piece m_piece[21];

public:
	// Access protector functions for AI piece analysis
	int getSizeX( int piece ) { return m_piece[piece].sizeX; }
	int getSizeY( int piece ) { return m_piece[piece].sizeY; } 
	int getOrients( int piece ) { return m_piece[piece].rot; }
	int getFlips( int piece ) { return m_piece[piece].flip; }
	char indexOf( int piece, int x, int y  )
	{ return m_piece[piece].layout[x][y]; }

	// Singleton accessor	
	static PieceSet* instance( )
	{
		static PieceSet gSingleton;
		return &gSingleton;
	}
};

// End definition
#endif