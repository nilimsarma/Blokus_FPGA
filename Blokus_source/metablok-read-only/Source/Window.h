/* ===========================================================================

	Project: DirectX Engine - Window

	Description:
	 Provides an implementation of a simple window control with panel
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

// Begin definition
#ifndef DX_GUI_WINDOW
#define DX_GUI_WINDOW

// Engine namespace
namespace DirectX {
namespace GUI {

// Window Control Class
class Window : public Panel
{
public:
	// Extended state indicators for window panels
	enum WindowState {  CLOSED = 0x10, MINIMIZED = 0x20, HOVER_MINIMIZED = 0x40, 
		HOVER_CLOSED = 0x80, DRAGGING = 0x100, NOT_UNFOCUSABLE = 0x200 };

	// Construction parameters
	static Window& create( WindowStyle* style, Panel* panel ); 
	Window& pos( int x, int y ) { m_posX = x; m_posY = y; return *this; }
	Window& size( int sx, int sy ) { m_sizeX = sx; m_sizeY = sy; return *this; }
	Window& name( const wchar_t* name ) { m_name = std::wstring(name); return *this; }
	Window& notUnfocusable( ) { m_state |= NOT_UNFOCUSABLE; return *this; }
	Window& depth( int depth ) { m_depth = depth; return *this; }
	Window& minimized( ) { m_state |= MINIMIZED; return *this; }
	Window& hidden( ) { m_state |= HIDDEN; return *this; }

	// Alters focusing properties of window
	void makeNotUnfocusable( ) { m_state |= NOT_UNFOCUSABLE; }
	void makeUnfocusable( ) { m_state &= (~NOT_UNFOCUSABLE); }
	int isUnfocusable( ) { return !(m_state&NOT_UNFOCUSABLE); }

	// Window actions
	int isMinimized( ) { return m_state&MINIMIZED; }
	void maximize( ) { m_state &= (~MINIMIZED); }
	void minimize( ) { m_state |= MINIMIZED; } 

    // Override containment for focusing
	bool contains( const POINT* pt );

	// Overridden message event callbacks
	void onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam );

	// Overridden panel render method
	void render( LPD3DXSPRITE d3dsprite, const POINT* pos );

private:
	// Default constructor
	Window( ) : m_name(L"") { }

	// Window render components
	__forceinline void renderMiddle( LPD3DXSPRITE d3dsprite, int xpos, int ypos );
	__forceinline void renderUpper( LPD3DXSPRITE d3dsprite, int xpos, int ypos );
	__forceinline void renderLower( LPD3DXSPRITE d3dsprite, int xpos, int ypos );
	__forceinline void renderRight( LPD3DXSPRITE d3dsprite, int xpos, int ypos );
	__forceinline void renderLeft( LPD3DXSPRITE d3dsprite, int xpos, int ypos );
	__forceinline void renderUL( LPD3DXSPRITE d3dsprite, int xpos, int ypos );
	__forceinline void renderUR( LPD3DXSPRITE d3dsprite, int xpos, int ypos );
	__forceinline void renderLL( LPD3DXSPRITE d3dsprite, int xpos, int ypos );
	__forceinline void renderLR( LPD3DXSPRITE d3dsprite, int xpos, int ypos );

	// Window title
	std::wstring m_name;
};

}}

// End definition
#endif