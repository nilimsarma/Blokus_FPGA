/* ===========================================================================

	Project: Beam AI player for Blokus

	Description:
	  Performs influence analysis on the given board state.

    Copyright (C) 2011 Lucas Sherman

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

// Standard includes
#include "Includes.h"

// Include header
#include "InfluenceMap.h"

// --------------------------------------------------------
//	ComputeInfluenceMap - Generates an influence map for 
//  the specified game state.
// --------------------------------------------------------
void InfluenceMap::generate( MoveLists* lists, short grid[][14], 
		int pieces[], int score[], int player )
{
	// Create queue for influence propogation
	Influence queue[BOARD_SIZE*BOARD_SIZE];
	int queueWrite = 0, queueRead = 0;

	// Zero influence map and influence buffer memory
	ZeroMemory( m_influence, sizeof(char)*14*14 );

	// Initialize influence map 
	for( int p = 0; p < NUM_PLAYERS; p++ )
		for( MoveList* iter = lists->getList( p ); 
			iter != NULL; iter = iter->getNext( ) ) 
		{
			int x = iter->getPositionX( );
			int y = iter->getPositionY( );

			m_influence[x][y] |= (1<<p)|0x4;

			if( !(m_influence[x][y]&0x8) )
			{
				m_influence[x][y] |= 0x8;
				queue[queueWrite].x = x; 
				queue[queueWrite].y = y; 
				queueWrite++;
			}
		}

	// Spread influence outward
	while( queueRead != queueWrite )
	{ 
		static const int NUM_NEIGHBORS = 8;
		static const int nx[NUM_NEIGHBORS] = { -1, 0, 1,  0, -1, -1, 1,  1 };
		static const int ny[NUM_NEIGHBORS] = {  0, 1, 0, -1, -1,  1, 1, -1 };

		int x = queue[queueRead].x;
		int y = queue[queueRead].y;

		// Check for territory clash
		if( (m_influence[x][y]&0x3) != 0x3 )
		for( int n = 0; n < NUM_NEIGHBORS; n++ )
		{
			int cx = x + nx[n];
			int cy = y + ny[n];

			// Check for invalid child coordinates
			if( cx >= 0 && cx < BOARD_SIZE && 
				cy >= 0 && cy < BOARD_SIZE &&
				!( m_influence[cx][cy] & 0x4 ) )
			{
				// Add the node to the queue
				if( !(m_influence[cx][cy]&0x8) )
				{
					ASSERT( queueWrite < 196 );
					queue[queueWrite].x = cx;
					queue[queueWrite].y = cy;
					queueWrite++;
				}

				// Spread influence to the tile
				m_influence[cx][cy] |= (m_influence[x][y]&0xB);
				ASSERT( m_influence[cx][cy]&0x8 );
			}
		}

		// Mark search completed
		m_influence[x][y] |= 0x4;

		queueRead++;
	}
}
//
// --------------------------------------------------------
//	DisplayMap - Outputs the influence map to console.
// --------------------------------------------------------
void InfluenceMap::displayMap( )
{
	// Initialize a static pointer to a mutex object
	static HANDLE mutex = CreateMutex( NULL, FALSE, NULL );
    
	// Wait for ownership of the mutex object
	unsigned long waitResult = WaitForSingleObject( mutex, INFINITE );
 
	// Check for access
    if( waitResult == WAIT_ABANDONED ) return; 

	// Display path multiplicity table
	std::cout << "\nInfluence Map:";
	for( int y = 0; y < BOARD_SIZE; y++ ) { std::cout << "\n";
	for( int x = 0; x < BOARD_SIZE; x++ ) { std::cout << " ";
		if( m_influence[x][y] & 0x4 )
			if( (m_influence[x][y] & 0x3) == 0x3 ) std::cout << "O"; 
			else if( m_influence[x][y] & 0x1 ) std::cout << "B"; 
			else if( m_influence[x][y] & 0x2 ) std::cout << "R"; 
			else std::cout << "-";
		else std::cout << "-"; } }

	// Release mutex ownership
	ReleaseMutex( mutex );
}