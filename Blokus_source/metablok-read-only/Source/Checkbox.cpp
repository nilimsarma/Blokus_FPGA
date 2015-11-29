/* ===========================================================================

	Project: DirectX Engine - Checkbox

	Description:
	 Provides an implementation of a simple checkbox control.

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
#include "Checkbox.h"
 
// --------------------------------------------------------
//	Creates a new checkbox object in the specified panel.
// --------------------------------------------------------
DirectX::GUI::Checkbox& DirectX::GUI::Checkbox::create( WindowStyle* style, Panel* panel )
{
	Checkbox* checkbox = new Checkbox( );
	checkbox->m_style = style; 
	checkbox->m_panel = panel; 
	panel->addControl( checkbox ); 
	return *checkbox;
}
//
// --------------------------------------------------------
//	Checks for a mouse button press on focus and then
//  changes the check state.
// --------------------------------------------------------
void DirectX::GUI::Checkbox::onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
	// Return focus to panel on right mouse button focus gain
	if( message == WM_LBUTTONUP || message == WM_RBUTTONUP ) 
		if(m_state&FOCUSED) m_panel->giveFocus( );

	// Check for checkbox clicked and determine if state change is necessary
	if( (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN) &&
		(m_state&FOCUSED) && (m_state&MOUSED_OVER) )
	{
		// Pump the check message through to any listeners
		if( m_state&CHECKED ) DirectX::Manager::instance( )->pumpMessage( this, CM_CHECKBOX_UNCHECKED, NULL );
		else DirectX::Manager::instance( )->pumpMessage( this, CM_CHECKBOX_CHECKED, NULL );

		// Change check state
		m_state ^= CHECKED;
	}
}
//
// --------------------------------------------------------
//	Renders the checkbox control in the proper state.
// --------------------------------------------------------
void DirectX::GUI::Checkbox::render( LPD3DXSPRITE d3dsprite, const POINT* pos )
{
	Matrix transform;

	// Compute absolute offset 
	int xpos = m_posX + pos->x; 
	int ypos = m_posY + pos->y;

	// Compute the button sprite transform
	float xscale = m_sizeX / (float)(m_style->checkboxOff.right - m_style->checkboxOff.left);
	float yscale = m_sizeY / (float)(m_style->checkboxOff.bottom - m_style->checkboxOff.top);
	D3DXMatrixTransformation2D( &transform, NULL, 0.0, &Vector2( xscale, yscale ), 
		NULL, NULL, &Vector2( (float)xpos, (float)ypos ) );

	// Set the sprite transform
	d3dsprite->SetTransform( &transform );

	// Render the button sprite with under the transform
	if( m_state&CHECKED ) d3dsprite->Draw( m_style->spriteSheet.getImage( ), &m_style->checkboxOn, NULL, NULL, 0xFFFFFFFF );
	else  d3dsprite->Draw( m_style->spriteSheet.getImage( ), &m_style->checkboxOff, NULL, NULL, 0xFFFFFFFF );
}