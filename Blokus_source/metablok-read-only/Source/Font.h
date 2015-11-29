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

// Begin definition
#ifndef DX_ENGINE_FONT
#define DX_ENGINE_FONT

// DirectX engine namespace
namespace DirectX
{
	// Font encapsulation class
	class Font : public Resource
	{
	public:
		enum Alignment { Left = 0x0, Right = 0x2, Top = 0x0, Bottom = 0x8, Center = 0x1, VCenter = 0x4 };
		enum Formatting { NoClip = 0x100, WordBreak = 0x10, SingleLine = 0x20 };

		// Constructor and destructor
		Font( ) { m_font = NULL; } ~Font( ) { release( ); }

		// Creation
		void createWin32Font( wchar_t* name, unsigned int size, bool bold = false, bool italic = false );
		void createDefault( );

		// D3Dptr accessor
		LPD3DXFONT getFont( ) { return m_font; }

		// Returns true if the font exists 
		bool created( ) { return (m_font != NULL); }

		// Font drawing interface
		void drawText( long x, long y, const wchar_t* str, unsigned long color );
		void drawText( RECT* rect, const wchar_t* str, unsigned long format, unsigned long color );

		// Batched font drawing
		void drawTextEx( long x, long y, const wchar_t* str, unsigned long color );
		void drawTextEx( RECT* rect, const wchar_t* str, unsigned long format, unsigned long color );

		// Device state change
		void onDeviceLost( ) { m_font->OnLostDevice( ); }
		void onDeviceReset( ) { m_font->OnResetDevice( ); }
		void release( );

	protected:
		LPD3DXFONT m_font;
	};
}

// End definition
#endif                                                                                                                                           