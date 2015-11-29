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

// Include Engine 
#include "DirectX.h"

// Include Header
#include "ComboBox.h"

// --------------------------------------------------------
//	Creates a new combo box control.
// --------------------------------------------------------
DirectX::GUI::ComboBox& DirectX::GUI::ComboBox::
	create( WindowStyle* style, Panel* panel )
{
	ComboBox* comboBox = new ComboBox( );
	comboBox->m_style = style; 
	comboBox->m_panel = panel; 
	panel->addControl( comboBox ); 
	return *comboBox;
}
//
// --------------------------------------------------------
//	Adds a new selectable item to the combo box.
// --------------------------------------------------------
void DirectX::GUI::ComboBox::addItem( const wchar_t *str, int key )
{
	// Push the item on the back of the list
	m_items.push_back( Item( str, key ) );
}
//
// --------------------------------------------------------
//	Handle comboBox opening, closing, and selection
// --------------------------------------------------------
void DirectX::GUI::ComboBox::onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
	// Extract mouse coordinates
	POINT pt = { LOWORD(lParam)-m_posX, 
				 HIWORD(lParam)-m_posY };

	// Check if the mouse has entered the button region
	if( message == WM_MOUSEMOVE )
	{
		// Handle mouse-over arrow sprite changes
		if( (m_state&MOUSED_OVER) && 
			pt.x > m_sizeX-m_sizeY ) 
			m_state |= HOVER_ARROW;
		else m_state &= (~HOVER_ARROW);

		return;
	}

	// Return the button to its original state when
	// the left mouse is released and the button is pressed
	if( message == WM_LBUTTONUP && (m_state&PRESSED_ARROW) ) 
	{
		m_state ^= PRESSED_ARROW;
		if( m_state&HOVER_ARROW ) 
			if( m_state&OPEN ) close( );
			else open( );

		return;
	}

	// Set the sprite to the down sprite on focus gained
	// by a left mouse button press
	else if( message == WM_LBUTTONDOWN )
		if( m_state&HOVER_ARROW )
			m_state |= PRESSED_ARROW;
		else 
		{
			// Toggle open/closed
			if( m_state&OPEN ) 
			{	
				close( );
				
				// Get index of new selected element
				int i = pt.y / m_sizeY;
				if( i == 0 ) return;

				// Swap keys
				int key = m_items[i].key;
				m_items[i].key = m_items[0].key;
				m_items[0].key = key;

				// Swap text strings
				std::wstring temp = m_items[i].text;
				m_items[i].text = m_items[0].text;
				m_items[0].text = temp;

				// Throw selection message
				DirectX::Manager::instance( )->pumpMessage
					( this, CM_COMBO_BOX_SELECTION, &key );
			}
			else open( );
		}
}
//
// --------------------------------------------------------
//	Sets the currently selected item in the ComboBox.
// --------------------------------------------------------
void DirectX::GUI::ComboBox::setItem( int key )
{
	// Search array for keyed element
	for( int i = 0; i < m_items.size( ); i++ )
		if( m_items[i].key == key )
		{
			// Swap item keys
			m_items[i].key = m_items[0].key;
			m_items[0].key = key;

			// Swap text strings
			std::wstring temp = m_items[i].text;
			m_items[i].text = m_items[0].text;
			m_items[0].text = temp;
		}
}
//
// --------------------------------------------------------
//	Closes the combo box when it loses focus.
// --------------------------------------------------------
void DirectX::GUI::ComboBox::onLoseFocus( ) 
{ 
	m_state ^= FOCUSED; 
	close( );
}
//
// --------------------------------------------------------
//	Extends containment to the selection box field.
// --------------------------------------------------------
bool DirectX::GUI::ComboBox::contains( const POINT* pt )
{
	// Standard containment
	if( Control::contains( pt ) ) return true;
	
	// Open selection field containment
	if( (m_state&OPEN) && pt->x > m_posX && pt->x < m_posX+m_sizeX-m_sizeY &&
		pt->y > m_posY && pt->y < m_posY+m_sizeY*m_items.size( ) )
		return true;

	return false;
}
//
// --------------------------------------------------------
//	Renders the combo box control.
// --------------------------------------------------------
void DirectX::GUI::ComboBox::render( LPD3DXSPRITE d3dsprite, const POINT* pos )
{
	Matrix transform;
	
	// Compute absolute offset 
	int xpos = m_posX + pos->x; 
	int ypos = m_posY + pos->y;

	// Combo box field dimensions
	float fieldSizeX = (float)(m_style->selectionBox.right - m_style->selectionBox.left);
	float fieldSizeY = (float)(m_style->selectionBox.bottom - m_style->selectionBox.top);

	// Draw the selection box background	
	int nItems = (m_state&OPEN) ? m_items.size( ) : 1; 
	Vector2 fieldScale( (float)(m_sizeX-m_sizeY) / 
		fieldSizeX, (float)m_sizeY*nItems / fieldSizeY );
	D3DXMatrixTransformation2D( &transform, NULL, 0.0, &fieldScale, NULL, 
		NULL, &Vector2( (float)xpos, (float)ypos ) );
	d3dsprite->SetTransform( &transform );
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		&m_style->selectionBox, NULL, NULL, 0xFFFFFFFF );

	// Render backdrop
	if( m_state&OPEN )
	{
		// Get index of hovered item
		int my = DirectX::Manager::instance( )->getMouseY( );
		int hoverItem =  (my - ypos) / m_sizeY;

		// Check for valid item index
		if( hoverItem >= 0 && hoverItem < m_items.size( ) )
		{
			transform._42 = (float)ypos+hoverItem*m_sizeY; 
			transform._22 = (float)m_sizeY / (float)fieldSizeY;
			d3dsprite->SetTransform( &transform );
			d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
				&m_style->backdrop, NULL, NULL, 0xFFFFFFFF );
		}
	}

	// Set transform for text rendering
	D3DXMatrixIdentity( &transform ); d3dsprite->SetTransform( &transform );
	RECT textRect = { xpos, ypos, xpos+m_sizeX-m_sizeY, ypos+m_sizeY };

	// Draw selected item text and other item text if open
	for( int i = 0; i < nItems; i++ ) 
	{
		// Render this items assosciated text
		m_style->font.getFont( )->DrawTextW( d3dsprite, m_items[i].text.c_str( ), 
			-1, &textRect, Center | VCenter, m_style->color );

		// Adjust text rectangle height
		textRect.top = textRect.bottom; 
		textRect.bottom += m_sizeY; 
	}

	// Compute arrow sprite image and transform
	int buttonSize = m_style->downArrow.right - m_style->downArrow.left;
	float arrowScale = (float)m_sizeY / (float)buttonSize;
	D3DXMatrixTransformation2D( &transform, NULL, 
		0.0, &Vector2( arrowScale, arrowScale ), 
		NULL, NULL, &Vector2( (float)(xpos+
		m_sizeX-m_sizeY), (float)ypos ) );

	LPRECT arrow; // Select arrow button sprite
	if( (m_state&PRESSED_ARROW) && (m_state&HOVER_ARROW) ) 
		arrow = &m_style->downArrowPressed;
	else if( (m_state&HOVER_ARROW) || (m_state&PRESSED_ARROW) ) 
		arrow = &m_style->downArrowHover;
	else arrow = &m_style->downArrow;

	// Render the arrow sprite
	d3dsprite->SetTransform( &transform );
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		arrow, NULL, NULL, 0xFFFFFFFF );
}