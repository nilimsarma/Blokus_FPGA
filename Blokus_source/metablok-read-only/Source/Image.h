/* ===========================================================================

	Project: DirectX Engine - Image

	Description:
	 Encapsulates a LPDIRECT3DTEXTURE9 object and manages resource allocation 
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
#ifndef DX_ENGINE_IMAGE
#define DX_ENGINE_IMAGE

// Color component retrieval
#define getA(c) ( ((c)&0xff000000)>>24 )
#define getR(c) ( ((c)&0x00ff0000)>>16 )
#define getG(c) ( ((c)&0x0000ff00)>>8  )
#define getB(c) ( ((c)&0x000000ff)>>0  )

// DirectX namespace
namespace DirectX
{
	// Encapsulating class
	class Image : public Resource
	{

	public:
		// Constructors and destructor
		Image( ); ~Image( ) { release( ); }

		// Creation constructors
		Image( int width, int height, int depth );
		Image( const wchar_t* filename );

		// Creation
		void create( int width, int height, int depth, DWORD usage = D3DUSAGE_DYNAMIC );
		void create( const wchar_t* filename, unsigned long colorFilter = 0 );

		// Returns true if image exists
		bool created( ) { return (m_image != NULL); }

		// Saving images to file
		void saveToFile( const wchar_t* filename, D3DXIMAGE_FILEFORMAT format );

		// Direct3D object accessor
		LPDIRECT3DTEXTURE9 get( ) { return m_image; }

		// Drawing commands
		void drawCircle( int xCenter, int yCenter, int radius, DWORD clr, bool fill );
		void drawBox( int x1, int y1, int x2, int y2, DWORD clr, bool fill );
		void setColor( int x, int y, DWORD clr );
		DWORD getColor( int x, int y );

		// Surface editing
		D3DLOCKED_RECT* lock( );
		D3DLOCKED_RECT* lock( RECT* rectangle );
		void unlock( );

		// Image data accessors
		int width( ) { return m_desc.Width; }
		int height( ) { return m_desc.Height; }
		const D3DXVECTOR2* scale( ) { return &m_scaling; }
		D3DFORMAT format( ) { return m_desc.Format; }

		LPDIRECT3DTEXTURE9 getImage( ) { return m_image; }
		void setTexture( LPDIRECT3DTEXTURE9 texture ) { m_image = texture; m_image->GetLevelDesc( 0, &m_desc ); }

		// Device state operations
		void onDeviceLost( );
		void onDeviceReset( );
		void release( );

	private:
		D3DXVECTOR2 m_scaling;			 // Image scale vector from load
		D3DSURFACE_DESC m_desc;			 // Stores basic image data
		D3DLOCKED_RECT m_imgData;		 // Locked rectangle for manipulation
		LPDIRECT3DTEXTURE9 m_image;		 // Pointer to the D3Ddata structure for this image
		short m_lockHeight, m_lockWidth; // Locked rectangle dimensions

	};

}
																																					//
// End definition
#endif