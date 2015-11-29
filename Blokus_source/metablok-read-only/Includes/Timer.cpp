/* ===========================================================================

	Project: AI player for Blokus

	Description:
	 Implements a basic system timer for use by AI players. Code taken from/
	 based on that available from Chad Vernon's tutorials at 
	 http://www.chadvernon.com.

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

// Windows headers
#include <windows.h>

// Header include
#include "Timer.h"

// Constructor
Timer::Timer( )
{ 
	LARGE_INTEGER temp;
	QueryPerformanceFrequency(&temp);
	m_ticksPerSecond = temp.QuadPart;

	m_currentTime = m_lastTime = m_lastFPSUpdate = 0;
	m_runningTime = m_timeElapsed = m_FPS = 0.0f;
	m_FPSUpdateInterval = m_ticksPerSecond >> 1;
	m_numFrames = 0; m_stopped = true;
}
//
// Starts the timer
void Timer::start( )
{
	LARGE_INTEGER temp;
	QueryPerformanceCounter( &temp );
	m_lastTime = temp.QuadPart;
	m_stopped = false;
}
//
// Stops the timer
void Timer::stop( )
{
	LARGE_INTEGER stopTime;
	QueryPerformanceCounter( &stopTime );
	m_runningTime += (float)(stopTime.QuadPart - m_lastTime) / (float)m_ticksPerSecond;
	m_stopped = true;
}
//
// Updates FPS and total runtime
void Timer::update( )
{
	// Get the current time
	LARGE_INTEGER temp;
	QueryPerformanceCounter( &temp );
	m_currentTime = temp.QuadPart;

	// Track elapsed time
	m_timeElapsed = (float)(m_currentTime - m_lastTime) / (float)m_ticksPerSecond;
	m_runningTime += m_timeElapsed;

	// Save current time
	m_lastTime = m_currentTime;
}
