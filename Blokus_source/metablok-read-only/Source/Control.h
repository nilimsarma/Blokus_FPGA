/* ===========================================================================

	Project: DirectX Engine GUI - Control

	Description:
	 Defines the abstract base class for GUI controls managed by the 
	 internal manager.

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
#ifndef DX_GUI_CONTROL_H
#define DX_GUI_CONTROL_H

// Engine namespace
namespace DirectX {
namespace GUI {

// Panel prototype
class Panel;

// Enumerated text alignment specifications
enum TextAlign { Left = 0x0, Right = 0x2, Top = 0x0, Bottom = 0x8, 
				 Center = 0x1, VCenter = 0x4, NoClip = 0x100 };

// GUI control
class Control
{
public:
	// Default state indicators for controls
	enum ControlState { HAS_HOTSPOT = 0x1, FOCUSED = 0x2, MOUSED_OVER = 0x4, HIDDEN = 0x10 };

	// Constructor - initialize fields
	Control( ) : m_panel( NULL ), m_style(NULL), m_sizeX(32), 
		m_sizeY(32), m_depth(0), m_state(0), m_posX(0), m_posY(0),
		m_color(0xFFFFFFFF) { }

	// Destructor - decouple from panel
	virtual ~Control( ) { release( ); }

	// Panelling operations
	void moveToPanel( Panel& panel );
	Panel* getPanel( ) { return m_panel; }

	// Interaction accessors
	bool hasFocus( ) { return (m_state&FOCUSED) == FOCUSED; }
	bool hasMouse( ) { return (m_state&MOUSED_OVER) == MOUSED_OVER; }

	// Interaction detection
	virtual bool contains( const POINT *pt );

	// Visibility operations
	int isHidden( ) { return (m_state&HIDDEN); }
	virtual void hide( ) { m_state |= HIDDEN; }
	virtual void show( ) { m_state &= (~HIDDEN); }

	// Style modifier/accessor
	virtual void setStyle( WindowStyle* style ) { m_style = style; }
	virtual WindowStyle* getStyle( ) { return m_style; }

	// Position modifiers and accessors
	void setPosition( int x, int y ) { move( x-m_posX, y-m_posY ); }
	void setPositionX( int pos ) { move( pos-m_posX, 0 ); }
	void setPositionY( int pos ) { move( 0, pos-m_posY ); }
	void move( int x, int y ) { m_posX += x; m_posY += y; }
	void moveX( int dist ) { m_posX += dist; }
	void moveY( int dist ) { m_posY += dist; }
	int getPositionX( ) { return m_posX; }
	int getPositionY( ) { return m_posY; }

	// Global position accessors for rendering
	virtual void setDepth( int depth ) { m_depth = depth; }
	virtual int getDepth( ) { return m_depth; }

	// Size accessors/modifiers
	virtual void setSize( int x, int y ) { m_sizeX = x; m_sizeY = y; }
	virtual void setSizeX( int size ) { m_sizeX = size; }
	virtual void setSizeY( int size ) { m_sizeY = size; }
	int getSizeX( ) { return m_sizeX; }
	int getSizeY( ) { return m_sizeY; }

	// Message event callbacks
	virtual void onKeyboardEvent( UINT message, WPARAM wParam, LPARAM lParam ) { }
	virtual void onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam ) { }
	virtual void onMouseEnter( ) { m_state |= MOUSED_OVER; }
	virtual void onMouseLeave( ) { m_state ^= MOUSED_OVER; }
	virtual void onGainFocus( ) { m_state |= FOCUSED; }
	virtual void onLoseFocus( ) { m_state ^= FOCUSED; }

	// Color modifiers
	unsigned long getColor( ) { return m_color; }
	void setColor( unsigned long clr ) { m_color = clr; }

	// Rendering 
	virtual void render( LPD3DXSPRITE d3dsprite, const POINT* pos ) = 0;

protected:
	// Parent panel
	Panel* m_panel;

	// :TODO: Deprecated
	unsigned long m_color;
	WindowStyle* m_style;

	// Control parameters
	int m_depth, m_posX, m_posY;
	int m_sizeX, m_sizeY;
	int m_state;

private:
	// Decouple from panel
	void release( );
};

}}

// End definition
#endif