/* ===========================================================================

	Project: Minimax AI player for Blokus

	Description:
	 Standardizes common header files used by several source files.

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
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include <string>
#include <limits>
#include <math.h>

// Debug macros
#include "Debug.h"

// Type definitions
#include "Types.h"
#include "TypesEx.h"

// High-res Timer
#include "Timer.h"

// Opening book
#include "OpeningBook.h"

// Heuristic functions
#include "Heuristic.h"

// Include header
#include "Minimax.h"

// End def
#endif