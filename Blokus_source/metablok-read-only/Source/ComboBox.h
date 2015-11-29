/* ===========================================================================

	Project: DirectX Engine - Combo Box

	Description:
	 Provides an implementation of a simple combo box control.

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
#ifndef DX_GUI_COMBOBOX
#define DX_GUI_COMBOBOX

// Engine namespace
namespace DirectX {
namespace GUI {

// Combo box control class
class ComboBox : public Control
{
public:
	enum ComboBoxState { HOVER_ARROW = 0x20, PRESSED_ARROW = 0x40, OPEN = 0x80 };

	// Construction parameters
	static ComboBox& create( WindowStyle* style, Panel* panel ); 
	ComboBox& pos( int x, int y ) { m_posX = x; m_posY = y; return *this; }
	ComboBox& size( int sx, int sy ) { m_sizeX = sx; m_sizeY = sy; return *this; }
	ComboBox& depth( int depth ) { m_depth = depth; return *this; }
	ComboBox& hidden( ) { m_state |= HIDDEN; return *this; }

	// Combo box list modifiers
	void clear( ) { m_items.clear( ); }
	void addItem( const wchar_t* str, int key );
	void removeItem( const wchar_t* str );
	void removeItem( int key );

	// Selected item modifier / accessor
	int getItem( ) { return m_items[0].key; }
	void setItem( int key );

	// Returns selected item by text
	const wchar_t* getItemText( ) 
		{ return m_items[0].text.c_str( ); }

	// Opens/Closes the ComboBox
	void close( ) { m_state &= (~OPEN); }
	void open( ) { m_state |= OPEN; }

	// Override containment
	bool contains( const POINT* pt );

	// Overriden control methods
	void render( LPD3DXSPRITE d3dsprite, const POINT* pos );
	void onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam );
	void onLoseFocus( );

private:
	// Default Constructor
	ComboBox( ) { }

	// Combo box item structure
	struct Item{ Item( const wchar_t* str, int id )
		{ text = std::wstring( str ); key = id; }
		std::wstring text; int key; };

	// Sorted list of combo box items
	std::vector<Item> m_items;
};

}}

// End definition
#endif