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

// Standard includes
#include <iostream>
#include <fstream>
#include "Types.h"
#include "TypesEx.h"
#include "Debug.h"

// Include header
#include "Piece.h"

// Static member variables
bool PieceSet::m_initialized;
Piece PieceSet::m_piece[21];

// --------------------------------------------------------
//	InitPieceConfigurations - Initializes the piece 
//  configuration data using the piece.txt file provided
//  to every AI. It is always in the initial working 
//  directory throughout execution and accessable for
//  reading at any time.
// --------------------------------------------------------
void PieceSet::initPieceConfigurations( )
{
	// Check for initialization
	if( m_initialized ) return; 

	// Open piece structure file for reading
	std::wstring filename = std::wstring( L"Pieces.txt" );
	std::fstream file( filename.c_str( ), std::ios::in );

	// Check for file load failure
	if( !file.is_open( ) ) {
		std::cout << "File not found: \"Pieces.txt\"";
		system("pause"); exit( 1 ); }

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
		{
			// Get piece format
			file >> m_piece[i].layout[j][k];

			// Convert to extended format
			switch( m_piece[i].layout[j][k] ) {
				case MATCH_NOT_PLAYERS: m_piece[i].layout[j][k] = EX_MATCH_NOT_PLAYERS; break;
				case MATCH_NOT_COVERED: m_piece[i].layout[j][k] = EX_MATCH_NOT_COVERED; break; 
				case MATCH_DONT_CARE: m_piece[i].layout[j][k] = EX_MATCH_DONT_CARE; break; 
				case MATCH_LIBERTY: m_piece[i].layout[j][k] = EX_MATCH_LIBERTY; break; }
		}
	}

	// Extend format
	extendPieces( );

	// Mark initialized
	m_initialized = true;
}
//
// --------------------------------------------------------
//  ExtendPieces - Initializes the relevant piece liberties 
//  and converts the piece respresentations to extended
//  format. Only liberties which will produce unique piece
//  placements are added to the internal liberty list but
//  all liberties in the piece layout are updated.
// --------------------------------------------------------
void PieceSet::extendPieces( )
{
	// Cycle through available pieces for liberties
	for( int i = 0; i < PIECE_COUNT; i++ ) 
	{
		// Get piece handle and init lib count
		Piece* piece = m_piece + i;
		piece->numOfLiberties = 0;

		// Cycle through piece pattern and mark liberties
		for( int x = 0; x < m_piece[i].sizeX; x++ )
		for( int y = 0; y < m_piece[i].sizeY; y++ )
		if( m_piece[i].layout[x][y] == EX_MATCH_LIBERTY )
		{
			//			-------------------
			//			 UNFLIPPED LIBERTY
			//			-------------------

			// Initialize the liberty position
			Liberty* next = piece->liberties + piece->numOfLiberties;
			next->x = x; next->y = y; next->flipped = false;

			// Check which type of liberty was here by examining the 
			// diagonally adjacent sqaures to this one.
			if( x < m_piece[i].sizeX-1 && y < m_piece[i].sizeY-1 
				&& m_piece[i].layout[x+1][y+1] == EX_MATCH_NOT_COVERED ) {
					if( m_piece[i].rot > 1 ) { next->angle = 1; piece->numOfLiberties++; }
					m_piece[i].layout[x][y] = EX_MATCH_LBTY_UL; }
			else if( x > 0 && y < m_piece[i].sizeY-1 
					 && m_piece[i].layout[x-1][y+1] == EX_MATCH_NOT_COVERED ) { 
						 next->angle = 0; piece->numOfLiberties++; m_piece[i].layout[x][y] = EX_MATCH_LBTY_UR; }
			else if( x>0 && y>0 && m_piece[i].layout[x-1][y-1] == EX_MATCH_NOT_COVERED ) {
				if( m_piece[i].rot > 3 ) { next->angle = 3; piece->numOfLiberties++; }
				m_piece[i].layout[x][y] = EX_MATCH_LBTY_LR; }
			else if( x<m_piece[i].sizeX-1 && y>0 && m_piece[i].layout[x+1][y-1] == EX_MATCH_NOT_COVERED ) {
				if( m_piece[i].rot > 2 ) { next->angle = 2; piece->numOfLiberties++; }
				m_piece[i].layout[x][y] = EX_MATCH_LBTY_LL; }

			//			-------------------
			//			  FLIPPED LIBERTY
			//			-------------------

			// Flipped liberties
			if( m_piece[i].flip )
			{
				// Initialize the liberty position
				next = piece->liberties + piece->numOfLiberties;
				next->x = m_piece[i].sizeX - 1 - x; next->y = y;
				next->flipped = true;

				// Check which type of liberty was here by examining the 
				// diagonally adjacent sqaures to this one.
				if( x<m_piece[i].sizeX-1 && y<m_piece[i].sizeY-1 && m_piece[i].layout[x+1][y+1] == EX_MATCH_NOT_COVERED )
					if( m_piece[i].rot > 1 ) { next->angle = 0; piece->numOfLiberties++; }
				if( x>0 && y<m_piece[i].sizeY-1 && m_piece[i].layout[x-1][y+1] == EX_MATCH_NOT_COVERED ) 
					{ next->angle = 1; piece->numOfLiberties++; }
				if( x>0 && y>0 && m_piece[i].layout[x-1][y-1] == EX_MATCH_NOT_COVERED ) {
					if( m_piece[i].rot > 3 ) { next->angle = 2; piece->numOfLiberties++; } }
				if( x<m_piece[i].sizeX-1 && y>0 && m_piece[i].layout[x+1][y-1] == EX_MATCH_NOT_COVERED ) {
					if( m_piece[i].rot > 2 ) { next->angle = 3; piece->numOfLiberties++; } }
			}
		}
		
		// Verify const max liberties limit
		ASSERT( piece->numOfLiberties <= MAX_LIBERTIES )
	}
}
//
// --------------------------------------------------------
//  DisplayPieces - Prints the specified piece layout to 
//  standard io. If the pid is invalid, all piece layouts
//  will be displayed in order from 0 to PIECE_COUNT.
// --------------------------------------------------------
void PieceSet::displayPiece( int pid )
{ 
	// Get range of pids to display
	int pidmin = pid, pidmax = pid;
	if( pid < 0 ) { pidmin = 0; pidmax = PIECE_COUNT-1; }
	if( pid >= PIECE_COUNT ) 
		{ pidmin = 0; pidmax = PIECE_COUNT-1; }

	// Cycle through specified pid range
	for( int i = pidmin; i <= pidmax; i++ )
	{
		// Piece statistics
		std::cout << "\n\nPiece Number: " << i
			<< "\nSize: ( " << m_piece[i].sizeX << ", " 
			<< m_piece[i].sizeY << ")"
			<< "\nRotations: " << m_piece[i].rot
			<< "\nFlips: " << m_piece[i].flip;
		
		// Piece pattern layout 
		std::cout << "\nPiece Layout: ";
		for( int y = 0; y < m_piece[i].sizeY; y++ ) { std::cout << "\n";
		for( int x = 0; x < m_piece[i].sizeX; x++ )
			std::cout << m_piece[i].layout[x][y] << " "; }
	}
	
	// Deliminate display
	std::cout << "\n\n";
}