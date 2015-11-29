/* ===========================================================================

	Project: Game Engine - Manager

	Description:
	 Handles initialization and shutdown of engine components along with
	 controlling game states and managing application shutdown.

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
#ifndef DX_ENGINE_MANAGER_H
#define DX_ENGINE_MANAGER_H

// Abstract GameState
#include "GameState.h"

// Engine namespace
namespace DirectX
{
	// Root class
	class EngineManager : public DirectX::Controller
	{
	public:
		// Startup and shutdown procedures
		void startUp( HINSTANCE instHandle ); 
		void shutDown( );

		// Returns true if the engine is still running a state
		bool isRunning( ) { return callStack.empty( ) ? false : true; }

		// Main function
		void main( );

		// DirectX controller methods
		bool onDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat );
		void onMessageEvent( UINT message, WPARAM wParam, LPARAM lParam );
		void onRenderFrame( DirectX::Camera* camera );

		// Default GUI style accessors :TODO: Breakdown window style
		DirectX::GUI::WindowStyle* getWindowStyle( ) { return &m_windowStyle; }

		// Debug font accessor
		DirectX::Font* getFont( ) { return &m_consoleFont; }

		// State transitions
		GameState* getState( ) { return currentState; }
		void pushState( GameState* state ) { callStack.push( state ); }
		void popState( ) { callStack.pop( ); }

		// Singleton access
		static EngineManager* instance( )
		{
			static EngineManager gSingleton;
			return &gSingleton;
		}

	private:
		// Contructor
		EngineManager( ) : currentState(NULL) { } 

		// DirectX Engine manager handle
		DirectX::Manager* graphicsManager;
		DirectX::NetworkManager* networkManager;
		DirectX::InputManager* inputManager;

		// Engine GUI Resources
		void loadDefaultWindowStyle( );
		DirectX::GUI::WindowStyle m_windowStyle;

		// Console window interface
		DirectX::GUI::DisplayPanel* m_consoleDisplay;
		DirectX::GUI::Window* m_consoleWin;
		DirectX::Font m_consoleFont;

		// Stack for user defined gamestates
		std::stack<GameState*> callStack;
		GameState* currentState;
	};
	
}

// End definition
#endif