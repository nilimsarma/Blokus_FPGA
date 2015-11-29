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

// Engine header
#include "DirectX.h"

// Include header
#include "EngineManager.h"

// Debug mode
#define SHEAR_DEBUG _DEBUG

// ---------------------------------------------------------
//	Creates and initializes the DirectX management object. 
//  It also sets the callback  functions for Direct3D and 
// initializes a window for the device. 
// ---------------------------------------------------------
void DirectX::EngineManager::startUp( HINSTANCE instHandle )
{
	// Acquire handles to subsystem controllers
	graphicsManager = DirectX::Manager::instance( );
	networkManager = DirectX::NetworkManager::instance( );
	inputManager = DirectX::InputManager::instance( );

	// Initialize subsystems
	//graphicsManager->startup( );
	networkManager->startup( );
	inputManager->startup( );

	// Register the engine as a graphics controller
	graphicsManager->addController( this );

	// Initialize the default display settings
	graphicsManager->createWindow( instHandle );
	graphicsManager->setDisplayMode( 800, 600, 32, 0 );
	graphicsManager->setWindowMode( true ); 
	graphicsManager->createDevice( );

	// Set default backdrop color to black
	graphicsManager->setBackdropColor( COLOR::BLACK );

	// Create default window style
	loadDefaultWindowStyle( );

	// Create engine console window
	//m_consoleWin.create( &m_windowStyle, NULL, 25, 25, 400, 450, 0xFFFF );
	m_consoleFont.createDefault( );
}
//
// ---------------------------------------------------------
//	Main function for game engine. Timing is handled 
//  internally with the DirectX manager, however the timer 
//  can be modified through the DirectX interface. The 
//  program will be automatically terminated from this 
//  function if the engine state stack is depleted.
// ---------------------------------------------------------
void DirectX::EngineManager::main( )
{
	// Check if the callstack is empty
	if( callStack.empty( ) ) return;

	// Check if the current state is unread
	if( !currentState ) currentState = callStack.top( );

	// If there is no new current game state
	// then process the current state else 
	// pause the current state and change the
	// current state to the state on the stack top
	int state = currentState->getState( );
	if( currentState == callStack.top( ) )
		switch( state )
		{
			case COMPLETED:
				// Shutdown previous state
				currentState->shutdown( );
				delete currentState;

				// Remove state from stack
				callStack.pop( );

				// Check for state stack depleted, 
				// else get next state from the stack
				if( callStack.empty( ) ) { currentState = NULL; return; }
				else currentState = callStack.top( );

				break;
			case WAITING:
				// Initialize state
				currentState->startup( );
			case PAUSED:
			case RUNNING:
				// Perform main functs
				currentState->main( );
		} 
	else
	{
		// Pause/Delete the current state 
		if( state != COMPLETED ) 
			currentState->pause( );
		else 
		{
			currentState->shutdown( );
			delete currentState;
			currentState = NULL;
		}

		// Change states
		if( !callStack.empty( ) ) {
			currentState = callStack.top( );
			currentState->startup( ); }
	}

	// Render the next frame
	graphicsManager->renderFrame( );
}
//
// ---------------------------------------------------------
//	Shutdown procedure to be called for engine release. All 
//  game states and  engines will be shutdown and the state 
//  stack will be cleared.
// ---------------------------------------------------------
void DirectX::EngineManager::shutDown( )
{
	// Shutdown any active gamestates
	while( !callStack.empty( ) )
	{
		// Shutdown the state
		GameState* state = callStack.top( );
		callStack.pop( ); state->shutdown( );

		// Check if this is the current state
		if( state == currentState ) 
			currentState = NULL;
	}

	// Shutdown the current state if
	// it has not already been deleted
	if( currentState ) { 
		currentState->shutdown( ); 
		delete currentState; 
	}

	// Shutdown engine components
	graphicsManager->release( );
	networkManager->shutdown( );
	inputManager->shutdown( );
}
//
//----------------------------------------------------------
// This callback function will be called at the end of every 
// call to perform a camera render function, and it will 
// also be called if the window needs to be repainted and 
// the render on window paint option in the DirectX Manager 
// is true.
//----------------------------------------------------------
void DirectX::EngineManager::onRenderFrame( DirectX::Camera* camera )
{
	if( !SHEAR_DEBUG ) return; // Check for debug mode

	// String processing devices
	std::wstringstream stringStream;
	std::wstring timeElapsed, fps, spf;

	// Convert application running time to appropriate display format
	float secondsAsF; float timeAsF = DirectX::Manager::instance( )->getRunningTime( );
	int milliseconds = (int)(1000.0f*modf( timeAsF, &secondsAsF ));
	int timeAsI = (int)secondsAsF;
	int hours = timeAsI / 3600;
	int minutes = ( timeAsI - hours*3600 ) / 60;
	int seconds = timeAsI - hours*3600 - minutes*60;
	
	int chars[9];
	chars[0] = hours / 10;
	chars[1] = hours - chars[0]*10;
	chars[2] = minutes / 10;
	chars[3] = minutes - chars[2]*10;
	chars[4] = seconds / 10;
	chars[5] = seconds - chars[4]*10;
	chars[6] = milliseconds / 100;
	chars[7] = ( milliseconds - chars[6]*100 ) / 10;
	chars[8] = milliseconds - chars[6]*100 - chars[7]*10;

	stringStream << chars[0] << chars[1] << L"::";
	stringStream << chars[2] << chars[3] << L"::";
	stringStream << chars[4] << chars[5] << L"::";
	stringStream << chars[6] << chars[7] << chars[8];
	stringStream >> timeElapsed; 

	// Convert timing data to string format
	stringStream.str( L"" ); stringStream.clear( );
	stringStream << DirectX::Manager::instance( )->getFPS( );
	fps = stringStream.str( ); stringStream.str( L"" ); stringStream.clear( );
	stringStream << 1000.0 / DirectX::Manager::instance( )->getFPS( );
	spf = stringStream.str( ); stringStream.str( L"" ); stringStream.clear( );

	// Clips strings to reasonable display length
	fps = std::wstring(L"Framerate: ") + fps.substr( 0, 4 );
	spf = std::wstring(L"Frame Render Time(ms): ") + spf.substr( 0, 4 );

	// Display generic application debug statistics
	m_consoleFont.drawText( 5,  5, spf.c_str( ), COLOR::WHITE );
	m_consoleFont.drawText( 5, 25, fps.c_str( ), COLOR::WHITE );
	m_consoleFont.drawText( 5, 45, timeElapsed.c_str( ), COLOR::WHITE );
}
//
// ---------------------------------------------------------
//	This callback function will be called after device 
//  creation and its callback. This is where the d3ddevice 
//  should be checked against the minimum requirements for 
//	system compatibility.
// ---------------------------------------------------------
bool DirectX::EngineManager::onDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat ) 
{
	// Check pixel and vertex shader versions
	if( pCaps->PixelShaderVersion < D3DPS_VERSION( 2, 0 ) ) return false;

	// Accept settings
	return true;
}
//
//--------------------------------------------------------------------------------------
//  This function will be called immediately after the Direct3D device window 
//  has recieved a an event msg that is affiliated with the device window
//--------------------------------------------------------------------------------------
void DirectX::EngineManager::onMessageEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
	// Check for close msg event when exitOnClose
	// variable is set within DirectX Engine
	if( message == WM_DESTROY ) {
		if( currentState ) { 
			currentState->shutdown( );
			delete currentState;
		}
	}
}
//																																					
//--------------------------------------------------------------------------------------
//  Loads the default window style sprite sheet and specifies icon regions for use by
//  user defined game states
//--------------------------------------------------------------------------------------
void DirectX::EngineManager::loadDefaultWindowStyle( )
{
	// Create the default window style
	m_windowStyle.spriteSheet.create( L"Images\\Window.bmp", 0xffffffff );

	// GUI font style
	m_windowStyle.font.createWin32Font( L"arial", 16, false, false );
	m_windowStyle.color = 0xFF000000;

	// List and combo boxes
	m_windowStyle.downArrow.left = 85;
	m_windowStyle.downArrow.right = 101;
	m_windowStyle.downArrow.top = 33;
	m_windowStyle.downArrow.bottom = 49;
	m_windowStyle.downArrowHover.left = 102;
	m_windowStyle.downArrowHover.right = 118;
	m_windowStyle.downArrowHover.top = 67;
	m_windowStyle.downArrowHover.bottom = 83;
	m_windowStyle.downArrowPressed.left = 85;
	m_windowStyle.downArrowPressed.right = 101;
	m_windowStyle.downArrowPressed.top = 50;
	m_windowStyle.downArrowPressed.bottom = 66;

	m_windowStyle.upArrow.left = 68;
	m_windowStyle.upArrow.right = 84;
	m_windowStyle.upArrow.top = 33;
	m_windowStyle.upArrow.bottom = 49;
	m_windowStyle.upArrowHover.left = 102;
	m_windowStyle.upArrowHover.right = 118;
	m_windowStyle.upArrowHover.top = 84;
	m_windowStyle.upArrowHover.bottom = 100;
	m_windowStyle.upArrowPressed.left = 68;
	m_windowStyle.upArrowPressed.right = 84;
	m_windowStyle.upArrowPressed.top = 50;
	m_windowStyle.upArrowPressed.bottom = 66;

	m_windowStyle.selectionBox.left = 85;
	m_windowStyle.selectionBox.right = 101;
	m_windowStyle.selectionBox.top = 84;
	m_windowStyle.selectionBox.bottom = 100;
	m_windowStyle.backdrop.left = 102;
	m_windowStyle.backdrop.right = 118;
	m_windowStyle.backdrop.top = 101;
	m_windowStyle.backdrop.bottom = 117;

	m_windowStyle.scroller.left = 122;
	m_windowStyle.scroller.right = 138;
	m_windowStyle.scroller.top = 101;
	m_windowStyle.scroller.bottom = 117;

	m_windowStyle.scrollbar.left = 139;
	m_windowStyle.scrollbar.right = 155;
	m_windowStyle.scrollbar.top = 101;
	m_windowStyle.scrollbar.bottom = 117;

	// Standard window
	m_windowStyle.windowBorderTopLeft.left = 0;
	m_windowStyle.windowBorderTopLeft.right = 16;
	m_windowStyle.windowBorderTopLeft.top = 0;
	m_windowStyle.windowBorderTopLeft.bottom = 32;
	m_windowStyle.windowBorderTop.left = 17;
	m_windowStyle.windowBorderTop.right = 33;
	m_windowStyle.windowBorderTop.top = 0;
	m_windowStyle.windowBorderTop.bottom = 32;
	m_windowStyle.windowBorderTopRight.left = 131;
	m_windowStyle.windowBorderTopRight.right = 179;
	m_windowStyle.windowBorderTopRight.top = 0;
	m_windowStyle.windowBorderTopRight.bottom = 32;
	m_windowStyle.windowBorderTopRightClose.left = 131;
	m_windowStyle.windowBorderTopRightClose.right = 179;
	m_windowStyle.windowBorderTopRightClose.top = 33;
	m_windowStyle.windowBorderTopRightClose.bottom = 65;
	m_windowStyle.windowBorderTopRightMin.left = 131;
	m_windowStyle.windowBorderTopRightMin.right = 179;
	m_windowStyle.windowBorderTopRightMin.top = 66;
	m_windowStyle.windowBorderTopRightMin.bottom = 98;
	m_windowStyle.windowBorderLeft.left = 0;
	m_windowStyle.windowBorderLeft.right = 16;
	m_windowStyle.windowBorderLeft.top = 33;
	m_windowStyle.windowBorderLeft.bottom = 49;
	m_windowStyle.windowBorderRight.left = 0;
	m_windowStyle.windowBorderRight.right = 16;
	m_windowStyle.windowBorderRight.top = 33;
	m_windowStyle.windowBorderRight.bottom = 49;
	m_windowStyle.windowMiddleTransparent.left = 51;
	m_windowStyle.windowMiddleTransparent.right = 67;
	m_windowStyle.windowMiddleTransparent.top = 33;
	m_windowStyle.windowMiddleTransparent.bottom = 49;
	m_windowStyle.windowMiddleOpaque.left = 0;
	m_windowStyle.windowMiddleOpaque.right = 16;
	m_windowStyle.windowMiddleOpaque.top = 33;
	m_windowStyle.windowMiddleOpaque.bottom = 49;
	m_windowStyle.windowBorderBottomLeft.left = 34;
	m_windowStyle.windowBorderBottomLeft.right = 50;
	m_windowStyle.windowBorderBottomLeft.top = 33;
	m_windowStyle.windowBorderBottomLeft.bottom = 49;
	m_windowStyle.windowBorderBottom.left = 0;
	m_windowStyle.windowBorderBottom.right = 16;
	m_windowStyle.windowBorderBottom.top = 33;
	m_windowStyle.windowBorderBottom.bottom = 49;
	m_windowStyle.windowBorderBottomRight.left = 17;
	m_windowStyle.windowBorderBottomRight.right = 33;
	m_windowStyle.windowBorderBottomRight.top = 33;
	m_windowStyle.windowBorderBottomRight.bottom = 49;
	m_windowStyle.windowBorderTopMiddle.left = 33;
	m_windowStyle.windowBorderTopMiddle.right = 130;
	m_windowStyle.windowBorderTopMiddle.top = 0;
	m_windowStyle.windowBorderTopMiddle.bottom = 32;
	m_windowStyle.windowBorderTopLeftMin.left = 0;
	m_windowStyle.windowBorderTopLeftMin.right = 16;
	m_windowStyle.windowBorderTopLeftMin.top = 51;
	m_windowStyle.windowBorderTopLeftMin.bottom = 83;
	m_windowStyle.windowBorderTopRightMinMin.left = 180;
	m_windowStyle.windowBorderTopRightMinMin.right = 228;
	m_windowStyle.windowBorderTopRightMinMin.top = 66;
	m_windowStyle.windowBorderTopRightMinMin.bottom = 98;
	m_windowStyle.windowBorderTopRightMinClose.left = 180;
	m_windowStyle.windowBorderTopRightMinClose.right = 228;
	m_windowStyle.windowBorderTopRightMinClose.top = 33;
	m_windowStyle.windowBorderTopRightMinClose.bottom = 65;
	m_windowStyle.windowBorderTopRightMinNormal.left = 180;
	m_windowStyle.windowBorderTopRightMinNormal.right = 228;
	m_windowStyle.windowBorderTopRightMinNormal.top = 0;
	m_windowStyle.windowBorderTopRightMinNormal.bottom = 32;

	// Standard button
	m_windowStyle.button.left = 17;
	m_windowStyle.button.right = 33;
	m_windowStyle.button.top = 50;
	m_windowStyle.button.bottom = 66;
	m_windowStyle.buttonHover.left = 34;
	m_windowStyle.buttonHover.right = 50;
	m_windowStyle.buttonHover.top = 50;
	m_windowStyle.buttonHover.bottom = 66;
	m_windowStyle.buttonDown.left = 51;
	m_windowStyle.buttonDown.right = 67;
	m_windowStyle.buttonDown.top = 50;
	m_windowStyle.buttonDown.bottom = 66;
	m_windowStyle.buttonInfo.left = 85;
	m_windowStyle.buttonInfo.right = 101;
	m_windowStyle.buttonInfo.top = 67;
	m_windowStyle.buttonInfo.bottom = 83;
	m_windowStyle.buttonScaleable = true;

	// Standard Checkboxes
	m_windowStyle.checkboxOff.left = 102;
	m_windowStyle.checkboxOff.right = 118;
	m_windowStyle.checkboxOff.top = 33;
	m_windowStyle.checkboxOff.bottom = 49;
	m_windowStyle.checkboxOn.left = 102;
	m_windowStyle.checkboxOn.right = 118;
	m_windowStyle.checkboxOn.top = 50;
	m_windowStyle.checkboxOn.bottom = 66;
}