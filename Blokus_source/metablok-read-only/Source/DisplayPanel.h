/* ===========================================================================

	Project: DirectX Engine - Display Panel

	Description:
	 Provides an implementation of a special panel which has no parent
	 panel. Display Panel callbacks are called directly by the DirectX 
	 Manager when objects at its depth are being rendered. Aside from this,
	 Display Panels have all the characteristics of a normal panel control.

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
#ifndef DX_GUI_DISPLAY_PANEL
#define DX_GUI_DISPLAY_PANEL

// Engine namespace
namespace DirectX {
namespace GUI {

// Window Control Class
class DisplayPanel : public Panel
{
public:
	// Destructor
	~DisplayPanel( );

	// Permanently join display panels
	void join( DisplayPanel* panel );

	// Construction parameters
	static DisplayPanel& create( );
	DisplayPanel& pos( int x, int y ) { m_posX = x; m_posY = y; return *this; }
	DisplayPanel& size( int sx, int sy ) { m_sizeX = sx; m_sizeY = sy; return *this; }
	DisplayPanel& depth( int depth ) { m_depth = depth; return *this; }

	// Overridden interaction detection
	virtual bool contains( const POINT *pt );

	// Override focus release
	virtual void giveFocus( );

private:
	DisplayPanel( ) { }
};

}}

// End definition
#endif