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

// Begin definition
#ifndef OPENING_BOOK_H
#define OPENING_BOOK_H

// C++ standard library
#include <vector>

// Type definitions
#include "Types.h"

// Opening book class
class OpeningBook
{

public:
	// Default constructor to appease the Visual C++ gods
	OpeningBook() { }

    // Opens a book file
    void openBook(const char *);
    
    // Returns a recommended move from the opening book
    // Prerequisite: isInBook returns true
    // Throws an error message on error.
    Move makeMove(std::vector<Move>&);
    
    // Determines whether the given position is in the opening book
    bool isInBook(std::vector<Move>&);

	// Prints the book to the given outstream depth-first
	void printBook(std::ostream&);
    
private:
    struct MoveProb {
        float prob;                     // Probability of making this move
        Move move;                      // The move to make
        std::vector<MoveProb> nextmoves;// Subsequent possible moves
    };

    MoveProb m_book;    // Root of opening book tree
    
    void parseLine(const char *, std::vector<MoveProb*>&);
	void printBook(std::ostream&, MoveProb&, int);
	const char *pieceNumToName(int pieceNumber);
};

#endif
