/* ===========================================================================

	Project: DirectX Engine - EditBox

	Description:
	 Provides an implementation of a simple edit box control.

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
#ifndef DX_GUI_EDITBOX
#define DX_GUI_EDITBOX

// Engine namespace
namespace DirectX {
namespace GUI {

// Window Control Class
class EditBox : public Control
{
public:
	// Construction parameters
	static EditBox& create( WindowStyle* style, Panel* panel ); 
	EditBox& pos( int x, int y ) { m_posX = x; m_posY = y; return *this; }
	EditBox& size( int sx, int sy ) { m_sizeX = sx; m_sizeY = sy; return *this; }
	EditBox& depth( int depth ) { m_depth = depth; return *this; }
	EditBox& text( const wchar_t* text ) { m_text = text; return *this; }
	EditBox& align( int alignment ) { m_alignment = alignment; return *this; }
	EditBox& hidden( ) { m_state |= HIDDEN; return *this; }

	// List modifiers
	const wchar_t* getText( ) { return m_text.c_str( ); }
	void setText( const wchar_t* text ) { m_text = text; }

	// Keyboard input reading
	void onKeyboardEvent( UINT message, WPARAM wParam, LPARAM lParam );

	// Renders the edit box
	void render( LPD3DXSPRITE d3dsprite, const POINT* pos );

private:
	// Default constructor
	EditBox( ) : m_text(L""), m_alignment( Center|VCenter ) { }

	// Internal text and cursor iterator
	std::wstring::iterator m_cursorPosition;
	std::wstring m_text;

	// Edit box formatting specs
	unsigned int m_charWidth;
	unsigned int m_maxChars;
	int m_alignment;
};

}}

// End definition
#endif