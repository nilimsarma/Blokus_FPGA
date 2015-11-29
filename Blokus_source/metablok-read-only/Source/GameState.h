/* ===========================================================================

	Project: Game Engine - GameState

	Description:
	 Abstract base class for application states.

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
#ifndef DX_ENGINE_GAMESTATE_H
#define DX_ENGINE_GAMESTATE_H

// Colonization namespace
namespace DirectX
{
	// GameState action states (For engine manager)
	enum State { WAITING, RUNNING, PAUSED, COMPLETED };

	// Game engine game state class
	class GameState : public DirectX::Controller
	{
	public:
		// Constructor - initialize wait state
		GameState( ) : m_state(WAITING) { }

		// State functions
		virtual void startup( ) { }
		virtual void main( ) { }
		virtual void shutdown( ) { }

		// Pause functionality
		virtual void unpause( ) { m_state = RUNNING; }
		virtual void pause( ) { m_state = PAUSED; }

		// Returns substate
		int getState( ) { return m_state; }

	protected:
		// Game Engine GameState states (For inheritance)
		enum State { WAITING, RUNNING, PAUSED, COMPLETED };

		State m_state;
	};

}

// End definition
#endif