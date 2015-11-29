/* ===========================================================================

	Project: AI player for Blokus

	Description:
	 Localized set of functions for generating profile data for AI players.

    Copyright (C) 2011 Lucas Sherman

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

// Begin definition
#ifndef PROFILER_H
#define PROFILER_H

// Profiler activation
#define PROFILE TRUE

// Profiler segments
enum ProfilerFunction
{
	tTotal,
	tReformatBoard,
	tMinimaxSearch,
		tMoveGeneration,
		tMoveEnumeration,
		tSimulateMoves,
			tMoveValidation,
			tLeakDetection,
			tKillLibs,
			tMarkUnsafe,
			tUpdateLibs,
			tMakeLibs,
			tMakeTable,
			tCopyLibs,
			tFightDetection,
		tEvaluateBoard,
			tStandard,
			tEndGame,
	tMax
};

// Profiler data collection
class Profiler 
{
public:
	// Returns a profile time identifier for the thread
	__forceinline static __int64 startProfile( )
	{
		if( PROFILE ) { LARGE_INTEGER temp;
						QueryPerformanceCounter( &temp );
					    return temp.QuadPart; }
	}

	// Determines profile of the specified function using the threads time identifier
	__forceinline static void endProfile( ProfilerFunction func, __int64 identifier )
	{
		if( PROFILE ) { LARGE_INTEGER temp; QueryPerformanceCounter( &temp );
			m_timeCosts[func] += temp.QuadPart - identifier; }
	}

	// Increment the node search counters
	__forceinline static void addSearchNode( ) { if( PROFILE ) m_nodesSearched++; }
	__forceinline static void addLeafNode( ) { if( PROFILE ) m_leavesSearched++; }

	// Clear profiler data
	__forceinline static void clear( )
	{
		if( PROFILE ) {
			for( int i = 0; i < tMax; i++ ) m_timeCosts[i] = 0; 
			m_nodesSearched = 0; m_leavesSearched = 0; }
	}

	// Print profile data to std output
	__forceinline static void displayResults( float searchTime, int maxSearchDepth )
	{
		// Automatically recorded statistical data
		std::cout << "\n-- Move Selection Statistics --\n"; 
		std::cout << searchTime << "s at Ply " << maxSearchDepth << "\n";

		// Profiled data
		if( PROFILE ) {
		std::cout << "Searched Nodes: " << m_nodesSearched << "\n";
		std::cout << "Searched Leafs: " << m_leavesSearched << "\n";
		std::cout << "Total Time " << (int)(100.0*(double)m_timeCosts[tTotal] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "  - Reformat Board: " << (int)(100.0*(double)m_timeCosts[tReformatBoard] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "  - Minimax: " << (int)(100.0*(double)m_timeCosts[tMinimaxSearch] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "      - Move Enumeration: " << (int)(100.0*(double)m_timeCosts[tMoveEnumeration] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "      - Move Simulation: " << (int)(100.0*(double)m_timeCosts[tSimulateMoves] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "        - Move Validation(shared): " << (int)(100.0*(double)m_timeCosts[tMoveValidation] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "        - Leak Detection(shared): " << (int)(100.0*(double)m_timeCosts[tLeakDetection] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "        - Kill Liberties(shared): " << (int)(100.0*(double)m_timeCosts[tKillLibs] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "        - Make Liberty: " << (int)(100.0*(double)m_timeCosts[tMakeLibs] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "          - Get Path Costs: " << (int)(100.0*(double)m_timeCosts[tMakeTable] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "        - Update Liberties: " << (int)(100.0*(double)m_timeCosts[tUpdateLibs] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "        - Fight Detection: " << (int)(100.0*(double)m_timeCosts[tFightDetection] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "        - Copy Liberties: " << (int)(100.0*(double)m_timeCosts[tCopyLibs] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "      - Board Evaluation: " << (int)(100.0*(double)m_timeCosts[tEvaluateBoard] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "        - Standard: " << (int)(100.0*(double)m_timeCosts[tStandard]
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		std::cout << "        - End Game: " << (int)(100.0*(double)m_timeCosts[tEndGame] 
			/ (double)m_timeCosts[tTotal] + 0.5) << "%\n";
		}
	}

private:
	// Profiler data members
	static __int64 m_timeCosts[tMax];
	static unsigned int m_nodesSearched;
	static unsigned int m_leavesSearched;
};

// End def
#endif