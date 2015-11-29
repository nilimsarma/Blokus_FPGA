/* ===========================================================================

	Project: DirectX Engine - Timer

	Description:
	 Handles timing for animation and synchronization with the DirectX Engine
	 render function. Code taken from Chad Vernon's tutorials at 
	 http://www.chadvernon.com.

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
#ifndef DX_ENGINE_TIMER_H
#define DX_ENGINE_TIMER_H

// DirectX namespace
namespace DirectX
{

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

}

// End definition
#endif
