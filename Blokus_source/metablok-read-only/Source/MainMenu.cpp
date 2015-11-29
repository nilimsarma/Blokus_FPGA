/* ===========================================================================

	Project: MetaBlok - MainMenu

	Description: Main menu for simulator application

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

// Standard includes
#include "MetaBlok.h"

// Include header
#include "MainMenu.h"

// DirectX GUI controls
using namespace DirectX::GUI;

// Main menu buttons
#define MM_NBUTTONS    6
#define MM_SIMULATOR   0
#define MM_MULTIPLAYER 1
#define MM_OPENINGS    2
#define MM_SETTINGS	   3
#define MM_CREDITS     4
#define MM_QUIT        5

// Main menu button text
static const wchar_t* strings[] = { L"MATCH SIMULATOR", L"MULTIPLAYER", 
	L"OPENING DEVELOPER", L"GRAPHICS SETTINGS", L"CREDITS", L"QUIT" };

// --------------------------------------------------------
//	Creates the GUI display panel and associated resources
// --------------------------------------------------------
void MainMenu::startup( )
{	
	// Grab handle to manager object
	m_manager = DirectX::Manager::instance( );

	// Register this object as a controller
	m_manager->addController( this );

	// Load background sprite
	m_backdropImg.create( L"Images/Menu.bmp" );

	// Get default window style from engine
	m_style = DirectX::EngineManager::instance( )->getWindowStyle( );

	// Create user interface display panel
	m_displayPanel = &DisplayPanel::create( ).size( 1024, 768 );

	// Create backdrop graphic
	m_backdrop = &Graphic::create( &m_backdropImg, m_displayPanel );

	// Create interface buttons
	for( int i = 0; i < MM_NBUTTONS; i++ )
		m_buttons[i] = &Button::create( m_style, m_displayPanel ).text( strings[i] );

	// Proceed to run
	m_state = RUNNING;

	// Window size error fix
	m_manager->reset( );
}
// 
// --------------------------------------------------------
//  Repositions GUI elements when the screen dimensions 
//  are changed.
// --------------------------------------------------------
void MainMenu::onDeviceReset( )
{
	// Update background graphic position
	int sh = m_manager->getDisplayHeight( ); int sw = sh * 4 / 3;
	int px = ( m_manager->getDisplayWidth( ) - sw ) / 2;
	m_backdrop->size( sw, sh ).pos( px, 0 );

	// Update button positions
	int height = m_manager->getDisplayHeight( ) / 2 - 80*(MM_NBUTTONS/2) + 90;
	int width = m_manager->getDisplayWidth( ) / 2 - 100;
	for( int i = 0; i < MM_NBUTTONS; i++ )
		m_buttons[i]->pos( width, height+70*i ).size( 200, 50 );
}
//
// --------------------------------------------------------
//  Handle state changes on menu button selection.
// --------------------------------------------------------
void MainMenu::onGuiEvent( DirectX::GUI::Control* control, unsigned int message, void* data )
{
	// Get handle to management class
	DirectX::EngineManager* manager = DirectX::EngineManager::instance( );

	// Launch simulator game state
    if( control == m_buttons[MM_SIMULATOR] ) 
	{ 
		manager->popState( ); m_state = COMPLETED; 
		manager->pushState( new MatchUI( ) );
	}

	// Launch opening book developer
	else if( control == m_buttons[MM_OPENINGS] ) 
	{
		manager->popState( ); m_state = COMPLETED; 
		manager->pushState( new OpeningUI( ) );
	}

	// Launch networking menu
	else if( control == m_buttons[MM_MULTIPLAYER] )
	{
		manager->popState( ); m_state = COMPLETED; 
		manager->pushState( new NetworkSetup( ) );
	}

	// Application termination
	else if( control == m_buttons[MM_QUIT] ) 
		{ manager->popState( ); m_state = COMPLETED; }
}
//
// --------------------------------------------------------
//	Deletes the GUI panel and releases graphics resources
// --------------------------------------------------------
void MainMenu::shutdown( )
{
	// Delete GUI panel
	delete m_displayPanel;

	// Delete this object as a controller
	m_manager->deleteController( this );

	// Release images
	m_backdropImg.release( );
}