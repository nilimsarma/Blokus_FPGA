/* ===========================================================================

	Project: MetaBlok - NetworkSetup

	Description: Networked match waiting screen and settings options menu.

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

// Include header
#include "NetworkSetup.h"

// Standard includes
#include "MetaBlok.h"

// DirectX GUI controls
using namespace DirectX::GUI;

// --------------------------------------------------------
//	Creates the GUI display panel and associated resources.
// --------------------------------------------------------
void NetworkSetup::startup( )
{	
	// Acquire handles to subsystem managers
	m_graphicsManager = DirectX::Manager::instance( );
	m_networkManager = DirectX::NetworkManager::instance( );
	m_engineManager = DirectX::EngineManager::instance( );

	// Register this object as a controller
	m_graphicsManager->addController( this );

	// Begin hosting a match 
	beginHostingMatch( );

	// Load background sprite
	m_backdropImg.create( L"Images/Menu.bmp" );

	// Get default window style from engine
	m_style = m_engineManager->getWindowStyle( );

	// Create user interface display panel
	m_displayPanel = &DisplayPanel::create( ).size( 1024, 768 );

	// Create backdrop graphic
	m_backdrop = &Graphic::create( &m_backdropImg, m_displayPanel );

	// Create control button panel
	m_buttonConnect = &Button::create( m_style, m_displayPanel )
		.size( 128, 32 ).text( L"Connect" );
	m_buttonServer = &Button::create( m_style, m_displayPanel )
		.size( 128, 32 ).text( L"Host" );
	m_buttonStart = &Button::create( m_style, m_displayPanel )
		.size( 128, 32 ).text( L"Ready" );
	m_buttonCancel = &Button::create( m_style, m_displayPanel )
		.size( 128, 32 ).text( L"Cancel" );

	// Position GUI elements
	positionGuiElements( );

	// Proceed to run
	m_state = RUNNING;
}
//
// --------------------------------------------------------
//	Initializes this instance as a match server and begins
//  listing on the default port.
// --------------------------------------------------------
void NetworkSetup::beginHostingMatch( )
{
	// Initialize this instance as a host on the default port if possible
	m_serverSocket = m_networkManager->createServer( DirectX::NCT_TCP, L"8000" );

	// Check for socket initialization error
	if( !m_networkManager->getSocketError( m_serverSocket ) ) 
	{
		// Enable non-blocking io for single-threaded server
		m_networkManager->makeNonBlocking( m_serverSocket );
		m_isHost = true; m_isClient = false;
	} 
	else 
	{ 
		// Close the connection to clear any dangling data
		m_networkManager->closeConnection( m_serverSocket );
		m_isClient = false; m_isHost = false; 
	}
}
//
// --------------------------------------------------------
//  Repositions GUI elements when the screen dimensions 
//  are changed.
// --------------------------------------------------------
void NetworkSetup::positionGuiElements( )
{
	// Get display dimensions from graphics manager
	int sh = m_graphicsManager->getDisplayHeight( ); int sw = sh * 4 / 3;

	// Position backdrop graphic
	int px = ( m_graphicsManager->getDisplayWidth( ) - sw ) / 2;
	m_backdrop->size( sw, sh ).pos( px, 0 );

	// Control buttons
	int boff = sw/2-64;
	m_buttonServer->pos( boff-225, sh-64 );
	m_buttonConnect->pos( boff-75, sh-64 );
	m_buttonStart->pos( boff+75, sh-64 );
	m_buttonCancel->pos( boff+225, sh-64 );
}
// 
// --------------------------------------------------------
//  Calls the gui repositioning procedure when the graphics
//  device is reset.
// --------------------------------------------------------
void NetworkSetup::onDeviceReset( )
{
	// Reposition gui elements
	positionGuiElements( );
}
//
// --------------------------------------------------------
//  Handle state changes on menu button selection.
// --------------------------------------------------------
void NetworkSetup::onGuiEvent( DirectX::GUI::Control* control, unsigned int message, void* data )
{
	// Begin hosting match
	if( control == m_buttonServer )
	{
		// Ignore if an active host
		if( m_isHost ) return;

		// Prompt if an active client
		else if( m_isClient ) ;

		else ;
	}

	// Launch simulator game state
    if( control == m_buttonCancel ) 
	{ 
		m_engineManager->popState( ); m_state = COMPLETED; 
		m_engineManager->pushState( new MainMenu( ) );
	}

	// Handle ready-up flagging
	else if( control == m_buttonStart )
	{
		// Set flag if host
		if( m_isHost )
		{
			m_isClientReady[0] = !m_isClientReady[0];
		}

		// Notify server if client
		else if( m_isClient )
		{
		}

		else return;
	}
}
//
// --------------------------------------------------------
//	Deletes the GUI panel and releases graphics resources.
// --------------------------------------------------------
void NetworkSetup::shutdown( )
{
	// Delete GUI panel
	delete m_displayPanel;

	// Delete this object as a controller
	m_graphicsManager->deleteController( this );

	// Release images
	m_backdropImg.release( );
}