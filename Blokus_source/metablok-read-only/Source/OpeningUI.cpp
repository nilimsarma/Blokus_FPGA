/* ===========================================================================

	Project: MetaBlok - OpeningUI

	Description: Graphical interface for developing opening books.

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

// Standard includes
#include "MetaBlok.h"

// Include header
#include "OpeningUI.h"

// DirectX GUI controls
using namespace DirectX::GUI;

// Toolbar buttons
#define TB_NBUTTONS        11
#define TB_NEW_BOOK         0
#define TB_LOAD_BOOK        1
#define TB_SAVE_BOOK        2
#define TB_SAVEAS_BOOK      3
#define TB_SETTINGS_BOOK    4
#define TB_SETTINGS_SCREEN  5
#define TB_UNDO				6
#define TB_REDO				7
#define TB_NEXT_VAR			8
#define TB_PREV_VAR			9
#define TB_QUIT			   10

// Human player 
#define NONE -1

// --------------------------------------------------------
//	Catches gui events and signals the attached match on
//  relevant message events.
// --------------------------------------------------------
void OpeningUI::onGuiEvent( DirectX::GUI::Control* control, unsigned int message, void* data )
{
	// Delete window panels when closed
	if( message == CM_WINDOW_CLOSED ) 
	{
		if( control == m_graphWin ) { delete m_graphWin; m_graphWin = NULL; }
		if( control == m_loadWin ) { delete m_loadWin; m_loadWin = NULL; }
		if( control == m_saveWin ) { delete m_saveWin; m_saveWin = NULL; }
		if( control == m_playWin ) { delete m_playWin; m_playWin = NULL; }
		if( control == m_quitWin ) { delete m_quitWin; m_quitWin = NULL; }
		if( control == m_newWin ) { delete m_newWin; m_newWin = NULL; }
	}

	// New match warning window
	else if( control->getPanel( ) == m_newWin )
	{
		if( control == m_newCancelButton ) { delete m_newWin; m_newWin = NULL; }
		if( control == m_newAcceptButton ) { delete m_newWin; 
			m_match.beginNewMatch( NULL ); m_newWin = NULL; }
	}

	// Match players window
	else if( control->getPanel( ) == m_playWin )
	{
		if( control == m_playCancelButton ) { delete m_playWin; m_playWin = NULL; }
		if( control == m_playAcceptButton ) { applyPlayers( ); delete m_playWin; 
			m_playWin = NULL; }
	}

	// Save as match window
	else if( control->getPanel( ) == m_saveWin )
	{
		if( control == m_saveCancelButton ) { delete m_saveWin; m_saveWin = NULL; }
		if( control == m_saveAcceptButton ) { quickSave( ); delete m_saveWin; m_saveWin = NULL; }
	}

	// Graphics settings window
	else if( control->getPanel( ) == m_graphWin )
	{
		if( control == m_graphCancelButton ) { delete m_graphWin; m_graphWin = NULL; }
		if( control == m_graphAcceptButton ) { applyGraphicsSettings( ); 
			delete m_graphWin; m_graphWin = NULL; }
		if( control == m_graphApplyButton ) { 
			applyGraphicsSettings( ); delete m_graphWin; 
			m_graphWin = NULL; createGraphicsWindow( ); }
	}

	// Quit verification window
	else if( control->getPanel( ) == m_quitWin )
	{
		DirectX::EngineManager* manager = DirectX::EngineManager::instance( );
		if( control == m_quitCancelButton ) { delete m_quitWin; m_quitWin = NULL; }
		if( control == m_quitAcceptButton ) { manager->popState( ); 
			manager->pushState( new MainMenu( ) ); m_state = COMPLETED; }  
	}

	// Load map window
	else if( control->getPanel( ) == m_loadWin )
	{
		if( control == m_loadCancelButton ) { delete m_loadWin; m_loadWin = NULL; }
		if( control == m_loadAcceptButton ) { load( ); delete m_loadWin; m_loadWin = NULL; }
		if( control == m_loadDeleteButton ) ; //createDeleteWindow( );
	}

	// Primary toolbar buttons
	else if( control == m_button[TB_NEW_BOOK] ) createNewMatchWindow( );
	else if( control == m_button[TB_SETTINGS_SCREEN] ) createGraphicsWindow( );
	else if( control == m_button[TB_SETTINGS_BOOK]  ) createPlayerWindow( );
	else if( control == m_button[TB_UNDO] ) m_match.undoMove( );
	else if( control == m_button[TB_REDO] ) m_match.redoMove( );
	else if( control == m_button[TB_LOAD_BOOK] ) createLoadWindow( );
	else if( control == m_button[TB_SAVE_BOOK] ) quickSave( );
	else if( control == m_button[TB_SAVEAS_BOOK] ) createSaveWindow( );
	else if( control == m_button[TB_NEXT_VAR] ) m_match.play( ); 
	else if( control == m_button[TB_PREV_VAR] ) m_match.step( );
	else if( control == m_button[TB_QUIT] ) createQuitWindow( );
}
//
// --------------------------------------------------------
//	Handles hotkeys for quick match interactions.
// --------------------------------------------------------
void OpeningUI::onKeyboardEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
	// Filter out of focus events
	if( m_manager->isFocusedControl( ) ) return;

	// Get scancode of key
	int key = MapVirtualKey( ( BYTE )( wParam & 0xFF ), MAPVK_VK_TO_VSC );

	// Key pressed
	if( message == WM_KEYDOWN )
	{
		// Undo/Redo move operations 
		if( key == KEY_Z ) if( m_manager->getKeyState( KEY_LCTRL ) ) m_match.undoMove( );
		if( key == KEY_Y ) if( m_manager->getKeyState( KEY_LCTRL ) ) m_match.redoMove( );

		// Open load match window
		if( key == KEY_O ) if( m_manager->getKeyState( KEY_LCTRL ) ) createLoadWindow( );

		// Quick save if filename specified, otherwise saveAs
		if( key == KEY_S ) if( m_manager->getKeyState( KEY_LCTRL ) ) quickSave( );
	}
}
//
// --------------------------------------------------------
//	Initializes a match with default settings and creates
//  the GUI toolbar.
// --------------------------------------------------------
void OpeningUI::startup( )
{
	// Get handle to directX Manager
	m_manager = DirectX::Manager::instance( );

	// Register as an active controller
	m_manager->addController( this );

	// Get default window style from engine
	m_style = DirectX::EngineManager::instance( )->getWindowStyle( );

	// Create user interface display panel
	m_displayPanel = &DisplayPanel::create( ).size( 1024, 768 );

	// Initialize match
	m_match.startup( );

	// Create toolbar
	createToolbar( );

	// Proceed to run
	m_state = RUNNING;
}
//
// --------------------------------------------------------
//	Deletes the GUI panel and terminates the active match.
// --------------------------------------------------------
void OpeningUI::shutdown( )
{
	// Delete GUI panel
	delete m_displayPanel;

	// Delete this object as a controller
	m_manager->deleteController( this );

	// Terminate match
	m_match.shutdown( );
}
//
// --------------------------------------------------------
//	Creates the primary toolbar along the bottom edge of 
//  the screen.
// --------------------------------------------------------
void OpeningUI::createToolbar( )
{
	// Load button icons
	m_spriteSheet.create( L"Images\\OpeningIcons.bmp", 0xffff0000 );

	// Toolbar icon rectangles
	RECT buttonRect[TB_NBUTTONS];
	for( int g = 0; g < TB_NBUTTONS; g++ ) {
		int i = g / 4; int j = g % 4;
		buttonRect[g].left   = 32*i;
		buttonRect[g].right  = 32*(i+1);
		buttonRect[g].top    = 32*j;
		buttonRect[g].bottom = 32*(j+1); }

	// Create interface buttons
	int height = m_manager->getDisplayHeight( ) - 50;
	int width = (m_manager->getDisplayWidth( ) - TB_NBUTTONS*40 + 8)/2;
	for( int i = 0; i < TB_NBUTTONS; i++ )
		m_button[i] = &Button::create( m_style, m_displayPanel )
						.graphic( &m_spriteSheet, &buttonRect[i] )
						.pos( width+40*i, height )
						.size( 32, 32 );
}
//
// --------------------------------------------------------
//	Creates the graphics settings window panel.
// --------------------------------------------------------
void OpeningUI::createGraphicsWindow( )
{
	if( m_graphWin ) return;

	// Get current display mode settings
	int height = m_manager->getDisplayHeight( );
	int width  = m_manager->getDisplayWidth( );

	// Graphics window panel
	m_graphWin = &Window::create( m_style, m_displayPanel )
					.pos( width/2-125, height/2-112 )
					.size( 250, 225 ).depth( -1 )
					.notUnfocusable( );

	// Vertical sync settings checkbox and label
	Label::create( m_style, m_graphWin ).pos( 35, 100 )
		.size( 200, 24 ).text( L"Wait for vertical sync?" );
	m_graphVSync = &Checkbox::create( m_style, m_graphWin )
		.pos( 30, 100 ).size( 24, 24 ).checked( );

	// Windowed mode settings checkbox and label
	Label::create( m_style, m_graphWin ).pos( 35, 128 )
		.size( 200, 24 ).text( L"Play in window?" );
	m_graphMode = &Checkbox::create( m_style, m_graphWin )
		.pos( 30, 128 ).size( 24, 24 ).checked( );

	// Graphics panel buttons
	m_graphApplyButton  = &Button::create( m_style, m_graphWin )
		.pos(  45, 175 ).size( 48, 24 ).text( L"Apply" );
	m_graphAcceptButton = &Button::create( m_style, m_graphWin )
		.pos( 100, 175 ).size( 48, 24 ).text( L"Accept" );
	m_graphCancelButton = &Button::create( m_style, m_graphWin )
		.pos( 155, 175 ).size( 48, 24 ).text( L"Cancel" );

	// Display modes combo box
	m_graphDisp = &ComboBox::create( m_style, m_graphWin )
		.pos( 30, 60 ).size( 180, 24 ).depth( -1 );

	// Populate display mode settings combo box
	DirectX::DisplayMode curr = m_manager->getDisplayMode( );
	for( int i = 0; i < m_manager->getAdapterDisplayModeCount( 0 ); i++ )
	{
		// Add the display mode to the combo box if it meets min specs
		DirectX::DisplayMode mode = m_manager->getAdapterDisplayMode( 0, i );
		if( mode.Height < 600 || mode.Format != D3DFMT_X8R8G8B8 || mode.RefreshRate < 60 ) continue;
		std::wstringstream stream; stream << mode.Width << "x" << mode.Height << " at " << mode.RefreshRate << " Hertz";
		m_graphDisp->addItem( stream.str( ).c_str( ), i );

		// Check for current display mode setting
		if( mode.Format == curr.Format && 
			mode.Height == curr.Height && 
			mode.Width == curr.Width ) 
			m_graphDisp->setItem( i );
	}
}
//
// --------------------------------------------------------
//	Creates the save match as window panel.
// --------------------------------------------------------
void OpeningUI::createSaveWindow( )
{
	if( m_saveWin ) return;

	// Get current display mode settings
	int height = m_manager->getDisplayHeight( );
	int width  = m_manager->getDisplayWidth( );

	// Save as window panel
	m_saveWin = &Window::create( m_style, m_displayPanel )
					.pos( width/2-125, height/2-88 )
					.size( 250, 175 ).depth( -1 )
					.notUnfocusable( );

	// Filename edit box
	m_saveName = &EditBox::create( m_style, m_saveWin )
		.pos( 30, 60 ).size( 200, 24 );

	// Save as panel buttons
	m_saveAcceptButton  = &Button::create( m_style, m_saveWin )
		.pos(  50, 115 ).size( 72, 24 ).text( L"Save" );
	m_saveCancelButton  = &Button::create( m_style, m_saveWin )
		.pos( 125, 115 ).size( 72, 24 ).text( L"Cancel" );
}
//
// --------------------------------------------------------
//  Creates the player selection window.
// --------------------------------------------------------
void OpeningUI::createPlayerWindow( )
{
	if( m_playWin ) return;

	// Get current display mode settings
	int height = m_manager->getDisplayHeight( );
	int width  = m_manager->getDisplayWidth( );

	// Player selection window panel
	m_playWin = &Window::create( m_style, m_displayPanel )
					.pos( width/2-105, height/2-112 )
					.size( 210, 225 ).depth( -1 )
					.notUnfocusable( );

	// Player selection panel buttons
	m_playAcceptButton = &Button::create( m_style, m_playWin )
		.pos( 50, 175 ).size( 48, 24 ).text( L"Accept" );
	m_playCancelButton = &Button::create( m_style, m_playWin )
		.pos( 105, 175 ).size( 48, 24 ).text( L"Cancel" );

	// Player selection combo boxes
	for( int p = 3; p >= 0; p-- )
	m_playSel[p] = &ComboBox::create( m_style, m_playWin )
		.pos( 30, 40+p*30 ).size( 150, 24 ).depth( -1 );

	// Add human play option to boxes
	for( int i = 0; i < 4; i++ ) m_playSel[i]->addItem( L"Human", NONE );

	// Check if there are any players to be loaded
	WIN32_FIND_DATAW findData; HANDLE fileHandle;
	fileHandle = FindFirstFileW( L"Players\\*", &findData );
	if( fileHandle == INVALID_HANDLE_VALUE ) return;

	// Loop through files
	int key = 0; do 
	{ 
		// Compose filename string
		std::wstring filename( L"Players\\" );
		filename += std::wstring( findData.cFileName );

		// Open the file to test for existance (filter folders)
		std::fstream file( filename.c_str( ), std::ios::in );

		// Check for file load failure
		if( !file.is_open( ) ) continue;
		else file.close( );

		// Add match to list box
		for( int i = 0; i < 4; i++ )
			m_playSel[i]->addItem( findData.cFileName, key ); key++;
	}  
	while( FindNextFileW( fileHandle, &findData ) );
	FindClose( fileHandle );
}
//
// --------------------------------------------------------
//	Creates the match settings window panel.
// --------------------------------------------------------
void OpeningUI::createMatchWindow( )
{
	if( m_matchWin ) return;
}
//
// --------------------------------------------------------
//  Creates the load file window panel.
// --------------------------------------------------------
void OpeningUI::createLoadWindow( )
{
	if( m_loadWin ) return;

	// Get current display mode settings
	int height = m_manager->getDisplayHeight( );
	int width  = m_manager->getDisplayWidth( );

	// Graphics window panel
	m_loadWin = &Window::create( m_style, m_displayPanel )
					.pos( width/2-125, height/2-150 )
					.size( 250, 300 ).depth( -1 )
					.notUnfocusable( );

	// Graphics panel buttons
	m_loadDeleteButton  = &Button::create( m_style, m_loadWin )
		.pos(  45, 250 ).size( 48, 24 ).text( L"Delete" );
	m_loadAcceptButton = &Button::create( m_style, m_loadWin )
		.pos( 100, 250 ).size( 48, 24 ).text( L"Load" );
	m_loadCancelButton = &Button::create( m_style, m_loadWin )
		.pos( 155, 250 ).size( 48, 24 ).text( L"Cancel" );

	// Display modes combo box
	m_loadMatches = &ListBox::create( m_style, m_loadWin )
		.pos( 40, 60 ).size( 170, 144 ).depth( -1 );

	// Check if there are any maps to be loaded
	WIN32_FIND_DATAW findData; HANDLE fileHandle;
	fileHandle = FindFirstFileW( L"Books\\*", &findData );
	if( fileHandle == INVALID_HANDLE_VALUE ) return;

	// Loop through files
	int key = 0; do 
	{ 
		// Compose filename string
		std::wstring filename( L"Saves\\" );
		filename += std::wstring( findData.cFileName );

		// Open the file to test for existance
		std::fstream file( filename.c_str( ), std::ios::in );

		// Check for file load failure
		if( !file.is_open( ) ) continue;
		else file.close( );

		// Check the file type
		int end = filename.size( );
		if( filename.at( end-4 ) != '.' ||
			filename.at( end-3 ) != 'o' ||
			filename.at( end-2 ) != 'b' ||
			filename.at( end-1 ) != 'k' )	
			continue; 

		// Remove file type extension
		findData.cFileName[filename.size( )-10] = '\0';

		// Add match to list box
		m_loadMatches->addItem( findData.cFileName, key ); key++;
	}  
	while( FindNextFileW( fileHandle, &findData ) );
	FindClose( fileHandle );
}
//
// --------------------------------------------------------
//  Creates the new match save warning window panel.
// --------------------------------------------------------
void OpeningUI::createNewMatchWindow( )
{
	if( m_newWin ) return;

	// Get current display mode settings
	int height = m_manager->getDisplayHeight( );
	int width  = m_manager->getDisplayWidth( );

	// Graphics window panel
	m_newWin = &Window::create( m_style, m_displayPanel )
					.pos( width/2-125, height/2-88 )
					.size( 250, 175 ).depth( -1 )
					.notUnfocusable( );

	// Unsaved data warning message
	Label::create( m_style, m_newWin )
		.text( L"Are you sure you want to\n start a new book?\n\nAny unsaved data will be lost." )
		.size( 210, 100 ).pos( 20, 30 );

	// New match panel buttons
	m_newAcceptButton  = &Button::create( m_style, m_newWin )
		.pos(  50, 120 ).size( 72, 24 ).text( L"Continue" );
	m_newCancelButton  = &Button::create( m_style, m_newWin )
		.pos( 125, 120 ).size( 72, 24 ).text( L"Cancel" );
}
//
// --------------------------------------------------------
//  Creates the quit save warning window panel.
// --------------------------------------------------------
void OpeningUI::createQuitWindow( )
{
	if( m_quitWin ) return;

	// Get current display mode settings
	int height = m_manager->getDisplayHeight( );
	int width  = m_manager->getDisplayWidth( );

	// Graphics window panel
	m_quitWin = &Window::create( m_style, m_displayPanel )
					.pos( width/2-125, height/2-88 )
					.size( 250, 175 ).depth( -1 )
					.notUnfocusable( );

	// Unsaved data warning message
	Label::create( m_style, m_quitWin )
		.text( L"Are you sure you want to\n quit without saving?\n\nAny unsaved data will be lost." )
		.size( 210, 100 ).pos( 20, 30 );

	// New match panel buttons
	m_quitAcceptButton  = &Button::create( m_style, m_quitWin )
		.pos(  50, 120 ).size( 72, 24 ).text( L"Quit" );
	m_quitCancelButton  = &Button::create( m_style, m_quitWin )
		.pos( 125, 120 ).size( 72, 24 ).text( L"Cancel" );
}
//
// --------------------------------------------------------
//	Applies the graphics settings as determined by the 
//  selected options in the graphics window panel.
// --------------------------------------------------------
void OpeningUI::applyGraphicsSettings( )
{
	// Update device display mode
	unsigned int adapter = 0, mode = m_graphDisp->getItem( );
	DirectX::DisplayMode displayMode = m_manager->
		getAdapterDisplayMode( adapter, mode );
	m_manager->setDisplayMode( displayMode );

	// Update vertical sync settings
	if( m_graphVSync->isChecked( ) ) 
		m_manager->enableVSync( );
	else m_manager->disableVSync( );
 
	// Update window mode settings
	if( m_graphMode->isChecked( ) ) 
		m_manager->setWindowMode( true );
	else m_manager->setWindowMode( false );

	// Reset device 
	m_manager->reset( );
}
//
// --------------------------------------------------------
//	Applies the player selections to the current match.
// --------------------------------------------------------
void OpeningUI::applyPlayers( )
{
	for( int i = 0; i < 4; i++ )
		m_match.setAiPlayer( i, m_playSel[i]->getItemText( ) );
}
//
// --------------------------------------------------------
//	Saves the match to the previously specified file.
// --------------------------------------------------------
void OpeningUI::quickSave( )
{
	// Check for active save file edit box
    if( m_saveWin ) m_saveFilename = m_saveName->getText( );

	// Unspecified filename, open save file as window
	if( m_saveFilename == L"" ) { createSaveWindow( ); return; }

	// Attempt to save the current book to disk
	//m_openingBook.saveToDisk( m_saveFilename.c_str( ) );
}
//
// --------------------------------------------------------
//	Loads the specified match into the match simulator.
// --------------------------------------------------------
void OpeningUI::load( )
{
	// Get the match filename from the list box
	int key = m_loadMatches->getLastSelected( );
	const wchar_t* filename = m_loadMatches->getString( key );
	
	// Attempt to load the specified match
	/*if( m_openingBook.loadFromDisk( filename ) ) 
		m_saveFilename = filename;*/
}
//
// --------------------------------------------------------
//	Updates GUI component positions if invalidated by a 
//  display mode change during the last reset operation.
// --------------------------------------------------------
void OpeningUI::onDeviceReset( )
{
	// Update toolbar buttons
	int height = m_manager->getDisplayHeight( ) - 50;
	int width = (m_manager->getDisplayWidth( ) - TB_NBUTTONS*40 + 8)/2;
	for( int i = 0; i < TB_NBUTTONS; i++ ) 
		m_button[i]->setPosition( width+40*i, height );
}