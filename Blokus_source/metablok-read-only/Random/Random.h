/* ===========================================================================

	Project: Random AI for Blokus

	Description:
	 A simple AI player that chooses a move at random from all the
	 moves available this turn and returns it.

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

// Begin definition
#ifndef RANDOM_H
#define RANDOM_H

// Define player
class Random
{
public:
	// Initialize the AI players settings data
	void startup( int boardSize, int startTile[][2], int nPlayers );

	// Enumerate available moves and select one at random to return
	Move makeMove( char grid[][20], bool pieces[][21], int score[], int player );

	// Shutdown AI player
	void shutdown( ) { }

private:
	// Piece layout structure
	struct Piece { int sizeX, sizeY; int rot; int flip; char layout[7][6]; };

	// Enumerates all available moves for the given game state
	void getAvailableMoves( std::vector<Move>* out, char grid[][20], 
		bool pieces[][21], int player );

	// Valid move checking
	bool isValidMove( Move move, char grid[][20], int player );
	bool isValidPattern( int pieceNumber, bool* liberty, int px, int py, 
		int gx, int gy, char grid[][20], int player );

	// Load piece configurations
	void loadPieceConfigs( );
	
	// Match settings data
	int m_nPlayers, m_boardSize;    
	int m_startTile[4][2];
	Piece m_piece[21];
};

// End definition
#endif
