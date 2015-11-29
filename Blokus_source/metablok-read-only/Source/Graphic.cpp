/* ===========================================================================

	Project: DirectX Engine - Graphic

	Description:
	 Provides an implementation of a simple non-animated graphic control.

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
#include "Graphic.h"

// --------------------------------------------------------
//	Creates a new graphic control.
// --------------------------------------------------------
DirectX::GUI::Graphic& DirectX::GUI::Graphic::create( Image* image, Panel* panel )
{
	Graphic* graphic = new Graphic( );
	graphic->m_image = image; 
	graphic->m_panel = panel;
	panel->addControl( graphic ); 

	graphic->m_rect.left = 0; graphic->m_rect.top = 0;
	graphic->m_rect.bottom = graphic->m_image->height( );
	graphic->m_rect.right = graphic->m_image->width( );

	return *graphic;
}
//
// --------------------------------------------------------
//	Renders the graphic control in the proper state.
// --------------------------------------------------------
void DirectX::GUI::Graphic::render( LPD3DXSPRITE d3dsprite, const POINT* pos )
{
	Matrix transform;

	// Compute the sprite transformation matrix
	Vector2 position = Vector2( (float)(m_posX + pos->x), (float)(m_posY + pos->y) );
	Vector2 scale = Vector2( (float)m_sizeX / (float)(m_rect.right - m_rect.left),
							 (float)m_sizeY / (float)(m_rect.bottom - m_rect.top) );
	D3DXMatrixTransformation2D( &transform, NULL, 0.0, &scale, NULL, NULL, &position );

	// Set the sprite transform matrix
	d3dsprite->SetTransform( &transform );

	// Render the sprite 
	d3dsprite->Draw( m_image->getImage( ), &m_rect, NULL, NULL, 0xFFFFFFFF );
}