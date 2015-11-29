/* ===========================================================================

	Project: Game Engine - Input Manager

	Description: Implements controller input functionality using the Windows 
	 XInput API:

	http://msdn.microsoft.com/en-us/library/windows/desktop/hh405053(v=VS.85).aspx

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
#ifndef DX_ENGINE_INPUT_MANAGER_H
#define DX_ENGINE_INPUT_MANAGER_H

// Engine namespace
namespace DirectX
{

// Startup error codes
enum InputInitError 
{

	IIE_NONE = 0 
};

// Management class
class InputManager
{
public:
	// Destructor
	~InputManager( ) { shutdown( ); }

	// Startup error output
	InputInitError getInitError( ) { return m_initError; }

	// Startup / Shutdown processes
	InputInitError startup( ); void shutdown( ) { }

	// Controller state accessors

	// Device caps accessors

	// Returns true if the controller is active
	int isActive( int controller ) { return m_isActive[controller]; }

	// Singleton accessor	
	static InputManager* instance( )
	{
		static InputManager gSingleton;
		return &gSingleton;
	}

protected:
	// Update controller state data
	void detectControllers( );
	void updateControllers( );

private:
	InputManager( ) { }

	InputInitError m_initError;
	XINPUT_STATE m_stateData[XUSER_MAX_COUNT];
	XINPUT_CAPABILITIES m_caps[XUSER_MAX_COUNT];
	XINPUT_GAMEPAD m_gamepads[XUSER_MAX_COUNT];
	int m_isActive[XUSER_MAX_COUNT];
};

}

// End file definition
#endif
