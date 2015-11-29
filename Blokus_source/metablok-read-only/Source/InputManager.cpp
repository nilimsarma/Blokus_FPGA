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

// Standard Includes
#include "DirectX.h"

// Include header
#include "InputManager.h"

// --------------------------------------------------------
//  Detects and registers the initially active controllers
//  and their capabilities.
// --------------------------------------------------------
DirectX::InputInitError DirectX::InputManager::startup( )
{
	// Detect controllers
	detectControllers( );

	return IIE_NONE;
}
//
// --------------------------------------------------------
//  Updates the active controller flags and reloads the 
//  device caps when new hardware is detected through the
//  windows message callback in the engine manager.
// --------------------------------------------------------
void DirectX::InputManager::detectControllers( )
{
	// Detect initial controllers
	for( int i = 0; i < XUSER_MAX_COUNT; i++ )
		if( XInputGetCapabilities( i, 0, &m_caps[i] ) == ERROR_SUCCESS )
			m_isActive[i] = TRUE;  
		else m_isActive[i] = FALSE;
}
//
// --------------------------------------------------------
//  Updates the state information for any controllers which
//  are currently active.
// --------------------------------------------------------
void DirectX::InputManager::updateControllers( )
{
	// Update state data for active controllers
	for( int i = 0; i < XUSER_MAX_COUNT; i++ )
		if( m_isActive[i] ) XInputGetState( i, &m_stateData[i] );
}