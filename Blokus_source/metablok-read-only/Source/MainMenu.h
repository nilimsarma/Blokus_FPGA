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

// Begin definition
#ifndef MAIN_MENU_H
#define MAIN_MENU_H

// Splash screen
class MainMenu : public DirectX::GameState
{
public:
	// Privatized Construction
	MainMenu( ) { m_state = WAITING; }

	// Primary methods
	void startup( );
	void shutdown( );

	// DirectX controller functions
	void onKeyboardEvent( UINT message, WPARAM wParam, LPARAM lParam ) { }
	void onGuiEvent( DirectX::GUI::Control* control, unsigned int message, void* data );
	void onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam ) { }
	void onDeviceReset( );

	// Message passing interface
	void messagePump( UINT message, WPARAM wParam, LPARAM lParam );

private:
	// User interface display panel
	DirectX::GUI::WindowStyle* m_style;
	DirectX::GUI::DisplayPanel* m_displayPanel;
	DirectX::Manager* m_manager;

	// Backdrop graphic
	DirectX::Image m_backdropImg;
	DirectX::GUI::Graphic* m_backdrop;

	// Primary menu buttons
	DirectX::GUI::Button* m_buttons[6];
};

// End definition
#endif