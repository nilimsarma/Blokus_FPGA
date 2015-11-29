/* ===========================================================================

	Project: Beam AI player for Blokus

	Description:
	  Simulates a move in a game state by updating all game state data used
	  by the minimax search algorithm. This includes board and piece state,
	  and the move list structure.

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

// Standard includes
#include "Includes.h"

// Include header
#include "MoveSimulator.h"

// Player liberty bit masks
const short plibs[2] = { (0x1<<EX_GRID_LBTY_UR)|(0x1<<EX_GRID_LBTY_UL)|
					     (0x1<<EX_GRID_LBTY_LL)|(0x1<<EX_GRID_LBTY_LR),
					     (0x2<<EX_GRID_LBTY_UR)|(0x2<<EX_GRID_LBTY_UL)|
					     (0x2<<EX_GRID_LBTY_LL)|(0x2<<EX_GRID_LBTY_LR) };
const short libs = (0x3<<EX_GRID_LBTY_UR)|(0x3<<EX_GRID_LBTY_UL)|
				   (0x3<<EX_GRID_LBTY_LL)|(0x3<<EX_GRID_LBTY_LR);

// --------------------------------------------------------
//  ReformatBoard - Converts the format of the board from 
//  a single byte cover map to a 4 byte cover, adjacent, 
//  liberty_1-4, leak map used for speeding up the board
//	reasoning and move enumeration algorithms. Also 
//  reformats piece arrays into a single 4byte field.
// --------------------------------------------------------
void MoveSimulator::reformatBoard( char boardIn[][20], short boardOut[][14],
	bool piecesIn[][21], int piecesOut[], int startTile[][2] )
{
	// Begin profiling liberty creation
	__int64 reformatTimeID = Profiler::startProfile( );

	// Zero piece memory
	piecesOut[PLAYER_BLUE] = 0;
	piecesOut[PLAYER_RED]  = 0;

	// Zero board memory
	for( int i = 0; i < BOARD_SIZE; i++ )
	for( int j = 0; j < BOARD_SIZE; j++ )
		boardOut[i][j] = 0;

	// Pack pieces array
	for( int p = 0; p < NUM_PLAYERS; p++ )
	for( int i = 0; i < PIECE_COUNT; i++ )
		piecesOut[p] |= (piecesIn[p][i] << i);

	// Convert board to extended format
	for( int i = 0; i < BOARD_SIZE; i++ )
	for( int j = 0; j < BOARD_SIZE; j++ )
	{
		// Check if grid square is covered
		if( boardIn[i][j] != GRID_COVER_NONE )
		{
			// Mark tile as unsafe for all
			boardOut[i][j] = (short)boardIn[i][j]+1;
			boardOut[i][j] |= (0x3<<EX_GRID_NOT_SAFE);
		}

		// Check for liberties and safeties
		else  
		{ 
			// Add a backup starting liberty
			for( int k = 0; k < NUM_PLAYERS; k++ )
			if( i == startTile[k][0] && j == startTile[k][1] ) {
				if( k == 0 ) boardOut[i][j] = (k+1)<<EX_GRID_LBTY_LR;
				if( k == 1 ) boardOut[i][j] = (k+1)<<EX_GRID_LBTY_UL;
				continue; }

			// Check for adjacent covered tile
			if( j<BOARD_SIZE-1 && boardIn[i][j+1]!=GRID_COVER_NONE )
				boardOut[i][j] |= ((boardIn[i][j+1]+1)<<EX_GRID_NOT_SAFE);
			if( i<BOARD_SIZE-1 && boardIn[i+1][j]!=GRID_COVER_NONE )
				boardOut[i][j] |= ((boardIn[i+1][j]+1)<<EX_GRID_NOT_SAFE);
			if( j>0 && boardIn[i][j-1]!=GRID_COVER_NONE )
				boardOut[i][j] |= ((boardIn[i][j-1]+1)<<EX_GRID_NOT_SAFE);
			if( i>0 && boardIn[i-1][j]!=GRID_COVER_NONE ) 
				boardOut[i][j] |= ((boardIn[i-1][j]+1)<<EX_GRID_NOT_SAFE);
			
			// Check if this tile is a liberty for any players
			if( (i>0 && j<BOARD_SIZE-1 && boardIn[i-1][j+1]!=GRID_COVER_NONE) && 
				!((boardOut[i][j])&((boardIn[i-1][j+1]+1)<<EX_GRID_NOT_SAFE)) ) 
				boardOut[i][j] |= ((boardIn[i-1][j+1]+1) << EX_GRID_LBTY_UR);
			if( (i<BOARD_SIZE-1 && j<BOARD_SIZE-1 && boardIn[i+1][j+1]!=GRID_COVER_NONE) && 
				!((boardOut[i][j])&((boardIn[i+1][j+1]+1)<<EX_GRID_NOT_SAFE)) ) 
				boardOut[i][j] |= ((boardIn[i+1][j+1]+1) << EX_GRID_LBTY_UL);
			if( (i<BOARD_SIZE-1 && j>0 && boardIn[i+1][j-1]!=GRID_COVER_NONE) && 
				!((boardOut[i][j])&((boardIn[i+1][j-1]+1)<<EX_GRID_NOT_SAFE)) ) 
				boardOut[i][j] |= ((boardIn[i+1][j-1]+1) << EX_GRID_LBTY_LL);
			if( (i>0 && j>0 && boardIn[i-1][j-1]!=GRID_COVER_NONE) && 
				!((boardOut[i][j])&((boardIn[i-1][j-1]+1)<<EX_GRID_NOT_SAFE)) ) 
				boardOut[i][j] |= ((boardIn[i-1][j-1]+1) << EX_GRID_LBTY_LR);
		}
	}

	// Stop profiling make time
	Profiler::endProfile( tReformatBoard, reformatTimeID );
}
//
// --------------------------------------------------------
//  SimulateMove - Simulates a move on the input board and
//  stores the resulting state in the output variables.
// --------------------------------------------------------
void MoveSimulator::simulateMove( const Move* move, 
		short grid[][14], int pieces[], int score[], int player,
		short gridOut[][14], int piecesOut[], int scoreOut[], int* playerOut,
		MoveLists* movelists, MoveLists* movelistsOut )
{
	// Copy board data to output
	for( int i = 0; i < BOARD_SIZE; i++ )
	for( int j = 0; j < BOARD_SIZE; j++ )
		gridOut[i][j] = grid[i][j];

	// Copy piece data to output
	piecesOut[PLAYER_BLUE] = pieces[PLAYER_BLUE];
	piecesOut[PLAYER_RED]  = pieces[PLAYER_RED];

	// Update piece registry
	piecesOut[player] &= ~(1<<move->pieceNumber);

	// Copy piece data to output array
	for( int i = 0; i < NUM_PLAYERS; i++ ) 
		scoreOut[i] = score[i];

	// Update player score variable
		 if( move->pieceNumber > 8 ) scoreOut[player] += 5;
	else if( move->pieceNumber > 3 ) scoreOut[player] += 4;
	else if( move->pieceNumber > 1 ) scoreOut[player] += 3;
	else if( move->pieceNumber > 0 ) scoreOut[player] += 2;
	else scoreOut[player] += 1;

	// Get piece object handle
	Piece* piece = PieceSet::getPiece( move->pieceNumber );

	// Cache piece dimensions
	int x = piece->getSizeX( );
	int y = piece->getSizeY( );

	// Get the player mask bit
	int playerBit = (1 << player);

	// Buffers for new liberties
	GridLiberty newLiberties[8];
	int nNewLiberties = 0; 

	// Copy move lists structure before simulation
	movelistsOut->copy( movelists );

	// Apply the piece pattern to the grid
	if( move->flipped == PIECE_UNFLIPPED )
	{
		if( move->rotated == PIECE_ROTATE_0 ) {
			for( int j = 0, gy = move->gridY; j < y; j++,gy++ )
			for( int i = 0, gx = move->gridX; i < x; i++,gx++ ) 
				applyPiecePattern( piece, movelistsOut, gridOut, player, playerBit, i, j, gx, gy,
					nNewLiberties, newLiberties ); }

		else if( move->rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move->gridY; i >= 0; i--,gy++ )
			for( int j =   0, gx = move->gridX; j <  y; j++,gx++ )
				applyPiecePattern( piece, movelistsOut, gridOut, player, playerBit, i, j, gx, gy,
					nNewLiberties, newLiberties ); }

		else if( move->rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move->gridY; j >= 0; j--,gy++ )
			for( int i = x-1, gx = move->gridX; i >= 0; i--,gx++ )
				applyPiecePattern( piece, movelistsOut, gridOut, player, playerBit, i, j, gx, gy,
					nNewLiberties, newLiberties ); }

		else if( move->rotated == PIECE_ROTATE_270 ) {
			for( int i =   0, gy = move->gridY; i <  x; i++,gy++ )
			for( int j = y-1, gx = move->gridX; j >= 0; j--,gx++ )
				applyPiecePattern( piece, movelistsOut, gridOut, player, playerBit, i, j, gx, gy,
					nNewLiberties, newLiberties ); }

	} else 
	{
		if( move->rotated == PIECE_ROTATE_0 ) {
			for( int j =   0, gy = move->gridY; j <  y; j++,gy++ )
			for( int i = x-1, gx = move->gridX; i >= 0; i--,gx++ )
				applyPiecePattern( piece, movelistsOut, gridOut, player, playerBit, i, j, gx, gy,
					nNewLiberties, newLiberties ); }

		else if( move->rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move->gridY; i >= 0; i--,gy++ )
			for( int j = y-1, gx = move->gridX; j >= 0; j--,gx++ )
				applyPiecePattern( piece, movelistsOut, gridOut, player, playerBit, i, j, gx, gy,
					nNewLiberties, newLiberties ); }

		else if( move->rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move->gridY; j >= 0; j--,gy++ )
			for( int i =   0, gx = move->gridX; i <  x; i++,gx++ )
				applyPiecePattern( piece, movelistsOut, gridOut, player, playerBit, i, j, gx, gy,
					nNewLiberties, newLiberties ); }

		else if( move->rotated == PIECE_ROTATE_270 ) {
			for( int i = 0, gy = move->gridY; i < x; i++,gy++ )
			for( int j = 0, gx = move->gridX; j < y; j++,gx++ )
				applyPiecePattern( piece, movelistsOut, gridOut, player, playerBit, i, j, gx, gy,
					nNewLiberties, newLiberties ); }
	}

	// Update any affect liberty move lists
	movelistsOut->updateLiberties( gridOut, pieces, PLAYER_MAX );
	movelistsOut->updateLiberties( gridOut, pieces, PLAYER_MIN );

	// Begin profiling liberty creation
	__int64 makeLibsTimeID = Profiler::startProfile( );

	// Compute liberty angle from orientation
	for( int i = 0; i < nNewLiberties; i++ ) 
		newLiberties[i].angle += move->rotated;
	if( move->flipped )
		for( int i = 0; i < nNewLiberties; i++ ) {
			newLiberties[i].angle -= (newLiberties[i].angle%2)*2;
			newLiberties[i].angle++; }
	for( int i = 0; i < nNewLiberties; i++ )
		newLiberties[i].angle %= 4;

	// Create any new liberties on the board
	for( int i = 0; i < nNewLiberties; i++ )
		movelistsOut->makeLiberty( newLiberties[i].x, newLiberties[i].y, 
			newLiberties[i].angle, player, gridOut, piecesOut );

	// Stop profiling make time
	Profiler::endProfile( tMakeLibs, makeLibsTimeID );

	// Fighting liberty detection
	movelistsOut->detectFightingLiberties( );

	// Switch player to move
	*playerOut = 1 - player;
}
//
// --------------------------------------------------------
//  ApplyPiecePattern - Applies a piece pattern to the grid 
//  at the specified piece and grid coordinates.
// --------------------------------------------------------
void MoveSimulator::applyPiecePattern( Piece* piece, MoveLists* moveLists,
	short gridOut[][14], int player, int playerBit, int i, int j, int gx, int gy,
	int& nNewLiberties, GridLiberty newLiberties[] )
{
	// Get piece layout
	short pattern = piece->getLayout( i, j );

	// Ignore off grid tiles
	if( gx < 0 || gx >= BOARD_SIZE || gy < 0 || gy >= BOARD_SIZE ) return; 

	// Mark tiles adjacent to covered tiles as unsafe for player
	if( pattern == EX_MATCH_NOT_PLAYERS ) 
	{
		// Mark grid unsafe for player
		moveLists->markUnsafeTile( gx, gy, player );
		gridOut[gx][gy] |= ( playerBit<<EX_GRID_NOT_SAFE ); 
	}

	// Convert newly covered tiles to unsafe for all players
	else if( pattern == EX_MATCH_NOT_COVERED )
	{
		// Mark unsafe tiles in move lists 
		moveLists->markUnsafeTile( gx, gy, PLAYER_MAX );
		moveLists->markUnsafeTile( gx, gy, PLAYER_MIN );

		// Cover the underlying grid square with unsafe marks
		gridOut[gx][gy] = (playerBit | (0x3<<EX_GRID_NOT_SAFE));
	}

	// Mark newly found liberties if the location is safe
	else if( pattern >= EX_MATCH_LBTY_UR && pattern <= EX_MATCH_LBTY_LR )
	{
		// Add the new liberty to the buffer
		if( !(gridOut[gx][gy] & (playerBit<<EX_GRID_NOT_SAFE)) )
		if( !(gridOut[gx][gy] & plibs[player]) )
		{
			newLiberties[nNewLiberties].x = gx;
			newLiberties[nNewLiberties].y = gy;
			newLiberties[nNewLiberties].angle = pattern - EX_MATCH_LBTY_UR;
			nNewLiberties++;
		}
	}
}