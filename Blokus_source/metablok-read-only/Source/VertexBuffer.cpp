/* ===========================================================================

	Project: DirectX Engine - VertexBuffer

	Description:
	 Encapsulates a LPDIRECT3DVERTEXBUFFER9 object and manages resource 
	 allocation and deallocation.

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

// *********************************************
//			  VERTEX FLAGS REFERENCE
// *********************************************
//
//   Vertex Formatting Flags and Sizes
//
// D3DFVF_RESERVED0        0x001
// D3DFVF_POSITION_MASK    0x400E
// D3DFVF_XYZ              0x002	= 3*float
// D3DFVF_XYZRHW           0x004	= 3*float
// D3DFVF_XYZB1            0x006	= 3*float
// D3DFVF_XYZB2            0x008	= 3*float
// D3DFVF_XYZB3            0x00a	= 3*float
// D3DFVF_XYZB4            0x00c	= 3*float
// D3DFVF_XYZB5            0x00e	= 3*float
// D3DFVF_XYZW             0x4002	= 3*float
//
// D3DFVF_NORMAL           0x010	= 3*float
// D3DFVF_PSIZE            0x020	= DWORD
// D3DFVF_DIFFUSE          0x040	= DWORD
// D3DFVF_SPECULAR         0x080	= DWORD
//
// D3DFVF_TEXCOUNT_MASK    0xf00
// D3DFVF_TEXCOUNT_SHIFT   8
// D3DFVF_TEX0             0x000	= 2*DWORD
// D3DFVF_TEX1             0x100	= 2*DWORD
// D3DFVF_TEX2             0x200	= 2*DWORD
// D3DFVF_TEX3             0x300	= 2*DWORD
// D3DFVF_TEX4             0x400	= 2*DWORD
// D3DFVF_TEX5             0x500	= 2*DWORD
// D3DFVF_TEX6             0x600	= 2*DWORD
// D3DFVF_TEX7             0x700	= 2*DWORD
// D3DFVF_TEX8             0x800	= 2*DWORD
//
// *********************************************

// Standard includes
#include "DirectX.h"

// Header include
#include "VertexBuffer.h"

// Constructor
DirectX::VertexBuffer::VertexBuffer( ) 
{
	vertexBuffer = NULL;
	pVertexBuffer = NULL;
}
																																					//
// Destructor
DirectX::VertexBuffer::~VertexBuffer( )
{
	release( );
}
																																					//
// Creates the vertex buffer
void DirectX::VertexBuffer::create( unsigned long FVF, int nmbVerts, bool dynamic )
{
	// Determine buffer properties 
	D3DPOOL pool = dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED; 
	DWORD usage = dynamic ? D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC : D3DUSAGE_WRITEONLY;

	// Calculate vertex stride
	vertexStride =
	((FVF|D3DFVF_XYZ)      == FVF) * 12 +
	((FVF|D3DFVF_XYZRHW)   == FVF) * 12 +
	((FVF|D3DFVF_XYZB1)    == FVF) * 12 +
	((FVF|D3DFVF_XYZB2)    == FVF) * 12 +
	((FVF|D3DFVF_XYZB3)    == FVF) * 12 +
	((FVF|D3DFVF_XYZB4)    == FVF) * 12 +
	((FVF|D3DFVF_XYZB5)    == FVF) * 12 +
	((FVF|D3DFVF_XYZW)     == FVF) * 12 +
	((FVF|D3DFVF_NORMAL)   == FVF) * 12 +
	((FVF|D3DFVF_DIFFUSE)  == FVF) * 4  +
	((FVF|D3DFVF_SPECULAR) == FVF) * 4  +
	((FVF|D3DFVF_TEX1)     == FVF) * 8  +
	((FVF|D3DFVF_TEX2)     == FVF) * 8  +
	((FVF|D3DFVF_TEX3)     == FVF) * 8  +
	((FVF|D3DFVF_TEX4)     == FVF) * 8  +
	((FVF|D3DFVF_TEX5)     == FVF) * 8  +
	((FVF|D3DFVF_TEX6)     == FVF) * 8  +
	((FVF|D3DFVF_TEX7)     == FVF) * 8  +
	((FVF|D3DFVF_TEX8)     == FVF) * 8;

	// Create vertex buffer
	LPDIRECT3DDEVICE9 d3ddev = DirectX::Manager::instance( )->getD3DDev( );
	d3ddev->CreateVertexBuffer( nmbVerts*vertexStride, usage, FVF, pool, &vertexBuffer, NULL );
	if( pool == D3DPOOL_DEFAULT ) DirectX::Manager::instance( )->addResource( this );
	vertexBuffer->GetDesc( &m_desc );
}
void DirectX::VertexBuffer::create( unsigned long FVF, LPDIRECT3DVERTEXBUFFER9 buffer )
{
	// Copy pointer to buffer
	vertexBuffer = buffer;
	vertexBuffer->GetDesc( &m_desc );

	// Calculate vertex stride
	vertexStride =
	((FVF|D3DFVF_XYZ)      == FVF) * 12 +
	((FVF|D3DFVF_XYZRHW)   == FVF) * 12 +
	((FVF|D3DFVF_XYZB1)    == FVF) * 12 +
	((FVF|D3DFVF_XYZB2)    == FVF) * 12 +
	((FVF|D3DFVF_XYZB3)    == FVF) * 12 +
	((FVF|D3DFVF_XYZB4)    == FVF) * 12 +
	((FVF|D3DFVF_XYZB5)    == FVF) * 12 +
	((FVF|D3DFVF_XYZW)	   == FVF) * 12 +
	((FVF|D3DFVF_NORMAL)   == FVF) * 12 +
	((FVF|D3DFVF_DIFFUSE)  == FVF) * 4  +
	((FVF|D3DFVF_SPECULAR) == FVF) * 4  +
	((FVF|D3DFVF_TEX1)     == FVF) * 8  +
	((FVF|D3DFVF_TEX2)     == FVF) * 8  +
	((FVF|D3DFVF_TEX3)     == FVF) * 8  +
	((FVF|D3DFVF_TEX4)     == FVF) * 8  +
	((FVF|D3DFVF_TEX5)     == FVF) * 8  +
	((FVF|D3DFVF_TEX6)     == FVF) * 8  +
	((FVF|D3DFVF_TEX7)     == FVF) * 8  +
	((FVF|D3DFVF_TEX8)     == FVF) * 8;
}
																																					//
// Sets the vertex's x coordinate
void DirectX::VertexBuffer::setVertexPositionX( int vertexID, float pos )
{
	memcpy( (char*)pVertexBuffer+vertexID*vertexStride, &pos, 4 );
}
																																					//
// Sets the vertex's y coordinate
void DirectX::VertexBuffer::setVertexPositionY( int vertexID, float pos )
{
	memcpy( (char*)pVertexBuffer+vertexID*vertexStride+4, &pos, 4 );
}
																																					//
// Sets the vertex's z coordinate
void DirectX::VertexBuffer::setVertexPositionZ( int vertexID, float pos )
{
	memcpy( (char*)pVertexBuffer+vertexID*vertexStride+8, &pos, 4 );
}
																																					//
// Sets the vertex's x coordinate
float DirectX::VertexBuffer::getVertexPositionX( int vertexID )
{
	float pos;
	memcpy( &pos, (char*)pVertexBuffer+vertexID*vertexStride, 4 );
	return pos;
}
																																					//
// Sets the vertex's y coordinate
float DirectX::VertexBuffer::getVertexPositionY( int vertexID )
{
	float pos;
	memcpy( &pos,(char*)pVertexBuffer+vertexID*vertexStride+4, 4 );
	return pos;
}
																																					//
// Sets the vertex's z coordinate
float DirectX::VertexBuffer::getVertexPositionZ( int vertexID )
{
	float pos;
	memcpy( &pos, (char*)pVertexBuffer+vertexID*vertexStride+8, 4 );
	return pos;
}
																																					//
// Returns a string with the vertex fvf information
// Useful for debug display purposes
std::wstring DirectX::VertexBuffer::getFVFFlagString( )
{ 
	std::wstring flagString(L"");
	if ( m_desc.FVF != 0 ) {
		flagString += std::wstring(L"|");
		if ((m_desc.FVF|D3DFVF_XYZ)      == m_desc.FVF) flagString += std::wstring(L"XYZ|");
		if ((m_desc.FVF|D3DFVF_XYZRHW)   == m_desc.FVF) flagString += std::wstring(L"XYZRHW|");
		if ((m_desc.FVF|D3DFVF_XYZB1)    == m_desc.FVF) flagString += std::wstring(L"XYZB1|");
		if ((m_desc.FVF|D3DFVF_XYZB2)    == m_desc.FVF) flagString += std::wstring(L"XYZB2|");
		if ((m_desc.FVF|D3DFVF_XYZB3)    == m_desc.FVF) flagString += std::wstring(L"XYZB3|");
		if ((m_desc.FVF|D3DFVF_XYZB4)    == m_desc.FVF) flagString += std::wstring(L"XYZB4|");
		if ((m_desc.FVF|D3DFVF_XYZB5)    == m_desc.FVF) flagString += std::wstring(L"XYZB5|");
		if ((m_desc.FVF|D3DFVF_XYZW)	  == m_desc.FVF) flagString += std::wstring(L"XYZW|");
		if ((m_desc.FVF|D3DFVF_NORMAL)   == m_desc.FVF) flagString += std::wstring(L"NORM|");
		if ((m_desc.FVF|D3DFVF_DIFFUSE)  == m_desc.FVF) flagString += std::wstring(L"DIFFUSE|");
		if ((m_desc.FVF|D3DFVF_SPECULAR) == m_desc.FVF) flagString += std::wstring(L"SPECULAR|");
		if ((m_desc.FVF|D3DFVF_TEX1)     == m_desc.FVF) flagString += std::wstring(L"TEX1|");
		if ((m_desc.FVF|D3DFVF_TEX2)     == m_desc.FVF) flagString += std::wstring(L"TEX2|");
		if ((m_desc.FVF|D3DFVF_TEX3)     == m_desc.FVF) flagString += std::wstring(L"TEX3|");
		if ((m_desc.FVF|D3DFVF_TEX4)     == m_desc.FVF) flagString += std::wstring(L"TEX4|");
		if ((m_desc.FVF|D3DFVF_TEX5)     == m_desc.FVF) flagString += std::wstring(L"TEX5|");
		if ((m_desc.FVF|D3DFVF_TEX6)     == m_desc.FVF) flagString += std::wstring(L"TEX6|");
		if ((m_desc.FVF|D3DFVF_TEX7)     == m_desc.FVF) flagString += std::wstring(L"TEX7|");
		if ((m_desc.FVF|D3DFVF_TEX8)     == m_desc.FVF) flagString += std::wstring(L"TEX8|");
	} else flagString += std::wstring(L"NO FVF");
	
	return flagString;
}

																																					//
// Performs device lost operations
void DirectX::VertexBuffer::onDeviceLost( )
{
	void* vb; 
	vertexBuffer->Lock( 0, 0, &vb, D3DLOCK_READONLY );
	pVertexBuffer = malloc(m_desc.Size);
	memcpy( pVertexBuffer, vb, m_desc.Size );
	vertexBuffer->Unlock( );
	vertexBuffer->Release( );
	vertexBuffer = NULL;
}
																																					//
// Performs device reset operations
void DirectX::VertexBuffer::onDeviceReset( )
{
	LPDIRECT3DDEVICE9 d3ddev = DirectX::Manager::instance( )->getD3DDev( );
	HRESULT hr = d3ddev->CreateVertexBuffer( m_desc.Size, m_desc.Usage, m_desc.FVF, m_desc.Pool, &vertexBuffer, NULL );
	if( hr ) MessageBox( NULL, L"Creation of vertex buffer has failed.", L"DirectX Engine", NULL );

	void* vb; 
	vertexBuffer->Lock( 0, 0, &vb, 0 );
	memcpy( vb, pVertexBuffer, m_desc.Size );
	vertexBuffer->Unlock( );

	free( pVertexBuffer );
	pVertexBuffer = NULL;
}
																																					//
// Releases the vertex buffer 
void DirectX::VertexBuffer::release( )
{	
	if( vertexBuffer )
	{
		// Remove from manager
		if( m_desc.Pool == D3DPOOL_DEFAULT )
			DirectX::Manager::instance( )->deleteResource( this );

		vertexBuffer->Release( );
		vertexBuffer = NULL;
	}
}