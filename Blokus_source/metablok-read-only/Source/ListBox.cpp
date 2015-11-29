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

// Include Engine 
#include "DirectX.h"

// Include Header
#include "ListBox.h"

// --------------------------------------------------------
//	Creates a new list box control.
// --------------------------------------------------------
DirectX::GUI::ListBox& DirectX::GUI::ListBox::
	create( WindowStyle* style, Panel* panel )
{
	ListBox* listBox = new ListBox( );
	listBox->m_style = style; 
	listBox->m_panel = panel; 
	panel->addControl( listBox ); 
	return *listBox;
}
//
// --------------------------------------------------------
//	Adds a new selectable item to the list box.
// --------------------------------------------------------
void DirectX::GUI::ListBox::addItem( const wchar_t *str, int key, bool selected )
{
	// Push the item on the back of the list
	m_items.push_back( Item( str, key, selected ) );
}
//
// --------------------------------------------------------
//	Deselects any selected entries in the list box.
// --------------------------------------------------------
void DirectX::GUI::ListBox::deselectAll( )
{
	int j = m_items.size( );
	for( int i = 0; i < j; i++ )
		m_items[i].selected = false;
}
// 
// --------------------------------------------------------
//	Returns the list index of the list item with the 
//  specified key. 
// --------------------------------------------------------
const wchar_t* DirectX::GUI::ListBox::getString( int key )
{
	return m_items[indexOf(key)].text.c_str( );
}
//
// --------------------------------------------------------
//	Returns the list index of the list item with the 
//  specified key. 
// --------------------------------------------------------
int DirectX::GUI::ListBox::indexOf( int key )
{
	int j = m_items.size( );
	for( int i = 0; i < j; i++ )
		if( m_items[i].key == key )
			return i;
	return -1;
}
//
// --------------------------------------------------------
//	Returns the list index of the element with the 
//  specified string.
// --------------------------------------------------------
int DirectX::GUI::ListBox::indexOf( const wchar_t* str )
{
	int j = m_items.size( );
	for( int i = 0; i < j; i++ )
		if( m_items[i].text == str )
			return i;

	return -1;
}
//
// --------------------------------------------------------
//	Renders the ListBox control.
// --------------------------------------------------------
void DirectX::GUI::ListBox::render( LPD3DXSPRITE d3dsprite, const POINT* pos )
{
	// Compute absolute offset 
	int xpos = m_posX + pos->x; 
	int ypos = m_posY + pos->y;

	// Get default sprite sizes for scaling operations
	float fieldSizeX = (float)(m_style->selectionBox.right - m_style->selectionBox.left);
	float fieldSizeY = (float)(m_style->selectionBox.bottom - m_style->selectionBox.top);
	int scrollbarWidth = m_style->downArrow.right - m_style->downArrow.left;

	// Compute backdrop sprite transform
	Matrix transformField;
	float fieldScaleX = (float)(m_sizeX-scrollbarWidth) / fieldSizeX;
	float fieldScaleY = (float)m_sizeY / fieldSizeX;
	Vector2 scaling( fieldScaleX, fieldScaleY ), position( (float)xpos, (float)ypos );
	D3DXMatrixTransformation2D( &transformField, NULL, 
		0.0, &scaling, NULL, NULL, &position );

	// Draw the selection box background
	d3dsprite->SetTransform( &transformField );
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		&m_style->selectionBox, NULL, NULL, 0xFFFFFFFF );

	// Determine number of items in list box
	int nItemsToDraw = m_sizeY / m_spacing; 
	int limit = ( m_items.size( ) < m_topItemIndex+nItemsToDraw ) ? 
		m_items.size( ) : m_topItemIndex+nItemsToDraw;

	// Draw the highlights on selected items
	for( int i = m_topItemIndex; i < limit; i++ )
		if( m_items[i].selected )
		{
			// Compute highlight sprite transform
			Matrix transformBackdrop;
			float highlightSizeX = (float)(m_style->backdrop.right - m_style->backdrop.left);
			float highlightSizeY = (float)(m_style->backdrop.bottom - m_style->backdrop.top);
			float highlightScaleX = (float)(m_sizeX-scrollbarWidth) / highlightSizeX;
			float highlightScaleY = (float)m_spacing / highlightSizeY;
			Vector2 scaling( highlightScaleX, highlightScaleY );
			Vector2 position( (float)xpos, (float)(ypos+(i-m_topItemIndex)*m_spacing) );
			Matrix transform; D3DXMatrixTransformation2D( &transformBackdrop, NULL, 0.0, 
				&scaling, NULL, NULL, &position );

			// Draw the highlight sprite
			d3dsprite->SetTransform( &transformBackdrop ); 
			d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
				&m_style->backdrop, NULL, NULL, 0xFFFFFFFF );
		}

	// Draw text for visible list box entries
	Matrix matrix; D3DXMatrixIdentity( &matrix ); d3dsprite->SetTransform( &matrix );
	RECT textRect; SetRect( &textRect, xpos, ypos, xpos+m_sizeX-scrollbarWidth, ypos+m_spacing );
	for( int i = m_topItemIndex; i < limit; i++ )
	{
		// Render list item string
		m_style->font.getFont( )->DrawTextW( d3dsprite, 
			m_items[i].text.c_str( ), -1, &textRect, 
			Center | VCenter, m_style->color );

		// Adjust text rectangle for next element
		textRect.top = textRect.bottom; textRect.bottom += m_spacing;
	}

	// Compute arrow transforms
	Matrix transformUp, transformDw;
	float arrowSizeX = (float)(m_style->downArrow.right - m_style->downArrow.left);
	float arrowSizeY = (float)(m_style->downArrow.bottom - m_style->downArrow.top);
	Vector2 arrowScale( (float)scrollbarWidth / fieldSizeX, (float)scrollbarWidth / fieldSizeY );
	Vector2 positionUp( (float)xpos+(float)m_sizeX-(float)scrollbarWidth, (float)ypos );
	Vector2 positionDw( (float)xpos+(float)m_sizeX-(float)scrollbarWidth, (float)ypos+(float)m_sizeY-(float)scrollbarWidth );
	D3DXMatrixTransformation2D( &transformUp, NULL, 0.0, &arrowScale, NULL, NULL, &positionUp );
	D3DXMatrixTransformation2D( &transformDw, NULL, 0.0, &arrowScale, NULL, NULL, &positionDw );

	// Render up arrow with correct sprite
	d3dsprite->SetTransform( &transformUp );
	if( (m_state&PRESS_UP) && (m_state&HOVER_UP) ) 
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
			&m_style->upArrowPressed, NULL, NULL, 0xFFFFFFFF );
	else if( (m_state&HOVER_UP) || (m_state&PRESS_UP) ) 
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
			&m_style->upArrowHover, NULL, NULL, 0xFFFFFFFF );
	else d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
			&m_style->upArrow, NULL, NULL, 0xFFFFFFFF );

	// Render down arrow with correct sprite
	d3dsprite->SetTransform( &transformDw );
	if( (m_state&PRESS_DOWN) && (m_state&HOVER_DOWN) ) 
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
			&m_style->downArrowPressed, NULL, NULL, 0xFFFFFFFF );
	else if( (m_state&HOVER_DOWN) || (m_state&PRESS_DOWN) ) 
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
			&m_style->downArrowHover, NULL, NULL, 0xFFFFFFFF );
	else d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
			&m_style->downArrow, NULL, NULL, 0xFFFFFFFF );

	// Compute scroller transform
	Matrix transformScroller;
	float scrollerSizeY = (float)(m_style->scroller.bottom - m_style->scroller.top);
	Vector2 scalingScroller( 1.0f, (float)(m_sizeY-2*scrollbarWidth) / (float)(scrollerSizeY) ); 
	Vector2 positionScroller( (float)(xpos+m_sizeX-scrollbarWidth), (float)(ypos+scrollbarWidth) );
	D3DXMatrixTransformation2D( &transformScroller, NULL, 0.0, &scalingScroller, NULL, NULL, &positionScroller );

	// Render scroll bar backdrop
	d3dsprite->SetTransform( &transformScroller );
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		&m_style->scroller, NULL, NULL, 0xFFFFFFFF );
}
//
// --------------------------------------------------------
//	Recieves input messages and computes the current hover 
//  location of the mouse for rendering.
// --------------------------------------------------------
void DirectX::GUI::ListBox::onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
	// Extract mouse coordinates
	POINT pt = { LOWORD(lParam)-m_posX, 
				 HIWORD(lParam)-m_posY };

	// Scrollbar width
	int scrollbarWidth = m_style->downArrow.right 
		- m_style->downArrow.left;

	// Number of items in window
	int nItemsToDraw = m_sizeY / 24; // :TODO: Item height variable

	// Update mouse-over data
	switch( message )
	{
		case WM_MOUSEMOVE:
			if( m_state&MOUSED_OVER )
				if( pt.x > m_sizeX-scrollbarWidth ) 
				{
					if( pt.y < scrollbarWidth ) { m_state|=HOVER_UP; m_state &= (~HOVER_DOWN); }
					else if( pt.y > m_sizeY-scrollbarWidth ) { m_state |= HOVER_DOWN; m_state &= (~HOVER_UP); }
					else { m_state &= (~HOVER_DOWN); m_state &= (~HOVER_UP); }
				}
			else {
				m_state&=(~HOVER_DOWN); m_state&=(~HOVER_UP); }
			break;

		// Update press data
		case WM_LBUTTONDOWN:
			if( m_state&HOVER_DOWN) 
			{
				m_state |= PRESS_DOWN; 
				if( m_topItemIndex+nItemsToDraw < m_items.size( ) ) 
					m_topItemIndex++;
			}
			else if( m_state&HOVER_UP) 
			{ 
				m_state |= PRESS_UP; 
				if( m_topItemIndex > 0 ) m_topItemIndex--;
			} 

			// Toggle item selection
			else if( pt.x < m_sizeX-24 )
			{
				// Get the selected item index
				int item = m_topItemIndex + pt.y / m_spacing;
				if( item >= m_items.size( ) ) return;
				if( !(m_state&MULTI_ITEM) ) deselectAll( );
				m_items[item].selected = !m_items[item].selected;
				m_selected = m_items[item].key;

				// Send the list box selection update message 
				DirectX::Manager::instance( )->pumpMessage( this, CM_LIST_BOX_SELECTION, NULL );
			}

			break;

		// Double clicked selection item message
		case WM_LBUTTONDBLCLK: DirectX::Manager::instance( )
			->pumpMessage( this, CM_LIST_BOX_DBLCLICK, NULL ); 
			break;


		// Button release
		case WM_LBUTTONUP:
			m_state&=(~PRESS_DOWN);
			m_state&=(~PRESS_UP);
			break;
	}
}