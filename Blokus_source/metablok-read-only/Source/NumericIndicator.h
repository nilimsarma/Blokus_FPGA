/* ===========================================================================

	Project: DirectX Engine - Numeric Indicator

	Description:
	 Simple control which displays the value of a hooked variable.

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

// Include Engine 
#include "DirectX.h"

// Begin definition
#ifndef DX_GUI_NUMERIC_INDICATOR
#define DX_GUI_NUMERIC_INDICATOR

// Engine namespace
namespace DirectX {
namespace GUI {

// Numeric Indicator Control Class
template <class T> class NumericIndicator : public Control
{
public:
	// Construction parameters
	static NumericIndicator<T>& create( WindowStyle* style, Panel* panel ); 
	NumericIndicator<T>& pos( int x, int y ) { m_posX = x; m_posY = y; return *this; }
	NumericIndicator<T>& size( int sx, int sy ) { m_sizeX = sx; m_sizeY = sy; return *this; }
	NumericIndicator<T>& align( int alignment ) { m_alignment = alignment; return *this; }
	NumericIndicator<T>& depth( int depth ) { m_depth = depth; return *this; }
	NumericIndicator<T>& hook( T* hook ) { m_hook = hook; return *this; }
	NumericIndicator<T>& hidden( ) { m_state |= HIDDEN; return *this; }

	// Rehooking the indicator
	const T* getHook( ) { return m_hook; }
	void setHook( T* hook ) { m_hook = hook; }

	// Make unfocusable :TODO: Focus but immediately giveFocus
	// to prevent mouse interaction with whats underneath this
	bool contains( const POINT* pt ) { return false; }

	// Overridden render method
	void render( LPD3DXSPRITE d3dsprite, const POINT* pos );

private:
	// Default Constructor
	NumericIndicator( ) : m_hook(NULL) { } 

	T* m_hook, m_value;
	int m_alignment;
};

}}

// --------------------------------------------------------
//	Creates a new numerical indicator.
// --------------------------------------------------------
template <class T>
DirectX::GUI::NumericIndicator<T>& DirectX::GUI::NumericIndicator<T>::
	create( WindowStyle* style, Panel* panel )
{
	NumericIndicator<T>* indicator = new NumericIndicator<T>( );
	indicator->m_style = style; 
	indicator->m_panel = panel;
	panel->addControl( indicator ); 
	return *indicator;
}
//
// --------------------------------------------------------
//	Draws the hooked value over top of the style standard
//  text backdrop.
// --------------------------------------------------------
template <class T> 
void DirectX::GUI::NumericIndicator<T>::render( LPD3DXSPRITE d3dsprite, const POINT* pos ) 
{
	Matrix transform;

	// Compute absolute offset 
	int xpos = m_posX + pos->x; 
	int ypos = m_posY + pos->y;

	// Initialize backdrop transform to perform proper scaling and offset 
	float fieldSizeX = (float)(m_style->selectionBox.right - m_style->selectionBox.left);
	float fieldSizeY = (float)(m_style->selectionBox.bottom - m_style->selectionBox.top);
	Vector2 scaling( (float)m_sizeX / fieldSizeX, (float)m_sizeY / fieldSizeY);
	Vector2 position( (float)xpos, (float)ypos );
	D3DXMatrixTransformation2D( &transform, NULL, 
		0.0, &scaling, NULL, NULL, &position );

	// Render the sprite under the transform
	d3dsprite->SetTransform( &transform );
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		&m_style->selectionBox, NULL, NULL, 0xFFFFFFFF );

	// Draw value string
	std::wstringstream ss; ss << *m_hook;
	RECT displayRect = { xpos, ypos, xpos+m_sizeX, ypos+m_sizeY };
	D3DXMatrixIdentity( &transform ); d3dsprite->SetTransform( &transform );
	m_style->font.getFont( )->DrawTextW( d3dsprite, ss.str( ).c_str( ), -1, 
		&displayRect, DT_NOCLIP | VCenter | Center, m_style->color );
}

// End definition
#endif