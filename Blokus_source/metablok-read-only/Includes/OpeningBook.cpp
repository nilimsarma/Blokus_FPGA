/* ===========================================================================

	Project: AI player for Blokus

	Description:
      Loads an opening book file and responds to queries by providing one
	  of the available moves from the opening book if the given board state
	  is in the book.

      Openings taken from the "Duo to the Death" blog:
      http://duotothedeath.wordpress.com/category/openings/

    Copyright (C) 2011 David Gloe

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

// C++ standard library headers
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

// Local includes
#include "OpeningBook.h"

// -----------------------------------------------------------------------------
// Constructor
// Creates the opening book from the given file
// Throws an error message on error
// -----------------------------------------------------------------------------
void OpeningBook::openBook(const char *filename)
{
    std::ifstream bookfile;
    char line[255];
    std::vector<MoveProb*> movestack;

    // Initialize random number generator
    srand((unsigned)time(NULL));
    
    // Initialize book root
    m_book.prob = 1.0;
    
    // Open file
    bookfile.open(filename);
    if(!bookfile.is_open() || !bookfile.good()) {
        throw "Could not open book file";
    }
    
    // Read file
    movestack.push_back(&m_book);
    while(bookfile.good()) {
        bookfile.getline(line, sizeof(line));
        parseLine(line, movestack);
    }
    
    // Close file
    bookfile.close();
}

// -----------------------------------------------------------------------------
// makeMove
// Returns a move from the opening book
// Throws an error message on error
// -----------------------------------------------------------------------------
Move OpeningBook::makeMove(std::vector<Move> &movelist)
{
    std::vector<Move>::iterator mlit;
    std::vector<MoveProb>::iterator chit;
    MoveProb *curMove = &m_book;
    bool found;
    float prob;
    
    // Move through the move list, traversing the tree
    for(mlit = movelist.begin(); mlit != movelist.end(); mlit++) {
    
        // Find child which corresponds to the current move
        found = false;
        for(chit = curMove->nextmoves.begin(); 
            chit != curMove->nextmoves.end(); chit++) {
            if(*mlit == chit->move) {
                curMove = &*chit;
                found = true;
                break;
            }
        }
        
        // If we didn't find a move, we are not in book
        // This is an error because they should have checked that it was in book
        // already.
        if(!found) {
            throw "Move not found in opening book!";
        }
    }
    
    if(curMove->nextmoves.size() == 0) {
        throw "Move has no following moves in opening book!";
    }
    
    // Now the children of the current move are the possible next moves
    prob = (float)rand() / (float)RAND_MAX;
    for(chit = curMove->nextmoves.begin(); 
        chit <= curMove->nextmoves.end(); chit++) {
        prob -= chit->prob;
        // If prob is lower than 0, choose this move
        if(prob <= 0.0) {
            return chit->move;
        }
    }
    
    // If we have reached here, total probability of child moves is < 1
    throw "Opening book error, total probability of next moves less than 1.0";
}

// -----------------------------------------------------------------------------
// isInBook
// Returns true if the given move is in the opening book
// -----------------------------------------------------------------------------
bool OpeningBook::isInBook(std::vector<Move> &movelist)
{    
    std::vector<Move>::iterator mlit;
    std::vector<MoveProb>::iterator chit;
    MoveProb *curMove = &m_book;
    bool found;
    
    // Move through the move list, traversing the tree
    for(mlit = movelist.begin(); mlit != movelist.end(); mlit++) {
    
        // Find child which corresponds to the current move
        found = false;
        for(chit = curMove->nextmoves.begin(); 
            chit != curMove->nextmoves.end(); chit++) {
            if(*mlit == chit->move) {
                curMove = &*chit;
                found = true;
                break;
            }
        }
        
        // If we didn't find a move, we are not in book
        if(!found) {
            return false;
        }
    }
    
    // This can't be a leaf node
    if(curMove->nextmoves.size() == 0) {
        return false;
    }
    
    return true;
}

// -----------------------------------------------------------------------------
// Parse a line from the input file
// -----------------------------------------------------------------------------
void OpeningBook::parseLine(const char *line, std::vector<MoveProb*> &movestack)
{
    std::stringstream linestream;
    MoveProb lineMove;
    int movePly;
    const char *lineptr;

    // Determine whether this line is a comment
    for(lineptr = line; isspace(*lineptr) && *lineptr != '\0'; lineptr++);
    if(!isdigit(*lineptr) || *lineptr == '\0') {
        return;
    }    
    
    // Read info from the line
    linestream << line;
    linestream >> movePly >> lineMove.prob >> lineMove.move.pieceNumber
        >> lineMove.move.gridX >> lineMove.move.gridY 
        >> lineMove.move.flipped >> lineMove.move.rotated;
        
    // Make sure info is valid
    if(linestream.bad()) {
        throw "Error parsing line";
    } else if(movePly < 1 || movePly > movestack.size()) {
        throw "Move ply must be between 1 and (current ply + 1)";
    } else if(lineMove.prob < 0.0 || lineMove.prob > 1.0) {
        throw "Move probability must be between 0 and 1";
    } else if(lineMove.move.pieceNumber < 0 || lineMove.move.pieceNumber > 20) {
        throw "Piece number must be between 0 and 20";
    } else if(lineMove.move.gridX < 0 || lineMove.move.gridX > 19) {
        throw "Grid X value must be between 0 and 19";
    } else if(lineMove.move.gridY < 0 || lineMove.move.gridY > 19) {
        throw "Grid Y value must be between 0 and 19";
    } else if(lineMove.move.flipped != 0 && lineMove.move.flipped != 1) {
        throw "Flipped value must be 0 or 1";
    } else if(lineMove.move.rotated < 0 || lineMove.move.rotated > 3) {
        throw "Rotated valuemust be between 0 and 3";
    }
    
    // Pop from stack until stack ply is less than move ply
    while(movestack.size() > movePly) {
        movestack.pop_back();
    }
    
    // Insert move as a child of last move in the stack
    movestack.back()->nextmoves.push_back(lineMove);

	// Insert move into move stack
	movestack.push_back(&movestack.back()->nextmoves.back());
}

// -----------------------------------------------------------------------------
// Prints the opening book to the given output stream
// -----------------------------------------------------------------------------
void OpeningBook::printBook(std::ostream& out) {
	// Print header
	out << std::setw(10) << "Ply" << std::setw(10) << "Prob" 
		<< std::setw(6) << "Piece" << std::setw(3) << "X" 
		<< std::setw(3) << "Y" << std::setw(8) << "Flipped"
		<< std::setw(8) << "Rotated" << std::endl;
	out << std::setprecision(6);
	printBook(out, m_book, 0);	
}

// -----------------------------------------------------------------------------
// Prints the opening book subtree to the given output stream
// -----------------------------------------------------------------------------
void OpeningBook::printBook(std::ostream& out, MoveProb& curMove, int ply) {
    std::vector<MoveProb>::iterator chit;

	// Print us
	for(int i = 0; i < 10; i++) { 
		if(i < ply) out << "-";
		else out << " "; 
	}
	out << std::setw(10) << std::fixed << curMove.prob 
		<< std::setw(6) << pieceNumToName(curMove.move.pieceNumber)
		<< std::setw(3) << curMove.move.gridX 
		<< std::setw(3) << curMove.move.gridY
		<< std::setw(8) << curMove.move.flipped
		<< std::setw(8) << curMove.move.rotated
		<< std::endl;
    
    // Print all child nodes
    for(chit = curMove.nextmoves.begin(); 
        chit != curMove.nextmoves.end(); chit++) {
        printBook(out, *chit, ply + 1);
    }

	return;
}

// -----------------------------------------------------------------------------
// Converts the piece number to a two char human-readable piece name
// -----------------------------------------------------------------------------
const char* OpeningBook::pieceNumToName(int pieceNumber) {
	switch(pieceNumber) {
		case 0:		return " 1";
		case 1:		return " 2";
		case 2:		return "I3";
		case 3:		return "V3";
		case 4:		return "T4";
		case 5:		return "Z4";
		case 6:		return "I4";
		case 7:		return "L4";
		case 8:		return " O";
		case 9:		return " P";
		case 10:	return " U";
		case 11:	return " F";
		case 12:	return "I5";
		case 13:	return "L5";
		case 14:	return "V5";
		case 15:	return " N";
		case 16:	return " W";
		case 17:	return "Z5";
		case 18:	return " Y";
		case 19:	return " T";
		case 20:	return " X";
		default: break;
	}
	return "00";
}
