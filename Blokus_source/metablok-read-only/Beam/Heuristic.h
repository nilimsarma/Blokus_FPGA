/* ===========================================================================

	Project: Beam AI player for Blokus

	Description:
	  Contains a series of evaluation functions for rating Blokus Duo board
	  positions in the extended board format.

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

// Include gaurds
#ifndef HEURISTIC_H
#define HEURISTIC_H

// Evaluation function format
typedef float (*EvalFunction) ( MoveLists* moves, 
	short grid[][14], int pieces[], int score[], int player );

// Heuristic namespace
namespace Heuristic {

	// Board evaluation functions
	extern const int nEvaluationFunctions;
	extern const EvalFunction evalFunction[];
	extern const char* evalFunctionName[];

// End namespace
} // Heuristic

// End definition
#endif