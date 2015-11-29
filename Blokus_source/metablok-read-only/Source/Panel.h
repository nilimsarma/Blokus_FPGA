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

// Begin definition
#ifndef DX_GUI_PANEL
#define DX_GUI_PANEL

// Engine namespace
namespace DirectX {
namespace GUI {

// GUI Panel class
class Panel : public Control
{
public:
	// Destructor - free controls
	virtual ~Panel( ) { clear( ); }

	// Control list management
	void addControl( Control* control );
	void deleteControl( Control *control );
	void updateControl( Control *control );
	void clear( );

	// Renders child controls
	virtual void render( LPD3DXSPRITE d3dsprite, const POINT* pos );

	// Overridden message event callbacks
	void onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam );
	void onKeyboardEvent( UINT message, WPARAM wParam, LPARAM lParam );
	
	// Lost focus/mouse
	virtual void onLoseFocus( );
	virtual void onMouseLeave( );

	// Return panel focus
	virtual void giveFocus( ) { 
		m_focusedControl->onLoseFocus( );
		m_focusedControl = NULL; }

protected:
	// Constructor - initialized controls
	Panel( ) : m_mousedOverControl(NULL), 
		m_focusedControl(NULL) { } 

	// Hosted control references
	std::list<Control*> m_controls;
	Control* m_mousedOverControl;
	Control* m_focusedControl;
};

}}

// End definition
#endif