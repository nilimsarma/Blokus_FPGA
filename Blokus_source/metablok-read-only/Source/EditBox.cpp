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

// Include engine
#include "DirectX.h"

// Include header
#include "EditBox.h"

// --------------------------------------------------------
//	Creates a new edit box control.
// --------------------------------------------------------
DirectX::GUI::EditBox& DirectX::GUI::EditBox::create( WindowStyle* style, Panel* panel )
{
	EditBox* editbox = new EditBox( );
	editbox->m_style = style; 
	editbox->m_panel = panel;
	panel->addControl( editbox ); 
	return *editbox;
}
//
// --------------------------------------------------------
//  Modifies the edit box text on keyboard events.
// --------------------------------------------------------
void DirectX::GUI::EditBox::onKeyboardEvent( UINT message, WPARAM wParam, LPARAM lParam ) 
{
	// Process key event
	if( message == WM_KEYDOWN )
	{
		// Get scancode of key
		char keyVirtual = ( BYTE )( wParam & 0xFF );
		int keyScancode = MapVirtualKey( ( BYTE )( wParam & 0xFF ), MAPVK_VK_TO_VSC );
		char keyCharacter =	MapVirtualKey( ( BYTE )( wParam & 0xFF ), MAPVK_VK_TO_CHAR );

		// Enter
		if( keyVirtual == VK_RETURN ) 
		{
		}

		// Backspace
		else if( keyScancode == KEY_BACKSPACE )
		{
			if( m_cursorPosition != m_text.begin( ) )
			{
				std::wstring::iterator prev = m_cursorPosition; prev--;
				m_cursorPosition = m_text.erase( prev, m_cursorPosition );
			}
		}

		// Move cursor
		else if( keyVirtual == VK_LEFT && m_cursorPosition != m_text.begin( ) ) m_cursorPosition--;
		else if( keyVirtual == VK_RIGHT && m_cursorPosition != m_text.end( )  ) m_cursorPosition++;

		// Insert input character into sequence
		else if( keyCharacter != '\0' )
		{
			// Check for letter and set proper case
			if( keyCharacter > 64 && keyCharacter < 96 )
				{ if( !DirectX::Manager::instance( )->getKeyState( KEY_LSHIFT ) ) keyCharacter += 32; }
			
			// Check for number
			else if( keyCharacter > 47 && keyCharacter < 58 );

			// Check for space
			else if( keyCharacter == 32 );

			// Check for minus character
			else if( keyCharacter == 45 );

			// Otherwise reject the input
			else return;

			// Insert the new character into the sequence
			m_cursorPosition = m_text.insert( m_cursorPosition, keyCharacter );
			m_cursorPosition++;
		}
	}
}
//
// --------------------------------------------------------
//  Renders the edit box and the internal text.
// --------------------------------------------------------
void DirectX::GUI::EditBox::render( LPD3DXSPRITE d3dsprite, const POINT* pos )
{
	// Compute absolute offset 
	int xpos = m_posX + pos->x; 
	int ypos = m_posY + pos->y;

	// Render edit box background sprite
	Matrix fieldTransform;
	float fieldScaleX = (float)m_sizeX / (float)(m_style->selectionBox.right - m_style->selectionBox.left);
	float fieldScaleY = (float)m_sizeY / (float)(m_style->selectionBox.bottom - m_style->selectionBox.top);
	D3DXMatrixTransformation2D( &fieldTransform, NULL, 0.0, &Vector2( fieldScaleX, fieldScaleY ), NULL, 
		NULL, &Vector2( (float)xpos, (float)ypos ) );

	// Render the properly positioned sprite
	d3dsprite->SetTransform( &fieldTransform );
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		&m_style->selectionBox, NULL, NULL, 0xFFFFFFFF );

	// Draw button text
	if( m_state&FOCUSED ) m_cursorPosition = m_text.insert( m_cursorPosition, '|' );
	Matrix iden; RECT textRect = { xpos, ypos, xpos+m_sizeX, ypos+m_sizeY };
	D3DXMatrixIdentity( &iden ); d3dsprite->SetTransform( &iden );
	m_style->font.getFont( )->DrawTextW( d3dsprite, m_text.c_str( ), -1, 
		&textRect, m_alignment, m_style->color );
	if( m_state&FOCUSED ) { std::wstring::iterator next = m_cursorPosition; next++; 
		m_cursorPosition = m_text.erase( m_cursorPosition, next ); }
}