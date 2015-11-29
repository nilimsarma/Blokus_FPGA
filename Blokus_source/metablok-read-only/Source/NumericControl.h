/* ===========================================================================

	Project: DirectX Engine - Numeric Control

	Description:
	 Simple control which hooks to a variable and can increase or decrease
	 the variables value in specified increments.

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
#ifndef DX_GUI_NUMERIC_CONTROL
#define DX_GUI_NUMERIC_CONTROL

// Engine namespace
namespace DirectX {
namespace GUI {

// Window Control Class
template <class T>
class NumericControl : public Control
{
public:
	enum NumericState { HOVER_DOWN = 0x20, HOVER_UP = 0x40, PRESS_DOWN = 0x80, PRESS_UP = 0x100 };
	
	// Construction parameters
	static NumericControl<T>& create( WindowStyle* style, Panel* panel ); 
	NumericControl<T>& pos( int x, int y ) { m_posX = x; m_posY = y; return *this; }
	NumericControl<T>& size( int sx, int sy ) { m_sizeX = sx; m_sizeY = sy; return *this; }
	NumericControl<T>& align( int alignment ) { m_alignment = alignment; return *this; }
	NumericControl<T>& depth( int depth ) { m_depth = depth; return *this; }
	NumericControl<T>& set( const T& value ) { m_value = value; return *this; }
	NumericControl<T>& step( const T& step ) { m_step = step; return *this; }
	NumericControl<T>& min( const T& value ) { m_min = value; return *this; }
	NumericControl<T>& max( const T& value ) { m_max = value; return *this; }
	NumericControl<T>& hook( T* hook ) { m_hook = hook; return *this; }
	NumericControl<T>& hidden( ) { m_state |= HIDDEN; return *this; }

	// Rehook the indicator
	const T* getHook( ) { return &m_hook; }
	void rehook( T* hook ) { m_hook = hook; }
	void unhook( ) { m_hook = NULL; }

	// Get the hooked or internal control value
	const T& getValue( ) { return m_hook ? *m_hook : m_value; }

	// Mouse event callback override
	void onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam );

	// Focus/Mouse loss
	void onLoseFocus( ) { m_state&=(~PRESS_UP); m_state&=(~PRESS_DOWN); m_state&=(~FOCUSED); }
	void onMouseLeave( ) { m_state&=(~HOVER_UP); m_state&=(~HOVER_DOWN); m_state&=(~MOUSED_OVER); }

	// Render method
	void render( LPD3DXSPRITE d3dsprite, const POINT* pos );

private:
	// Default Constructor
	NumericControl( ) : m_hook(NULL) { } 

	// Control bounds and iterators
	T *m_hook, m_value, m_min, m_max, m_step;
};

}}

// --------------------------------------------------------
//	Creates a new numerical control.
// --------------------------------------------------------
template <class T>
DirectX::GUI::NumericControl<T>& DirectX::GUI::NumericControl<T>::
	create( WindowStyle* style, Panel* panel )
{
	NumericControl<T>* control = new NumericControl<T>( );
	control->m_style = style; 
	control->m_panel = panel;
	panel->addControl( control ); 
	return *control;
}
//
// --------------------------------------------------------
//	Creates a new numerical control using the passed in 
//  window style
// --------------------------------------------------------
template <class T>
void DirectX::GUI::NumericControl<T>::onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
	// Get mouse coordinates from message
	int mouseX = LOWORD( lParam );
	int mouseY = HIWORD( lParam );

	// Check msg type
	switch( message )
	{
		// Update mouse-over data
		case WM_MOUSEMOVE:
			if( m_state&MOUSED_OVER )
				if( mouseX > m_posX-m_sizeY/2 )
				{
					if( mouseY < m_posY + m_sizeY/2 ) { m_state|=HOVER_UP; m_state&=(~HOVER_DOWN); }
					else { m_state|=HOVER_DOWN; m_state&=(~HOVER_UP); }
				}
			else { m_state&=(~HOVER_DOWN); m_state&=(~HOVER_UP); }
			break;

		// Check for button press
		case WM_LBUTTONDOWN:
			if( m_state&HOVER_DOWN ) 
			{
				m_state |= PRESS_DOWN; 
				if( m_hook ) { *m_hook -= m_step; if( *m_hook < m_min ) *m_hook = m_min; }
				else { m_value -= m_step; if( m_value < m_min ) m_value = m_min; }
			}
 			else if( m_state&HOVER_UP ) 
			{ 
				m_state |= PRESS_UP; 
				if( m_hook ) { *m_hook += m_step; if( *m_hook > m_max ) *m_hook = m_max; }
				else { m_value += m_step; if( m_value > m_max ) m_value = m_max; }
			}
			break;

		// Release any buttons
		case WM_LBUTTONUP:
			m_state&=(~PRESS_DOWN);
			m_state&=(~PRESS_UP);
			break;
	}
}
																																						//
// --------------------------------------------------------
//	Draws the hooked value over top of the style standard
//  text backdrop
// --------------------------------------------------------
template <class T>
void DirectX::GUI::NumericControl<T>::render( LPD3DXSPRITE d3dsprite, const POINT* pos ) 
{
	Matrix transform;
	
	// Compute absolute offset 
	int xpos = m_posX + pos->x; 
	int ypos = m_posY + pos->y;

	// Create display bound rectangle
	RECT displayRect = { xpos, ypos, xpos+m_sizeX-m_sizeY/2, ypos+m_sizeY };

	// Initialize backdrop transform to perform proper scaling and offset 
	float fieldSizeX = (float)(m_style->selectionBox.right - m_style->selectionBox.left);
	float fieldSizeY = (float)(m_style->selectionBox.bottom - m_style->selectionBox.top);
	Vector2 scaling( (float)(m_sizeX-m_sizeY/2) / fieldSizeX, (float)m_sizeY / fieldSizeY);
	Vector2 position( (float)xpos, (float)ypos );
	D3DXMatrixTransformation2D( &transform, NULL, 
		0.0, &scaling, NULL, NULL, &position );

	// Render the backdrop under the transform
	d3dsprite->SetTransform( &transform );
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		&m_style->selectionBox, NULL, NULL, 0xFFFFFFFF );

	// Render the up/down arrow sprites

	// Draw value string
	std::wstringstream ss; ss << ( (m_hook!=NULL) ? *m_hook : m_value );
	D3DXMatrixIdentity( &transform ); d3dsprite->SetTransform( &transform );
	m_style->font.getFont( )->DrawTextW( d3dsprite, ss.str( ).c_str( ), -1, 
		&displayRect, DT_NOCLIP | VCenter | Center, m_style->color );
}

																																						//
// End definition
#endif