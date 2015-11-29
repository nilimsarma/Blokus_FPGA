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

// Include engine 
#include "DirectX.h"

// Include header
#include "Label.h"

// ------------------------------------
//  Creates a new label.
// ------------------------------------
DirectX::GUI::Label& DirectX::GUI::Label::create( WindowStyle* style, Panel* panel )
{
	Label* label = new Label( );
	label->m_style = (DirectX::GUI::WindowStyle*)style; 
	label->m_panel = panel; 
	panel->addControl( label ); 
	return *label;
}
//
// -------------------------------------
//  Renders the label text.
// -------------------------------------
void DirectX::GUI::Label::render( LPD3DXSPRITE d3dsprite, const POINT* pos )
{
	// Compute absolute offset 
	int xpos = m_posX + pos->x; 
	int ypos = m_posY + pos->y;

	// Construct render are for text label
	Matrix matrix; D3DXMatrixIdentity( &matrix ); d3dsprite->SetTransform( &matrix );
	RECT rect; SetRect( &rect, xpos, ypos, xpos+m_sizeX, ypos+m_sizeY );

	// Render the text using the internal
	m_style->font.getFont( )->DrawTextW( d3dsprite, 
		m_text.c_str( ), -1, &rect, m_alignment, m_color );
}