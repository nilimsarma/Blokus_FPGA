/* ===========================================================================

	Project: AI player for Blokus

	Description:
	 Implements a basic system timer for use by AI players. Code taken from
	 or based on that available from Chad Vernon's tutorials viewable at 
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

// Begin definition
#ifndef TIMER_H
#define TIMER_H

// Timer object
class Timer
{

public:
    Timer( );
    void start( );
    void stop( );
    void update( ); 

    bool  isStopped( ) { return m_stopped; }
    float getFPS( )	{ return m_FPS; }
    float getRunningTime( ) { return m_runningTime; }
    float getElapsedTime( ) { return m_stopped ? 0.0f : m_timeElapsed; } 

private:
    __int64 m_ticksPerSecond;
    __int64 m_currentTime;
    __int64 m_lastTime;
    __int64 m_lastFPSUpdate;
    __int64 m_FPSUpdateInterval;
    unsigned int m_numFrames;
    float m_runningTime;
    float m_timeElapsed;
    float m_FPS;
    bool m_stopped;
};

// End definition
#endif
