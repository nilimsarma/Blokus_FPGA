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

// Begin definition
#ifndef INFLUENCE_H
#define INFLUENCE_H

// Move simulator class
class InfluenceMap
{
public:
	// Influence map generation
	void generate( MoveLists* lists, short grid[][14], 
		int pieces[], int score[], int player );

	// Influence map accessors
	void isInfluencedByPlayer( int x, int y, int player );
	int getPlayerInfluence( int player ) { return m_areas[player]; }
	int getConflictedInfluence( ) { return m_borderAreas; }

	// Outputs the map to console
	void displayMap( );

private:
	struct Influence { int x, y, i, p; };
	char m_influence[14][14];
	int m_areas[NUM_PLAYERS];
	int m_borderAreas;
};

// End definition 
#endif