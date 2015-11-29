/* ===========================================================================

	Project: DirectX Engine - Button

	Description:
	 Provides an implementation of a simple button control.

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
#ifndef DX_GUI_BUTTON
#define DX_GUI_BUTTON

// Engine namespace
namespace DirectX {
namespace GUI {

// Disjoint Tree Class
class Button : public Control
{
public:
	// Construction parameters
	static Button& create( WindowStyle* style, Panel* panel ); 
	Button& pos( int x, int y ) { m_posX = x; m_posY = y; return *this; }
	Button& size( int sx, int sy ) { m_sizeX = sx; m_sizeY = sy; return *this; }
	Button& depth( int depth ) { m_depth = depth; return *this; }
	Button& text( const wchar_t* text ) { m_str = text; return *this; }
	Button& info( const wchar_t* text ) { m_hoverStr = text; return *this; }
	Button& align( int alignment ) { m_alignment = alignment; return *this; }
	Button& graphic( DirectX::Image* label, RECT* rect )
		{ m_label = label; m_labelRect = *rect; return *this; }
	Button& hidden( ) { m_state |= HIDDEN; return *this; }

	// Overridden message event callbacks
	void onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam );
	void render( LPD3DXSPRITE d3dsprite, const POINT* pos );

private:
	// Default constructor
	Button( ) : m_alignment(Center|VCenter), 
		m_label(NULL) { }

	// Graphic label
	DirectX::Image* m_label;
	RECT m_labelRect;

	// Button text
	int m_alignment;
	std::wstring m_hoverStr;
	std::wstring m_str;
};

}}

// End definition
#endif