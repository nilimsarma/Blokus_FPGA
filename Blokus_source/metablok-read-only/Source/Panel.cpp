/* ===========================================================================

	Project: DirectX Engine GUI - Panel

	Description:
	 Defines the abstract base class for GUI Panels. Panels are controls
	 which can host other controls internally such as windows. Panels 
	 must implement additional methods for managing their hosted controls.

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
#include "Panel.h"

// --------------------------------------------------------
//	Checks for a mouse button release and gives focus to
//  the parent panel when the click is completed
// --------------------------------------------------------
void DirectX::GUI::Panel::onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
	// Get relative mouse position
	DirectX::Manager* manager = DirectX::Manager::instance( );
	
	// Acquire relative coordinates of click within this panel
	POINT mousePos; mousePos.x = LOWORD( lParam ); mousePos.y = HIWORD( lParam );
	mousePos.x -= m_posX; mousePos.y -= m_posY;

	// Construct new lparam for child control messages
	LPARAM cParam = MAKELPARAM( mousePos.x, mousePos.y );

	// Acquire the new moused over control
	if( message == WM_MOUSEMOVE )
	{
		// Search active controls for topmost moused over control
		GUI::Control* updateMoused = NULL; int mousedDepth = INT_MAX;
		for( std::list<GUI::Control*>::reverse_iterator i = m_controls.rbegin( ); 
			i != m_controls.rend( ); i++ ) if( (*i)->contains(&mousePos) )
				{ updateMoused = *i; break; }

		// Check for moused over change
		if( updateMoused != m_mousedOverControl )
		{
			if( m_mousedOverControl ) m_mousedOverControl->onMouseLeave( );
			if( updateMoused ) updateMoused->onMouseEnter( );
			m_mousedOverControl = updateMoused;
		}
	}

	// Check if the mouse is over the focused control
	bool overFocused = (m_mousedOverControl == m_focusedControl);

	// Acquire the new focused control
	if( (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN) && !overFocused )
	{	
		if( m_focusedControl ) m_focusedControl->onLoseFocus( );
		if( m_mousedOverControl ) m_mousedOverControl->onGainFocus( );
		m_focusedControl = m_mousedOverControl; overFocused = true;
	}

	// Pass the message to the relevant controls
	if( m_focusedControl ) m_focusedControl->onMouseEvent( message, wParam, cParam );
	if( m_mousedOverControl && !overFocused ) 
		m_mousedOverControl->onMouseEvent( message, wParam, cParam );
}
//
// --------------------------------------------------------
//  Sends the mouse leave message to the moused over child.
// --------------------------------------------------------
void DirectX::GUI::Panel::onMouseLeave( ) 
{ 
	m_state ^= MOUSED_OVER; 
	if( m_mousedOverControl )
	{
		m_mousedOverControl->onMouseLeave( );
		m_mousedOverControl = NULL;
	}
}
//
// --------------------------------------------------------
//  Releases all child controls belonging to the panel.
// --------------------------------------------------------
void DirectX::GUI::Panel::clear( )
{
	// Delete all child controls
	while( !m_controls.empty( ) )
		delete m_controls.front( );
}
//
// --------------------------------------------------------
//	Passes the relevant keyboard event to the focused
//  child control. 
// --------------------------------------------------------
void DirectX::GUI::Panel::onKeyboardEvent( UINT message, WPARAM wParam, LPARAM lParam ) 
{ 
	// Pass the message to the focused control
	if( m_focusedControl ) m_focusedControl->onKeyboardEvent( message, wParam, lParam );
}
//
// --------------------------------------------------------
//	Adds a new control to the panel.
// --------------------------------------------------------
void DirectX::GUI::Panel::addControl( DirectX::GUI::Control* control )
{ 
	std::list<Control*>::iterator i = m_controls.begin( );
	while( i != m_controls.end( ) && control->getDepth( ) >= (*i)->getDepth( ) ) i++;
	m_controls.insert( i, control );
}
//
// --------------------------------------------------------
//	Removes a control from the panel.
// --------------------------------------------------------
void DirectX::GUI::Panel::deleteControl( DirectX::GUI::Control *control )
{ 
	if( m_controls.empty( ) ) return;
	m_controls.remove( control ); 
	if( m_mousedOverControl == control ) m_mousedOverControl = NULL;
	if( m_focusedControl == control ) m_focusedControl = NULL;
}
//
// --------------------------------------------------------
//	Renders any visible child controls.
// --------------------------------------------------------
void DirectX::GUI::Panel::render( LPD3DXSPRITE d3dsprite, const POINT* pos )
{
	// Render child controls based on depth ordering and visibility
	POINT position = { pos->x+m_posX, pos->y+m_posY }; 
	for( std::list<Control*>::iterator i = m_controls.begin( );
		i != m_controls.end( ); i++ ) if( !(*i)->isHidden( ) )
		(*i)->render( d3dsprite, &position );
}
//
// --------------------------------------------------------
//  Sends the lost focus message to the child..
// --------------------------------------------------------
void DirectX::GUI::Panel::onLoseFocus( ) 
{ 
	m_state ^= FOCUSED; 
	if( m_focusedControl )
	{
		m_focusedControl->onLoseFocus( );
		m_focusedControl = NULL;
	}
}