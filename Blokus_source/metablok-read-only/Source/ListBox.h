/* ===========================================================================

	Project: DirectX Engine - List Box

	Description:
	 Provides an implementation of a simple list box control with scrolling
	 capabilities.

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
#ifndef DX_GUI_LISTBOX
#define DX_GUI_LISTBOX

// Engine namespace
namespace DirectX {
namespace GUI {

// List box control class
class ListBox : public Control
{
public:
	enum ListBoxState { HOVER_UP = 0x40, HOVER_DOWN = 0x80, PRESS_UP = 0x100, PRESS_DOWN = 0x200, SCROLLING = 0x400, MULTI_ITEM = 0x800 };

	// Construction parameters
	static ListBox& create( WindowStyle* style, Panel* panel ); 
	ListBox& pos( int x, int y ) { m_posX = x; m_posY = y; return *this; }
	ListBox& size( int sx, int sy ) { m_sizeX = sx; m_sizeY = sy; return *this; }
	ListBox& spacing( int space ) { m_spacing = space; return *this; }
	ListBox& depth( int depth ) { m_depth = depth; return *this; }
	ListBox& multi( ) { m_state |= MULTI_ITEM; return *this; }
	ListBox& hidden( ) { m_state |= HIDDEN; return *this; }

	// List box list modifiers
	void clear( ) { m_items.clear( ); }
	void addItem( const wchar_t* str, int key, 
		bool selected = false );
	void removeItem( const wchar_t* str );
	void removeItem( int key );

	// Selected item modifier / accessor
	const wchar_t* getString( int key );
	int getItem( ); int getNextItem( );
	int getLastSelected( ) { return m_selected; }
	void deselectItem( int key );
	void selectItem( int key );
	bool isSelected( int key );
	void deselectAll( );

	// Focus/Mouse loss 
	void onLoseFocus( ) { m_state&=(~PRESS_UP); m_state&=(~PRESS_DOWN); 
		m_state&=(~FOCUSED); m_state&=(~SCROLLING); }
	void onMouseLeave( ) { m_state&=(~HOVER_UP); m_state&=(~HOVER_DOWN); 
		m_state&=(~MOUSED_OVER); }

	// Overriden control event methods
	void onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam );
	void render( LPD3DXSPRITE d3dsprite, const POINT* pos );

private:
	// Default Constructor
	ListBox( ) : m_topItemIndex(0), m_spacing(24), m_selected(0) { }

	// Structure of list item
	struct Item{ Item( const wchar_t* str, int id, bool sel = false )
		{ text = std::wstring( str ); key = id; selected = sel; }
		std::wstring text; int key; bool selected; };

	// Search helper function
	int indexOf( const wchar_t* str );
	int indexOf( int key );

	// List information
	std::vector<Item> m_items;
	int m_topItemIndex;
	int m_selected;
	int m_spacing; 
};

}}

// End definition
#endif