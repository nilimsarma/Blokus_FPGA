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

// Standard includes
#include "DirectX.h"

// Include header
#include "Button.h"

// --------------------------------------------------------
//	Creates a new button control.
// --------------------------------------------------------
DirectX::GUI::Button& DirectX::GUI::Button::create( WindowStyle* style, Panel* panel )
{
	Button* button = new Button( );
	button->m_style = style; 
	button->m_panel = panel;
	panel->addControl( button ); 
	return *button;
}
//
// --------------------------------------------------------
//	Checks if the button has been left-button clicked.
// --------------------------------------------------------
void DirectX::GUI::Button::onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
	// Pump button pressed message through to any listeners
	if( (m_state&FOCUSED) && (m_state&MOUSED_OVER) && message == WM_LBUTTONUP )
		DirectX::Manager::instance( )->pumpMessage( this, CM_BUTTON_PRESSED, NULL );

	// Return focus to panel on right mouse button focus gain
	if( message == WM_LBUTTONUP || message == WM_RBUTTONDOWN ) 
		if(m_state&FOCUSED) m_panel->giveFocus( );
}
//
// --------------------------------------------------------
//	Renders the button control in the proper state.
// --------------------------------------------------------
void DirectX::GUI::Button::render( LPD3DXSPRITE d3dsprite, const POINT* pos )
{
	Matrix transform;
	RECT* rect;

	// Determine appropriate sprite for current state
	if( m_state&FOCUSED && m_state&MOUSED_OVER ) rect = &m_style->buttonDown;
	else if( m_state&MOUSED_OVER || m_state&FOCUSED ) rect = &m_style->buttonHover;
	else rect = &m_style->button;

	// Compute absolute offset 
	int xpos = m_posX + pos->x; 
	int ypos = m_posY + pos->y;

	// Draw section scaled button
	if( m_style->buttonScaleable ) 
	{
		// Generate identity transform
		D3DXMatrixIdentity( &transform ); 
		
		// Create inner section rectangle
		RECT section, inner; int third = (rect->bottom-rect->top)/3;
		SetRect( &inner, rect->left+third, rect->top+third, rect->left+third*2, rect->top+third*2 );

		transform._41 = xpos; transform._42 = ypos; // Top Left
		d3dsprite->SetTransform( &transform ); 
		SetRect( &section, rect->left, rect->top, inner.left, inner.top );
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), &section, NULL, NULL, 0xFFFFFFFF );

		transform._41 = xpos+m_sizeX-third; transform._42 = ypos; // Top right
		d3dsprite->SetTransform( &transform );
		section.left = inner.right; section.right = rect->right;
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), &section, NULL, NULL, 0xFFFFFFFF );
		
		transform._41 = xpos+m_sizeX-third; transform._42 = ypos+m_sizeY-third; // Bottom right
		d3dsprite->SetTransform( &transform );
		section.top = inner.bottom; section.bottom = rect->bottom;
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), &section, NULL, NULL, 0xFFFFFFFF );

		transform._41 = xpos; transform._42 = ypos+m_sizeY-third; // Bottom left
		d3dsprite->SetTransform( &transform );
		section.left = rect->left; section.right = inner.left;
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), &section, NULL, NULL, 0xFFFFFFFF );

		transform._41 = xpos+third; transform._11 = (float)(m_sizeX-2*third) / (float)third; // Bottom
		d3dsprite->SetTransform( &transform );
		section.left = inner.left; section.right = inner.right;
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), &section, NULL, NULL, 0xFFFFFFFF );
		
		transform._42 = ypos; // Top
		d3dsprite->SetTransform( &transform );
		section.top = rect->top; section.bottom = inner.top;
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), &section, NULL, NULL, 0xFFFFFFFF );

		transform._22 = (float)(m_sizeY-2*third) / (float)third; transform._42 = ypos+third; // Middle
		d3dsprite->SetTransform( &transform );
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), &inner, NULL, NULL, 0xFFFFFFFF );

		transform._11 = 1.0f; transform._41 = xpos; // Left
		d3dsprite->SetTransform( &transform );
		SetRect( &section, rect->left, inner.top, inner.left, inner.bottom );
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), &section, NULL, NULL, 0xFFFFFFFF );

		transform._41 = xpos+m_sizeX-third; transform._42 = ypos+third; // Right
		d3dsprite->SetTransform( &transform );
		section.left = inner.right; section.right = rect->right;
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), &section, NULL, NULL, 0xFFFFFFFF );
	}
	else // Draw stretched button sprite 
	{
		// Compute the button sprite transform
		float xscale = m_sizeX / (float)(m_style->checkboxOff.right - m_style->checkboxOff.left);
		float yscale = m_sizeY / (float)(m_style->checkboxOff.bottom - m_style->checkboxOff.top);
		D3DXMatrixTransformation2D( &transform, NULL, 0.0, &Vector2( xscale, yscale ), 
			NULL, NULL, &Vector2( (float)xpos, (float)ypos ) );

		// Render the properly positioned sprite
		d3dsprite->SetTransform( &transform );
		d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
			rect, NULL, NULL, 0xFFFFFFFF );
	}

	// Draw button text
	RECT buttonRect = { xpos, ypos, xpos+m_sizeX, ypos+m_sizeY };
	D3DXMatrixIdentity( &transform ); d3dsprite->SetTransform( &transform );
	m_style->font.getFont( )->DrawTextW( d3dsprite, m_str.c_str( ), -1, 
		&buttonRect, m_alignment, m_style->color );

	// Render label
	if( m_label ) 
	{
		D3DXMatrixTranslation( &transform, (float)xpos, (float)ypos, 0.0f );
		d3dsprite->SetTransform( &transform );
		d3dsprite->Draw( m_label->getImage( ), &m_labelRect, NULL, NULL, 0xFFFFFFFF );
	}
}