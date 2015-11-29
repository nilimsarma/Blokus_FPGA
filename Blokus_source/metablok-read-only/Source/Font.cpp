/* ===========================================================================

	Project: DirectX Engine - Font

	Description:
	 Encapsulates a LPD3DXFONT object and manages resource allocation 
	 and deallocation.

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
#include "Font.h"

// --------------------------------------------------------
//	Deprecated: Creates a 20 point arial font
// --------------------------------------------------------
void DirectX::Font::createDefault( )
{
	// Create defualt font
	HRESULT hr = D3DXCreateFont( DirectX::Manager::instance( )->getD3DDev( ), 20, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"arial", &m_font );

	// Add to font list :TODO: locate font error
	DirectX::Manager::instance( )->addResource( this );
}
																																					//
// --------------------------------------------------------
//	Loads a standard windows font with specified settings
// --------------------------------------------------------
void DirectX::Font::createWin32Font( wchar_t *name, unsigned int size, bool bold, bool italic )
{	
	// Set font width
	unsigned long fw;
	if( bold ) fw = FW_NORMAL;
	else fw = FW_BOLD;

	// Create defualt font
	HRESULT hr = D3DXCreateFont( DirectX::Manager::instance( )->getD3DDev( ), size, 0, fw, 0, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, name, &m_font );

	// Add to font list :TODO: locate font error
	DirectX::Manager::instance( )->addResource( this );
}
																																					//
// --------------------------------------------------------
//	Releases the internal DirectX font object
// --------------------------------------------------------
void DirectX::Font::release( )
{
	if( m_font ) {
		// Release font
		m_font->Release( ); m_font = NULL;	
		
		// Remove from manager
		DirectX::Manager::instance( )->deleteResource( this );
	}
}
																																					//
// --------------------------------------------------------
//	Renders text through the the font object
// --------------------------------------------------------
void DirectX::Font::drawText( RECT* rect, const wchar_t* str, unsigned long format, unsigned long color ) 
{
	m_font->DrawTextW( NULL, str, -1, rect, format, color );
}
																																					//
// --------------------------------------------------------
//	Renders text through the the font object
// --------------------------------------------------------
void DirectX::Font::drawText( long x, long y, const wchar_t* str, unsigned long color ) 
{ 
	// Generate rectangle param
	RECT rect; SetRect( &rect, x, y, 0, 0 );
	
	// Draw text
	m_font->DrawTextW( NULL, str, -1, &rect, NoClip, color );
}