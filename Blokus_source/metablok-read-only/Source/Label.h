/* ===========================================================================

	Project: DirectX Engine - Label

	Description:
	 Provides an implementation of a simple label control.

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
#ifndef DX_GUI_LABEL
#define DX_GUI_LABEL

// Engine namespace
namespace DirectX {
namespace GUI {

// Label Control
class Label : public Control
{
public:
	enum LabelState { PRESSABLE = 0x20 };

	// Construction parameters
	static Label& create( WindowStyle* style, Panel* panel ); 
	Label& pos( int x, int y ) { m_posX = x; m_posY = y; return *this; }
	Label& size( int sx, int sy ) { m_sizeX = sx; m_sizeY = sy; return *this; }
	Label& text( const wchar_t* text ) { m_text = std::wstring(text); return *this; }
	Label& align( int alignment ) { m_alignment = alignment; return *this; }
	Label& depth( int depth ) { m_depth = depth; return *this; }
	Label& color( int color ) { m_color = color; return *this; }
	Label& pressable( ) { m_state |= PRESSABLE; return *this; }
	Label& hidden( ) { m_state |= HIDDEN; return *this; }

	// Press state accessor
	int isPressable( ) { return m_state&PRESSABLE; }

	// Restrict containment to pressables
	bool contains( const POINT* pt ) { 
		if( isPressable( ) ) 
			return Control::contains( pt );
		else return false; }

	// Render operation
	void render( LPD3DXSPRITE d3dsprite, const POINT* pos );

private:
	// Default Constructor
	Label( ) : m_alignment(Center|VCenter), 
		m_text(L"") { }

	unsigned long m_alignment;
	std::wstring m_text;
};

}}

// End definition
#endif