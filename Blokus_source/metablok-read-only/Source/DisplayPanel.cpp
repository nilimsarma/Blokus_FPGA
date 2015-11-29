/* ===========================================================================

	Project: DirectX Engine - Display Panel

	Description:
	 Provides an implementation of a special panel which has no parent
	 panel. Display Panel callbacks are called directly by the DirectX 
	 Manager when objects at its depth are being rendered. Aside from this,
	 Display Panels have all the characteristics of a normal panel control.

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
#include "DirectX.h"

// Include header
#include "DisplayPanel.h"

// --------------------------------------------------------
//	Registers the display panel with the engine manager.
// --------------------------------------------------------
DirectX::GUI::DisplayPanel& DirectX::GUI::DisplayPanel::create( )
{
	// Register a new display panel with the manager
	DisplayPanel* panel = new DisplayPanel( );
	DirectX::Manager::instance( )->addDisplayPanel( panel );
	return *panel;
}
//
// --------------------------------------------------------
//	Removes the display panel from the engine manager.
// --------------------------------------------------------
DirectX::GUI::DisplayPanel::~DisplayPanel( )
{
	// Remove from manager
	DirectX::Manager::instance( )->deleteDisplayPanel( this ); 
}
//
// --------------------------------------------------------
//  Returns true if the DisplayPanel has any child panels
//  which contain the specified point.
// --------------------------------------------------------
bool DirectX::GUI::DisplayPanel::contains( const POINT* pt )
{
	// Check for containment for any child panel
	for( std::list<Control*>::iterator i = m_controls.begin( );
		i != m_controls.end( ); i++ ) if( !(*i)->isHidden( ) )
		if( (*i)->contains( pt ) ) return true;

	return false;
}
//
// --------------------------------------------------------
//  Returns focus to the DirectX manager.
// --------------------------------------------------------
void DirectX::GUI::DisplayPanel::giveFocus( )
{ 
	// Defocus the focused control
	m_focusedControl->onLoseFocus( );
	m_focusedControl = NULL; 

	// Return control to manager
	DirectX::Manager::instance( )->giveFocus( );
}
