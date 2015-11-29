/* ===========================================================================

	Project: AI player for Blokus

	Description:
	 Define some extended board formatting options used by the Minimax and
	 Beam AI players for more efficient move enumeration.

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
#include "Profiler.h"

// Profiler data members
__int64 Profiler::m_timeCosts[tMax];
unsigned int Profiler::m_nodesSearched;
unsigned int Profiler::m_leavesSearched;