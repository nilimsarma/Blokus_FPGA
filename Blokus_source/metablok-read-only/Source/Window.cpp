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

// Standard includes
#include "DirectX.h"

// Include header
#include "Window.h"

// --------------------------------------------------------
//	Creates a new window control.
// --------------------------------------------------------
DirectX::GUI::Window& DirectX::GUI::Window::create( WindowStyle* style, Panel* panel )
{
	Window* window = new Window( );
	window->m_style = style; 
	window->m_panel = panel;
	panel->addControl( window );
	return *window;
}
//
// --------------------------------------------------------
//	Checks for a mouse button release and gives focus to
//  the parent panel when the click is completed.
// --------------------------------------------------------
void DirectX::GUI::Window::onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
	// Check for interactions with child controls when window is not minimized
	if( !(m_state&MINIMIZED) ) Panel::onMouseEvent( message, wParam, lParam );

	// Window border region widths
	long leftTopWidth = m_style->windowBorderTopLeft.right - m_style->windowBorderTopLeft.left;
	long rightTopWidth = m_style->windowBorderTopRight.right - m_style->windowBorderTopRight.left;
	long topWidth = m_style->windowBorderTopMiddle.right - m_style->windowBorderTopMiddle.left;
	long leftWidth = m_style->windowBorderLeft.right - m_style->windowBorderLeft.left;
	long rightWidth = m_style->windowBorderRight.right - m_style->windowBorderRight.left;
	long centerWidth = m_style->windowMiddleTransparent.right - m_style->windowMiddleTransparent.left;
	long leftBottomWidth = m_style->windowBorderBottomLeft.right - m_style->windowBorderBottomLeft.left;
	long rightBottomWidth = m_style->windowBorderBottomRight.right - m_style->windowBorderBottomRight.left;
	long bottomWidth = m_style->windowBorderBottom.right - m_style->windowBorderBottom.left;

	// Window border region heights
	long leftTopHeight = m_style->windowBorderTopLeft.bottom - m_style->windowBorderTopLeft.top;
	long rightTopHeight = m_style->windowBorderTopRight.bottom - m_style->windowBorderTopRight.top;
	long topHeight = m_style->windowBorderTopMiddle.bottom - m_style->windowBorderTopMiddle.top;
	long leftHeight = m_style->windowBorderLeft.bottom - m_style->windowBorderLeft.top;
	long rightHeight = m_style->windowBorderRight.bottom - m_style->windowBorderRight.top;
	long centerHeight = m_style->windowMiddleTransparent.bottom - m_style->windowMiddleTransparent.top;
	long leftBottomHeight = m_style->windowBorderBottomLeft.bottom - m_style->windowBorderBottomLeft.top;
	long rightBottomHeight = m_style->windowBorderBottomRight.bottom - m_style->windowBorderBottomRight.top;
	long bottomHeight = m_style->windowBorderBottom.bottom - m_style->windowBorderBottom.top;

	// Relevant window regions, excluding border region
	POINT pt = { LOWORD(lParam)-m_posX, HIWORD(lParam)-m_posY };
	RECT minimizeRegion = { m_sizeX-40, 12, m_sizeX-30, 22 };
	RECT closeRegion = { m_sizeX-20, 12, m_sizeX-10, 22 };
	RECT windowRegion = { leftWidth, topHeight, 
		m_sizeX-rightWidth, m_sizeY-bottomHeight };
	RECT windowPanel = { 0, 0, m_sizeX, m_sizeY };

	// Parse messages
	switch( message )
	{
		// Window dragging and Min/Close
		// button hover detection
		case WM_MOUSEMOVE:

			// Drag window
			if( m_state&DRAGGING ) 
				move( DirectX::Manager::instance( )->getMouseMoveX( ), 
					  DirectX::Manager::instance( )->getMouseMoveY( ) );

			// Check for button hovering ...
			else if( PtInRect( &minimizeRegion, pt ) )
				m_state |= HOVER_MINIMIZED;

			// ... Else ...
			else 
			{
				// ... Mask off minimized state ...
				m_state &= (~HOVER_MINIMIZED);

				// ... And check for closed hovering ...
				if( PtInRect( &closeRegion, pt ) )
					m_state |= HOVER_CLOSED;

				// ... Else mask off closed state.
				else m_state &= (~HOVER_CLOSED);
			}

			break;

		// Check for action buttons
		// or window drag initiation
		case WM_LBUTTONDOWN:

			// Check for window closed
			if(m_state&HOVER_CLOSED) 
			{
				// Send window close message
				DirectX::Manager::instance( )->
					pumpMessage( this, CM_WINDOW_CLOSED, NULL );

				hide( );
			} 

			// Check for maximize/minimize
			else if(m_state&HOVER_MINIMIZED)
			{
				 if(m_state&MINIMIZED) maximize( ); 
				 else minimize( );
			}

			// Begin window drag
			else if( !PtInRect( &windowRegion, pt ) && 
				      PtInRect( &windowPanel, pt ) ) 
					  m_state |= DRAGGING;

			break;

		// End window drag op
		case WM_LBUTTONUP:
			m_state &= (~DRAGGING);
			break;
	}
}
//
// --------------------------------------------------------
//	Return containment when focused if not unfocusable.
// --------------------------------------------------------
bool DirectX::GUI::Window::contains( const POINT* pt )
{
    if( m_state&NOT_UNFOCUSABLE ) return true;
	else return Panel::contains( pt );
}
//
// --------------------------------------------------------
//	Renders the window and any child controls.
// --------------------------------------------------------
void DirectX::GUI::Window::render( LPD3DXSPRITE d3dsprite, const POINT* pos )
{
	// Compute absolute offset 
	int xpos = m_posX + pos->x; 
	int ypos = m_posY + pos->y;
	
	// Render essential window components
	renderUpper( d3dsprite, xpos, ypos );
	renderUL( d3dsprite, xpos, ypos );
	renderUR( d3dsprite, xpos, ypos );

	// Render unminimized components
	if( !(m_state&MINIMIZED) )
	{
		renderMiddle( d3dsprite, xpos, ypos );
		renderLower( d3dsprite, xpos, ypos );
		renderRight( d3dsprite, xpos, ypos );
		renderLeft( d3dsprite, xpos, ypos );
		renderLL( d3dsprite, xpos, ypos );
		renderLR( d3dsprite, xpos, ypos );
		Panel::render( d3dsprite, pos );
	}
}
//
// --------------------------------------------------------
//	Renders the individual window components.
// --------------------------------------------------------
void DirectX::GUI::Window::renderMiddle( LPD3DXSPRITE d3dsprite, int xpos, int ypos )
{
	Matrix transform;

	RECT* icon = &m_style->windowMiddleTransparent;
	RECT* ricon = &m_style->windowBorderBottomRight;
	RECT* licon = &m_style->windowBorderTopLeft;
	float xscale = (float)( m_sizeX - (ricon->right-ricon->left) -
				   (licon->right-licon->left) );

	float yscale = (float)( m_sizeY - (licon->bottom - licon->top) -
				   (ricon->bottom - ricon->top) );

	D3DXMatrixTransformation2D( &transform, NULL, 0.0, 
		&Vector2( xscale/(float)(icon->right-icon->left), yscale/(float)(icon->bottom-icon->top) ), 
		NULL, NULL, &Vector2( (float)(xpos+(licon->right-licon->left)), 
		(float)(ypos+(licon->bottom-licon->top)) ) );

	d3dsprite->SetTransform( &transform ); 
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		icon, NULL, NULL, 0xd0FFFFFF );
}
void DirectX::GUI::Window::renderUpper( LPD3DXSPRITE d3dsprite, int xpos, int ypos )
{
	Matrix transform;

	RECT* icon = &m_style->windowBorderTop;
	RECT* ricon = &m_style->windowBorderTopRight;
	RECT* licon = &m_style->windowBorderTopLeft;
	float xscale = (float)( m_sizeX - (ricon->right-ricon->left) -
				   (licon->right-licon->left) );

	D3DXMatrixTransformation2D( &transform, NULL, 0.0, 
		&Vector2( xscale/(float)(icon->right-icon->left), 1.0f ), 
		NULL, NULL, &Vector2( (float)(xpos+(licon->right-licon->left)), 
		(float)ypos ) );

	d3dsprite->SetTransform( &transform ); 
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		icon, NULL, NULL, 0xFFFFFFFF );
}
void DirectX::GUI::Window::renderLower( LPD3DXSPRITE d3dsprite, int xpos, int ypos )
{
	Matrix transform;

	RECT* icon = &m_style->windowBorderBottom;
	RECT* ricon = &m_style->windowBorderBottomRight;
	RECT* licon = &m_style->windowBorderBottomLeft;
	float xscale = (float)( m_sizeX - (ricon->right-ricon->left) -
				   (licon->right-licon->left) );

	D3DXMatrixTransformation2D( &transform, NULL, 0.0, 
		&Vector2( xscale/(float)(icon->right-icon->left), 1.0f ), 
		NULL, NULL, &Vector2( (float)(xpos+(licon->right-licon->left)), 
		(float)(ypos+m_sizeY-(ricon->bottom-ricon->top)) ) );

	d3dsprite->SetTransform( &transform ); 
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		icon, NULL, NULL, 0xFFFFFFFF );
}
void DirectX::GUI::Window::renderRight( LPD3DXSPRITE d3dsprite, int xpos, int ypos )
{
	Matrix transform;

	RECT* icon = &m_style->windowBorderRight;
	RECT* ticon = &m_style->windowBorderTopRight;
	RECT* bicon = &m_style->windowBorderBottomRight;
	float yscale = (float)( m_sizeY - (ticon->bottom - ticon->top) -
				   (bicon->bottom - bicon->top) );

	D3DXMatrixTransformation2D( &transform, NULL, 0.0, &Vector2( 1.0f, yscale/(float)(icon->bottom-icon->top) ), 
		NULL, NULL, &Vector2( (float)xpos, 
			(float)(ypos-ticon->top+ticon->bottom) ) );

	d3dsprite->SetTransform( &transform ); 
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		icon, NULL, NULL, 0xFFFFFFFF );
}
void DirectX::GUI::Window::renderLeft( LPD3DXSPRITE d3dsprite, int xpos, int ypos )
{
	Matrix transform;

	RECT* icon = &m_style->windowBorderLeft;
	RECT* ticon = &m_style->windowBorderTopLeft;
	RECT* bicon = &m_style->windowBorderBottomLeft;
	float yscale = (float)( m_sizeY - (ticon->bottom - ticon->top) -
				   (bicon->bottom - bicon->top) );

	D3DXMatrixTransformation2D( &transform, NULL, 0.0, &Vector2( 1.0f, yscale/(float)(icon->bottom-icon->top) ), 
		NULL, NULL, &Vector2( (float)(xpos+m_sizeX-(bicon->right-bicon->left)), 
			(float)(ypos-ticon->top+ticon->bottom) ) );

	d3dsprite->SetTransform( &transform ); 
	d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
		icon, NULL, NULL, 0xFFFFFFFF );
}
void DirectX::GUI::Window::renderUL( LPD3DXSPRITE d3dsprite, int xpos, int ypos )
{
	// Determine control panel sprite
	RECT* cIcon; 
	if( m_state&MINIMIZED ) 
		cIcon = &m_style->windowBorderTopLeftMin;
	else cIcon = &m_style->windowBorderTopLeft;

	// Render control bar button panel
	Matrix transform;
	D3DXMatrixTransformation2D( &transform, NULL, 0.0, &Vector2( 1.0f, 1.0f ), 
		NULL, NULL, &Vector2( (float)xpos, (float)ypos ) );
		d3dsprite->SetTransform( &transform ); d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
			cIcon, NULL, NULL, 0xFFFFFFFF );
}
void DirectX::GUI::Window::renderUR( LPD3DXSPRITE d3dsprite, int xpos, int ypos )
{
	RECT* cIcon; 

	// Determine control panel sprite
	if( m_state&MINIMIZED )
	{
		if( m_state&HOVER_CLOSED ) 
			cIcon = &m_style->windowBorderTopRightMinClose;
		else if( m_state&HOVER_MINIMIZED )
			cIcon = &m_style->windowBorderTopRightMinMin;
		else cIcon = &m_style->windowBorderTopRightMinNormal;
	}
	else 
	{
		if( m_state&HOVER_CLOSED ) 
			cIcon = &m_style->windowBorderTopRightClose;
		else if( m_state&HOVER_MINIMIZED )
			cIcon = &m_style->windowBorderTopRightMin;
		else cIcon = &m_style->windowBorderTopRight;
	}

	// Render control bar button panel
	Matrix transform;
	D3DXMatrixTransformation2D( &transform, NULL, 0.0, &Vector2( 1.0f, 1.0f ), 
		NULL, NULL, &Vector2( (float)(xpos+m_sizeX-cIcon->right+cIcon->left), (float)ypos ) );
		d3dsprite->SetTransform( &transform ); d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
			cIcon, NULL, NULL, 0xFFFFFFFF );
}
void DirectX::GUI::Window::renderLL( LPD3DXSPRITE d3dsprite, int xpos, int ypos )
{
	Matrix transform;

	RECT* icon = &m_style->windowBorderBottomLeft;

	// Render control bar button panel
	D3DXMatrixTransformation2D( &transform, NULL, 0.0, &Vector2( 1.0f, 1.0f ), 
		NULL, NULL, &Vector2( (float)xpos, (float)(ypos+m_sizeY+icon->top-icon->bottom) ) );
		d3dsprite->SetTransform( &transform ); d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
			&m_style->windowBorderBottomLeft, NULL, NULL, 0xFFFFFFFF );
}
void DirectX::GUI::Window::renderLR( LPD3DXSPRITE d3dsprite, int xpos, int ypos )
{
	Matrix transform;

	RECT* icon = &m_style->windowBorderBottomRight;

	// Render control bar button panel
	D3DXMatrixTransformation2D( &transform, NULL, 0.0, &Vector2( 1.0f, 1.0f ), 
		NULL, NULL, &Vector2( (float)(xpos+m_sizeX+icon->left-icon->right), 
			(float)(ypos+m_sizeY+icon->top-icon->bottom) ) );
		d3dsprite->SetTransform( &transform ); d3dsprite->Draw( m_style->spriteSheet.getImage( ), 
			icon, NULL, NULL, 0xFFFFFFFF );
}