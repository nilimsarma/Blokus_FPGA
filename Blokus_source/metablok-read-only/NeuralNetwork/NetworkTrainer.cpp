/* ============================================================================

    Project: Blokus AI - Neural Network Trainer
    
    Provides a set of functions to load saved game files with evaluation
    scores and train the neural network with those values.
    
    File: NetworkTrainer.cpp
    Author: David Gloe
    Date: December 02, 2011
    
============================================================================= */

// Local includes
#include "NetworkTrainer.h"

// Standard library includes
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

// ------------------------------------------------------
// Creates a new neural network with the given inputs
// ------------------------------------------------------
NetworkTrainer::NetworkTrainer(double learningRate, double momentum, 
    bool linearOutput) : m_numberOfPlayers(2), m_currentPly(0), 
    m_currentPlayer(0)
{
	// Load game piece layouts 
	m_gamePieceLayouts = PieceSet::instance( );
	
	// Initialize neural network
	m_network.SetLearningRate(learningRate);
	if(momentum > 0.0) {
	    m_network.SetMomentum(true, momentum);
	}
	m_network.SetLinearOutput(linearOutput);
	
    m_network.Initialize(204, 10, 2);
}

// -------------------------------------------------------
// Creates a neural network from the given filename for
// further training.
// -------------------------------------------------------
NetworkTrainer::NetworkTrainer(const char *filename) : m_numberOfPlayers(2),
    m_currentPly(0), m_currentPlayer(0) {
    m_gamePieceLayouts = PieceSet::instance();
    if (!m_network.LoadData(filename)) {
        throw 1;
    }
}

// -------------------------------------------------------
// Destructor
// -------------------------------------------------------
NetworkTrainer::~NetworkTrainer() {
    m_network.Cleanup();
}

// --------------------------------------------------------
//   Loads the game from disk with the specified filename.
//	 If the load is unsuccessful or corrupt the function
//	 will return zero.
// --------------------------------------------------------
bool NetworkTrainer::train(const char* filename)
{
	if (!loadFile(filename)) {
	    return false;
    }
	
	// Inputs: one for each board square (196), 
    // one for packed piece array (6), one for current player (1),
    // one for each score (2)
	
	// Set board neuron inputs
	for(int i = 0; i < 14; i++) {
	    for(int j = 0; j < 14; j++) {
	        m_network.SetInput(i+j, m_board[i][j]);
	    }
	}
	
	// Pack pieces array
	int piecesOut[2][3];
	for( int p = 0; p < 2; p++ ) {
	    for( int i = 0; i < 3; i++ ) {
	        piecesOut[p][i] = 0;
	        for( int j = 0; j < 8; j++ ) {
	            if( 8*i+j >= 21 ) {
	                continue; 
	            } else { 
	                piecesOut[p][i] |= (m_pieces[p][8*i+j] << j); 
	            }
	        }
	    }
	}
	
	// Set remaining piece inputs
	for(int i = 0; i < 3; i++) {
	    m_network.SetInput(196+2*i, piecesOut[0][i]);
	    m_network.SetInput(196+2*i+1, piecesOut[1][i]);
	}
	
	// And misc other inputs
	m_network.SetInput(201, m_currentPlayer);
	m_network.SetInput(202, m_scores[0]);
	m_network.SetInput(203, m_scores[1]);
	
	// Set desired neuron outputs using this function
	m_network.SetDesiredOutput(0, m_score/100.0);
	m_network.SetDesiredOutput(1, 1.0 - m_score/100.0);
	
	// And back propogate to adjust weights 
	m_network.BackPropogate();

	// Return OK
	return true;
}

// ---------------------------------------------------------------------
// Reads the saved game file given by the file, and obtains the 
// neural network output. Returns a pair of floats representing the
// neural network output and the score given in the file.
// ---------------------------------------------------------------------
VerifyResult NetworkTrainer::verify(const char *filename) {
    VerifyResult result;
    result.networkOutput1 = 0.0;
    result.networkOutput2 = 0.0;
    result.desiredOutput = 0.0;
    
    if (!loadFile(filename)) {
	    return result;
    }
	
	// Inputs: one for each board square (196), 
    // one for each remaining piece (6), one for current player (1),
    // one for each score (2)
	
	// Set board neuron inputs
	for(int i = 0; i < 14; i++) {
	    for(int j = 0; j < 14; j++) {
	        m_network.SetInput(i+j, m_board[i][j]);
	    }
	}
	
    // Pack pieces array
	int piecesOut[2][3];
	for( int p = 0; p < 2; p++ ) {
	    for( int i = 0; i < 3; i++ ) {
	        piecesOut[p][i] = 0;
	        for( int j = 0; j < 8; j++ ) {
	            if( 8*i+j >= 21 ) {
	                continue; 
	            } else { 
	                piecesOut[p][i] |= (m_pieces[p][8*i+j] << j); 
	            }
	        }
	    }
	}
	
	// Set remaining piece inputs
	for(int i = 0; i < 21; i++) {
	    m_network.SetInput(196+2*i, m_pieces[0][i]);
	    m_network.SetInput(196+2*i+1, m_pieces[1][i]);
	}
	
	// And misc other inputs
	m_network.SetInput(201, m_currentPlayer);
	m_network.SetInput(202, m_scores[0]);
	m_network.SetInput(203, m_scores[1]);
	
	// And calculate the output
	m_network.FeedForward();
	
	// Set the result output
	result.networkOutput1 = m_network.GetOutput(0);
	result.networkOutput2 = m_network.GetOutput(1);
	result.desiredOutput = m_score/100.0;

	return result;
}

// --------------------------------------------------------
// Saves the neural network data to the given filename
// --------------------------------------------------------
void NetworkTrainer::save(const char *filename) {
    m_network.DumpData(filename);
}

// --------------------------------------------------------
// Loads the given save file and sets our internal vars
// --------------------------------------------------------
bool NetworkTrainer::loadFile(const char *filename) {
    // Open the specified file for reading
	std::ifstream file(filename);

	// Check for failure
	if(!file.good()) {
	    return false;
	}
	
	// Initialize values
	memset(m_board, 0, sizeof(m_board));
	m_score = 50;

	// Parse file using newline and space delimiters 
	std::string line; while( std::getline( file, line ) )
	{		
		// Put line data onto stream for delimination
		std::string token; std::stringstream iss; iss << line;

		// Parse leading token on line
		while( std::getline( iss, token, ' ' ) )
		{
			// Ignore line comments
			if( token.substr( 0, 2 ).compare("//") == 0 ) break;

			// Read simple state and settings data
			else if( token.compare("Number_Of_Players") == 0 ) ;
			else if( token.compare("Size_Of_Board") == 0 ) ;
			else if( token.compare("Wait_Turn") == 0 ) ;
			else if( token.compare("Wait_Game") == 0 ) ;
			else if( token.compare("Score") == 0) {
			    iss >> m_score;
			}
			else if( token.compare("Start_Tile") == 0) ;

			// Read player pieces
			else if( token.compare("Pieces") == 0)
			{
				// Get the player number
				int playerNumber; iss >> playerNumber;

				// Read the players piece bitstring
				std::string bitStr; iss >> bitStr;
				if( bitStr.size( ) != 21 ) return false;
				for( int i = 0; i < bitStr.size( ); i++ )
					if( bitStr.at( i ) == '0' ) 
						m_pieces[playerNumber][i] = false;
					else m_pieces[playerNumber][i] = true;
			}

			// Read in move 
			else if( token.compare("Move") == 0 ) 
			{
				Move move; 

				// Get the move definition
				iss >> move.pieceNumber;
				iss >> move.gridX;
				iss >> move.gridY;
				iss >> move.rotated;
				iss >> move.flipped;

				// Make the move
				makeMove( move );
			}

			// Unexpected token (ignore)
			/*
			else {
			    return false;
			}
			*/
		}
	}

	// Close file
	file.close();
	return true;
}

// --------------------------------------------------------
//	Updates the game data to reflect the execution of the
//  valid input move.
// --------------------------------------------------------
void NetworkTrainer::makeMove( Move move )
{
    // Add the move to the move history
	m_moveHistory[m_currentPly] = move;
	m_currentPly++;
	
	// Update piece registry
	m_pieces[m_currentPlayer][move.pieceNumber] = false;

	// Iterate over piece pattern and update game board
	int gx = move.gridX, gy = move.gridY;
	int x = m_gamePieceLayouts->getSizeX( move.pieceNumber );
	int y = m_gamePieceLayouts->getSizeY( move.pieceNumber );
	if( move.flipped == PIECE_UNFLIPPED )
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j = 0, gy = move.gridY; j < y; j++,gy++ )
			for( int i = 0, gx = move.gridX; i < x; i++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j =   0, gx = move.gridX; j <  y; j++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i =   0, gy = move.gridY; i <  x; i++,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }
	} else {
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j =   0, gy = move.gridY; j <  y; j++,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i =   0, gx = move.gridX; i <  x; i++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i = 0, gy = move.gridY; i < x; i++,gy++ )
			for( int j = 0, gx = move.gridX; j < y; j++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }
	}

	// Update player score variable
	if     ( move.pieceNumber < 1 ) m_scores[m_currentPlayer] += 1;
	else if( move.pieceNumber < 2 ) m_scores[m_currentPlayer] += 2;
	else if( move.pieceNumber < 4 ) m_scores[m_currentPlayer] += 3;
	else if( move.pieceNumber < 9 ) m_scores[m_currentPlayer] += 4;
	else m_scores[m_currentPlayer] += 5;

	// Update current player variable
	int prevPlayer = m_currentPlayer;
	m_currentPlayer++; m_currentPlayer %= m_numberOfPlayers;
	
	// Skip turns for players unable to move
	// NOTE: Not supported yet! Don't use positions with skip moves or
	// end of game positions
	/*
	while( !isMoveAvailable( ) && prevPlayer != m_currentPlayer)
	{
		// Move to next player
		m_currentPlayer++; m_currentPlayer %= m_numberOfPlayers;

		// Put a skip move on the history
		Move skip; skip.pieceNumber = -1;
		skip.flipped = skip.gridX = skip.gridY = skip.rotated = 0;
		m_moveHistory[m_currentPly] = skip;
		m_currentPly++;
	}

	// Check for game over condition (if no one can move)
	if( prevPlayer == m_currentPlayer && !isMoveAvailable( ) ) 
	{
		// Remove extra skips
		for( int i = 0; i < m_numberOfPlayers; i++ ) 
			m_currentPly--;
	}
	*/
}

