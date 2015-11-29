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

// Include header
#include "PieceSet.h"

// Standard includes
#include <fstream>

// --------------------------------------------------------
//	Initializes the piece information structures
// --------------------------------------------------------
PieceSet::PieceSet( )
{
	// Open piece structure file for reading
	std::fstream file( "Pieces.txt" , std::ios::in );

	// Check for file load failure
	if( !file.is_open( ) );// ERROR MSG

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
			file >> m_piece[i].layout[j][k];
	}
}
