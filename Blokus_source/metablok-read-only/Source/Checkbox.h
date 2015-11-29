/* ===========================================================================

	Project: DirectX Engine - Checkbox

	Description:
	 Provides an implementation of a simple checkbox control.

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
#ifndef DX_GUI_CHECKBOX
#define DX_GUI_CHECKBOX

// Engine namespace
namespace DirectX {
namespace GUI {

// Checkbox GUI control
class Checkbox : public Control
{
public:
	enum CheckboxState { CHECKED = 0x20 };

	// Construction parameters
	static Checkbox& create( WindowStyle* style, Panel* panel ); 
	Checkbox& pos( int x, int y ) { m_posX = x; m_posY = y; return *this; }
	Checkbox& size( int sx, int sy ) { m_sizeX = sx; m_sizeY = sy; return *this; }
	Checkbox& depth( int depth ) { m_depth = depth; return *this; }
	Checkbox& checked( ) { m_state |= CHECKED; return *this; }
	Checkbox& hidden( ) { m_state |= HIDDEN; return *this; }

	// Check state accessor
	int isChecked( ) { return m_state&CHECKED; }

	// Overridden message event callbacks
	void onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam );
	void render( LPD3DXSPRITE d3dsprite, const POINT* pos );

private:
	// Default Constructor
	Checkbox( ) { }
};

}}

// End definition
#endif