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

// Begin definition
#ifndef DX_GUI_GRAPHIC
#define DX_GUI_GRAPHIC

// Engine namespace
namespace DirectX {
namespace GUI {

// Disjoint Tree Class
class Graphic : public Control
{
public:
	// Construction parameters
	static Graphic& create( Image* image, Panel* panel ); 
	Graphic& pos( int x, int y ) { m_posX = x; m_posY = y; return *this; }
	Graphic& size( int sx, int sy ) { m_sizeX = sx; m_sizeY = sy; return *this; }
	Graphic& depth( int depth ) { m_depth = depth; return *this; }
	Graphic& rect( RECT& rect ) { m_rect = rect; return *this; }
	Graphic& hidden( ) { m_state |= HIDDEN; return *this; }

    // Image modifiers
	void setImage( DirectX::Image* image ) { m_image = image; }
	void setRect( RECT& rect ) { m_rect = rect; }

	// Overridden message event callbacks
	void render( LPD3DXSPRITE d3dsprite, const POINT* pos );

private:
	// Default constructor
	Graphic( ) { }

	// Graphic icon 
	DirectX::Image* m_image;
	RECT m_rect;
};

}}

// End definition
#endif