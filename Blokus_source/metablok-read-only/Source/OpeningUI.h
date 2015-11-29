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

// Begin definition
#ifndef OPENING_UI_H
#define OPENING_UI_H

// Match prototype
class Match;

// Match game state
class OpeningUI : public DirectX::GameState
{

public:
	// Initialize panel pointers to null
	OpeningUI( ) : m_displayPanel(NULL), m_graphWin(NULL), m_quitWin(NULL),
		m_matchWin(NULL), m_playWin(NULL), m_loadWin(NULL), m_newWin(NULL),
		m_saveWin(NULL), m_saveFilename(L"") { }

	// Primary methods
	void startup( ); void shutdown( );
	void main( ) { m_match.main( ); }

	// Overridden controller callbacks
	void onGuiEvent( DirectX::GUI::Control* control, unsigned int message, void* data );
	void onKeyboardEvent( UINT message, WPARAM wParam, LPARAM lParam );
	void onDeviceReset( );

private:
	// Match instance
	Match m_match;

	// GUI dynamic create functions
	void createGraphicsWindow( );
	void createNewMatchWindow( );
	void createPlayerWindow( );
	void createMatchWindow( );
	void createSaveWindow( );
	void createLoadWindow( );
	void createQuitWindow( );
	void createToolbar( );

	// Blokus user interface display panel
	DirectX::GUI::WindowStyle* m_style;
	DirectX::GUI::DisplayPanel* m_displayPanel;
	DirectX::Manager* m_manager;

	// Primary command toolbar
	DirectX::Image m_spriteSheet;
	DirectX::GUI::Button* m_button[13];

	// Graphics settings window
	DirectX::GUI::Window* m_graphWin;
	DirectX::GUI::Button* m_graphApplyButton;
	DirectX::GUI::Button* m_graphCancelButton;
	DirectX::GUI::Button* m_graphAcceptButton;
	DirectX::GUI::Checkbox* m_graphVSync;
	DirectX::GUI::Checkbox* m_graphMode;
	DirectX::GUI::ComboBox* m_graphDisp;
	void applyGraphicsSettings( );

	// Player selection window
	DirectX::GUI::Window* m_playWin;
	DirectX::GUI::ComboBox* m_playSel[4];
	DirectX::GUI::Button* m_playCancelButton;
	DirectX::GUI::Button* m_playAcceptButton;
	void applyPlayers( );

	// New match window
	DirectX::GUI::Window* m_newWin;
	DirectX::GUI::Button* m_newAcceptButton;
	DirectX::GUI::Button* m_newCancelButton;
		
	// Quit match window
	DirectX::GUI::Window* m_quitWin;
	DirectX::GUI::Button* m_quitAcceptButton;
	DirectX::GUI::Button* m_quitCancelButton;

	// Match settings window
	DirectX::GUI::Window* m_matchWin;
	DirectX::GUI::Button* m_matchAcceptButton;
	DirectX::GUI::Button* m_matchCancelButton;
	//DirectX::GUI::Radio* m_matchMode;

	// Load file window
	DirectX::GUI::Window* m_loadWin;
	DirectX::GUI::Button* m_loadAcceptButton;
	DirectX::GUI::Button* m_loadCancelButton;
	DirectX::GUI::Button* m_loadDeleteButton;
	DirectX::GUI::ListBox* m_loadMatches;
	void load( );

	// Save file window
	DirectX::GUI::Window* m_saveWin;
	DirectX::GUI::Button* m_saveAcceptButton;
	DirectX::GUI::Button* m_saveCancelButton;
	DirectX::GUI::EditBox* m_saveName;
	std::wstring m_saveFilename;
	void quickSave( );
};

// End definition
#endif