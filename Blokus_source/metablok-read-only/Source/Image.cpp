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

// Standard includes
#include "DirectX.h"

// Header include
#include "Image.h"

// --------------------------------------------------------
//	Zeros the image data pointer so existance checks return
//  false properly.
// --------------------------------------------------------
DirectX::Image::Image( )
{
	// Zero image data
	m_image = NULL;
}
																																					//
// --------------------------------------------------------
//	Create on construction functions
// --------------------------------------------------------
DirectX::Image::Image( const wchar_t* filename )
{	
	m_image = NULL;
	create( filename );
}
DirectX::Image::Image( int width, int height, int depth )
{
	m_image = NULL;
	create( width, height, depth );
}
																																					//
// --------------------------------------------------------
//	Creates a new image of the specified dimensions.
// --------------------------------------------------------
void DirectX::Image::create( int width, int height, int depth, DWORD usage )
{
	D3DPOOL pool = D3DPOOL_DEFAULT;
	if( usage == D3DUSAGE_DYNAMIC ) pool = D3DPOOL_MANAGED; 
	if( DirectX::Manager::instance( )->getD3DDev( )->CreateTexture( width, height, 1, usage, D3DFMT_A8R8G8B8,
									pool, &m_image, NULL ) )
									int apple = 5;

	// Load level0 desc
	m_image->GetLevelDesc(0,&m_desc);	

	// Set scale to none
	m_scaling = D3DXVECTOR2( 1.0f, 1.0f );
}
																																					//
// --------------------------------------------------------
//	Creates a new image from the specified file. Filename
//	is relative to current directory. DirectX supported image
//	types are:
//		.bmp - 32bit bitmap
//		.jpg - jpeg files
//		.ppm - ppm files
//		.tga - targa files
//		.png - png files
//		.dds - direct draw surface
//		.hdr - high dynamic range
//		.dib - dib files
//		.pfm - pmf files
// --------------------------------------------------------
void DirectX::Image::create( const wchar_t* filename, unsigned long colorFilter )
{
	// Image info structure
	D3DXIMAGE_INFO m_info;

	// Load the image 
	HRESULT hr = D3DXCreateTextureFromFileEx( DirectX::Manager::instance( )->getD3DDev( ), filename, 0, 0, 1, 0, D3DFMT_A8B8G8R8,
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, colorFilter, &m_info, 0, &m_image );
	if( hr ) MessageBox( NULL, L"Failed to load image.", L"DirectX Engine", NULL );

	// Load level0 desc
	m_image->GetLevelDesc(0,&m_desc);	

	m_scaling.x=(float)m_info.Width/(float)m_desc.Width;
	m_scaling.y=(float)m_info.Height/(float)m_desc.Height;
}
//
// --------------------------------------------------------
//	Saves a new as the specified file type. Filename
//	is relative to current directory and supported image
//	types are:
//		.bmp - 32bit bitmap
//		.jpg - jpeg files
//		.ppm - ppm files
//		.tga - targa files
//		.png - png files
//		.dds - direct draw surface
//		.hdr - high dynamic range
//		.dib - dib files
//		.pfm - pmf files
// --------------------------------------------------------
void DirectX::Image::saveToFile( const wchar_t* filename, D3DXIMAGE_FILEFORMAT format )
{
	// Save the texture to the specified file
	D3DXSaveTextureToFile( filename, format, m_image, NULL );
}
//
// --------------------------------------------------------
//	Locks the image data so it can be edited in sys mem
// --------------------------------------------------------
D3DLOCKED_RECT* DirectX::Image::lock( )
{ 
	m_image->LockRect( 0, &m_imgData, 0, 0 );
	m_lockHeight = m_desc.Height;
	m_lockWidth = m_desc.Width;
	return &m_imgData;
}
//
// --------------------------------------------------------
//	Locks the image data so it can be edited in sys mem
// --------------------------------------------------------
D3DLOCKED_RECT* DirectX::Image::lock( RECT* rectangle )
{ 
	m_image->LockRect( 0, &m_imgData, rectangle, 0 );
	m_lockWidth = (short)(rectangle->right - rectangle->left);
	m_lockHeight = (short)(rectangle->bottom - rectangle->top);
	return &m_imgData;
}
//
// --------------------------------------------------------
//	Unlocks the image data and updates the surface
// --------------------------------------------------------
void DirectX::Image::unlock( )
{ m_image->UnlockRect( 0 ); }
//
// --------------------------------------------------------
//	The image must be locked for this command to succeed
//	sets the color of the point (x,y) on the image to clr
// --------------------------------------------------------
void DirectX::Image::setColor( int x, int y, DWORD clr )
{
	if( x>=0 && y>=0 && x<(int)m_lockWidth && y<(int)m_lockHeight )
	*( (DWORD*)m_imgData.pBits + y * m_imgData.Pitch/sizeof(DWORD) + x ) = clr;
}
//
// --------------------------------------------------------
//	The image must be locked for this command to succeed
//	returns the color of the point (x,y) on the image
// --------------------------------------------------------
DWORD DirectX::Image::getColor( int x, int y )
{ return *( (DWORD*)m_imgData.pBits + y * m_imgData.Pitch/sizeof(DWORD) + x ); }
//
// --------------------------------------------------------
//	The image must be locked for this command to succeed
//	draws a circle at (xCenter,yCenter) of the specified
//	color. If the fill flag is true the circle will be filled
// --------------------------------------------------------
void DirectX::Image::drawCircle( int xCenter, int yCenter, int radius, DWORD clr, bool fill )
{
	int x=0, y=radius;
	int d=3-(2*radius);

	// For each horizontal stage
    while(x<=y){
		// Edge points
		setColor( xCenter+x, yCenter+y, clr );
		setColor( xCenter+y, yCenter+x, clr );
		setColor( xCenter-x, yCenter+y, clr );
		setColor( xCenter+y, yCenter-x, clr );
		setColor( xCenter-x, yCenter-y, clr );
		setColor( xCenter-y, yCenter-x, clr );
		setColor( xCenter+x, yCenter-y, clr );
		setColor( xCenter-y, yCenter+x, clr );
		// Inner fill
		if (fill) {	

			for( int x1 = xCenter-x; x1<=xCenter+x; x1++ )
				setColor( x1, yCenter+y, clr );
			for( int x1 = xCenter-x; x1<=xCenter+x; x1++ )
				setColor( x1, yCenter-y, clr );
			for( int x1 = xCenter-y; x1<=xCenter+y; x1++ )
				setColor( x1, yCenter+x, clr );
			for( int x1 = xCenter-y; x1<=xCenter+y; x1++ )
				setColor( x1, yCenter-x, clr );
		}
		if (d<0)
			d += (x << 2)+6;
		else
		{
			d += ((x-y) << 2)+10;
			y -= 1;
		}
		x++;
	}
}
//
// --------------------------------------------------------
//	The image must be locked for this command to succeed
//	draws a box from (x1,y1) to (x2,y2) of the specified
//	color. If the fill flag is true the box will be filled
// --------------------------------------------------------
void DirectX::Image::drawBox(int x1, int y1, int x2, int y2, DWORD clr, bool fill )
{
	if (fill) {
		for( int x=x1; x<=x2; x++ )
		for( int y=y1; y<=y2; y++ )
			setColor( x, y, clr );
	} else
	{
		for( int x=x1; x<=x2; x++ )
			{ setColor( x, y1, clr ); setColor( x, y2, clr ); }
		for( int y=y1; y<=y2; y++ )
			{ setColor( x1, y, clr ); setColor( x2, y, clr ); }
	}
}
//
// --------------------------------------------------------
//	Releases the direct3D image object if it exists and 
//  removes the object from the manager
// --------------------------------------------------------
void DirectX::Image::release( )
{
	// Verify image created
	if( m_image ) 
	{
		// Free memory for device reset
		if ( m_desc.Pool = D3DPOOL_DEFAULT )
		{
			// Remove from manager
			DirectX::Manager::instance( )->deleteResource( this );
		}
		
		// Release D3Dobj
		m_image->Release( );
		m_image = NULL;
	}
}
//
// --------------------------------------------------------
//	Handles data backup to sys mem if the device is lost
//	also frees image data in mem so device can be reset
//  when ready
// --------------------------------------------------------
void DirectX::Image::onDeviceLost( )
{
	// Free memory for device reset
	if ( m_desc.Pool = D3DPOOL_DEFAULT )
	{
		if( m_imgData.pBits != NULL ) m_image->UnlockRect( 0 ); 

		D3DLOCKED_RECT tImageData;
		m_image->LockRect( 0, &tImageData, 0, 0 );
		memcpy( m_imgData.pBits, tImageData.pBits, m_imgData.Pitch/sizeof(DWORD) * m_desc.Height );
		m_image->UnlockRect( 0 );
		m_image->Release( );
	}

}
//
// --------------------------------------------------------
//	Restores the image object from the sys mem backup
//  if the image was in the appropriate pool when the dev
//	was lost
// --------------------------------------------------------
void DirectX::Image::onDeviceReset( )
{
}