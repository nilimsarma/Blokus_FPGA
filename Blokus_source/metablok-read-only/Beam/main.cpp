/* ===========================================================================

	Project: Blokus AI - Beam 

	Description:
	 An AI player similar to Minimax with more extensive pruning techniques.
	 Rather than simple Alpha-Beta pruning, liberties which fall outside a
	 standard FIGHTING_DISTANCE from liberties of the other player are 
	 ignored. See the documentation for more implementation details.

    Copyright (C) 2011  <name of author>

		Lucas Sherman, email: LucasASherman@umn.edu, phone: 763-786-6916

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

// Application entry point
int main( int argc, char* argv[] )
{
	// AI Player
	Minimax player;

	// Display AI player header
	std::cout << " ***************************\n";
	std::cout << "    Beam Search AI Player\n";
	std::cout << " ***************************\n";

	// Display AI player filename
	std::cout << "Memory mapped file name: " << argv[0] << "\n";

	// Open memory mapped file
	std::wstringstream stream; stream << argv[0];
	HANDLE m_memoryMappedFile = CreateFileMappingW( INVALID_HANDLE_VALUE, 
		NULL, PAGE_READWRITE, 0, sizeof(GameData), stream.str( ).c_str( ) );
	
	// Verify success of file mapping 
	if( !m_memoryMappedFile ) { std::cout << "Failed to open memory mapped file: "
										  << argv[0]; system("pause"); exit(1); }
	else std::cout << "Sucessfully opened memory mapped file.\n";

	// Create memory mapped view
	GameData* gameData = (GameData*)MapViewOfFile( m_memoryMappedFile, 
			FILE_MAP_ALL_ACCESS, 0, 0, sizeof(GameData) );

	// Verify success of view mapping
	if( !gameData ) { std::cout << "Failed to map view of file"; system("pause"); exit(1); }
	else std::cout << "Sucessfully mapped view of file.\n";

	// Startup agent :TODO: Add boolean return to startup for valid match conditions
	player.startup( gameData->boardSize, gameData->startTile, gameData->nPlayers );

	// Main program loop
	std::vector<Move> moves;
	while( !gameData->matchOver ) {
		if( gameData->turnReady ) {
			gameData->move = player.makeMove( gameData->board, gameData->pieces, 
				gameData->score, gameData->player, gameData->ply, gameData->moveHistory );
			gameData->moveReady = TRUE; gameData->turnReady = FALSE; } }

	// Shutdown ai player
	player.shutdown( );
}