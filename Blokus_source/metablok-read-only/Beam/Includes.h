/* ===========================================================================

	Project: Beam AI player for Blokus

	Description:
	 Localized set of standard include files.

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

// Begin definitions
#ifndef INCLUDES_H
#define INCLUDES_H

// Windows headers
#include <windows.h>
#include <process.h>

// C++ Standard library 
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include <string>
#include <limits>
#include <bitset>

// Debug macros
#include "Debug.h"

// Type definitions
#include "Types.h"
#include "TypesEx.h"
#include "Piece.h"

// High-res Timer
#include "Timer.h"

// Profiler
#include "Profiler.h"

// Move enumeration 
#include "MemoryPool.h"
#include "MoveLists.h"

// Move simulation
#include "MoveSimulator.h"

// Opening book
#include "OpeningBook.h"

// Heuristic functions
#include "InfluenceMap.h"
#include "Heuristic.h"

// Include header
#include "Minimax.h"

// End def
#endif