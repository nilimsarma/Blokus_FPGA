/* ===========================================================================

	Project: Monte AI for Blokus

	Description:
	 An AI player which utilizes the Monte-Carlo method for move selection.

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

// Standard Includes
#include "Includes.h"

// Include header
#include "Monte.h"

// --------------------------------------------------------
//	Startup - Sets the seed for the rand generator and 
//  stores the match settings data.
// --------------------------------------------------------
void Monte::startup( int boardSize, int startTile[][2], int nPlayers ) 
{
	// Generate a new random seed
	srand( (unsigned int)time(NULL) );

	// Load piece data
	loadPieceConfigs( );

	// Store player/board data
	m_boardSize = boardSize;
	m_nPlayers = nPlayers;

	// Store starting liberty tiles
	for( int i = 0; i < nPlayers; i++ ) {
		m_startTile[i][0] = startTile[i][0];
		m_startTile[i][1] = startTile[i][1]; }
} 
//
// --------------------------------------------------------
//	MakeMove - Returns a move at random from those that
//  are returned by the GetAvailableMoves function.
// --------------------------------------------------------
Move Monte::makeMove( char grid[][20], bool pieces[][21], int score[], int player )
{
	// Enumerate all available moves
	std::vector<Move> moves; moves.reserve( 1200 );
	getAvailableMoves( &moves, grid, pieces, player );

	// Select random element 
 	int index = rand( ) % moves.size( );
	return moves[index];
}
//
// --------------------------------------------------------
//	GetAvailableMoves - Enumerates all available moves by 
//  searching through all possible orientations of a piece 
//  on the board.
// --------------------------------------------------------
void Monte::getAvailableMoves( std::vector<Move>* out, 
			char grid[][20], bool pieces[][21], int player )
{
	Move move;

	// Compile move list
	for( int i = 0; i < 21; i++ ) 
	if( pieces[player][i] )
	{
		move.pieceNumber = i;

		// Precompute iteration bounds
		int maxSizeX = m_boardSize - m_piece[move.pieceNumber].sizeX + 2;
		int maxSizeY = m_boardSize - m_piece[move.pieceNumber].sizeY + 2;
		int maxRange = max( maxSizeX, maxSizeY );

		// Check for valid move with subrange
		for( move.rotated = 0; move.rotated < m_piece[move.pieceNumber].rot; move.rotated++ ) 
		for( move.flipped = 0; move.flipped <= m_piece[move.pieceNumber].flip; move.flipped++ ) 
		for( move.gridX = -1; move.gridX < maxRange; move.gridX++ )
		for( move.gridY = -1; move.gridY < maxRange; move.gridY++ )
		if( isValidMove( move, grid, player ) ) out->push_back( move );
	}
}
//
// --------------------------------------------------------
//	IsValidMove - Checks if the given move is valid.
// --------------------------------------------------------
bool Monte::isValidMove( Move move, char grid[][20], int player )
{
	bool coversLiberty = false;

	// Run pattern analysis between grid and piece
	int gx = move.gridX, gy = move.gridY;
	int x = m_piece[move.pieceNumber].sizeX;
	int y = m_piece[move.pieceNumber].sizeY;
	if( move.flipped == PIECE_UNFLIPPED )
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j = 0, gy = move.gridY; j < y; j++,gy++ )
			for( int i = 0, gx = move.gridX; i < x; i++,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy, grid, player ) ) return false;
		}

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j =   0, gx = move.gridX; j <  y; j++,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy, grid, player ) ) return false;
		}

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy, grid, player ) ) return false;
		}

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i =   0, gy = move.gridY; i <  x; i++,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy, grid, player ) ) return false;
		}
	} else 
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j =   0, gy = move.gridY; j <  y; j++,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy, grid, player ) ) return false;
		}

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy, grid, player ) ) return false;
		}

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i =   0, gx = move.gridX; i <  x; i++,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy, grid, player ) ) return false;
		}

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i = 0, gy = move.gridY; i < x; i++,gy++ )
			for( int j = 0, gx = move.gridX; j < y; j++,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy, grid, player ) ) return false;
		}
	}

	// Check if liberty is covered
	if( coversLiberty ) return true;
	else return false;
}
//
// --------------------------------------------------------
//	IsValidPattern - Checks if the patterns on the grid and 
//  piece are compatible and updates the liberty covered 
//  flag if a liberty was covered.
// --------------------------------------------------------
bool Monte::isValidPattern( int pieceNumber, bool* liberty, int px, int py, 
						   int gx, int gy, char grid[][20], int player )
{
	// Check for outside grid bounds on a covering tile
	char piecePattern = m_piece[pieceNumber].layout[px][py];
	if( gx < 0 || gx >= m_boardSize || gy < 0 || gy >= m_boardSize ) 
		if( piecePattern == MATCH_NOT_COVERED ) return false;
		else return true;

	// Check if local patterns are acceptable
	char gridPattern = grid[gx][gy];
	switch( piecePattern ) {
		case MATCH_NOT_PLAYERS: 
			if( gridPattern == player ) return false; break;
		case MATCH_NOT_COVERED:
			if( gridPattern != GRID_COVER_NONE ) return false; 
			else if( gx == m_startTile[player][0] && 
					 gy == m_startTile[player][1] ) *liberty = true; break;
		case MATCH_LIBERTY:
			if( gridPattern == player ) *liberty = true; }

	// Pattern valid
	return true;
}
//
// --------------------------------------------------------
//	LoadPieceConfigs - Initializes the piece information 
//  structures using information form the provided file.
// --------------------------------------------------------
void Monte::loadPieceConfigs( )
{
	// Open piece structure file for reading
	std::wstring filename = std::wstring( L"Pieces.txt" );
	std::fstream file( filename.c_str( ), std::ios::in );

	// Check for file load failure
	if( !file.is_open( ) ) {
		std::cerr << "Couldn't open Pieces.txt!" << std::endl;
		throw 1; // TODO: better error handling
	}

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