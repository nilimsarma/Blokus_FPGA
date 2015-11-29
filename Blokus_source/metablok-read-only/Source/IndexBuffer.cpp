/* ===========================================================================

	Project: DirectX Engine - IndexBuffer

	Description:
	 Encapsulates a LPDIRECT3DINDEXBUFFER9 object and manages resource 
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

// Standard includes
#include "DirectX.h"

// Header include
#include "IndexBuffer.h"

// Constructor
DirectX::IndexBuffer::IndexBuffer( )
{
	indexBuffer = NULL;
	pIndexBuffer = NULL;
}
																																					//
// Destructor
DirectX::IndexBuffer::~IndexBuffer( )
{
	release( );
}
																																					//
// Creates the index buffer
void DirectX::IndexBuffer::create( int nmbFaces, bool dynamic )
{
	// Determine buffer properties 
	D3DPOOL pool = dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED; 
	DWORD usage = dynamic ? D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC : D3DUSAGE_WRITEONLY;

	// Safe buffer creation
	LPDIRECT3DDEVICE9 d3ddev = DirectX::Manager::instance( )->getD3DDev( );
	HRESULT hr = d3ddev->CreateIndexBuffer( nmbFaces*2, usage, D3DFMT_INDEX16, pool, &indexBuffer, NULL );
	if( hr ) { MessageBox( NULL, L"Creation of index buffer has failed.", L"DirectX Engine", NULL ); DebugBreak( ); exit( 0 ); }
	if( pool == D3DPOOL_DEFAULT ) DirectX::Manager::instance( )->addResource( this );

	// Store buffer properties
	indexBuffer->GetDesc( &m_options );
}
																																					//
// Releases memory held by the buffer
void DirectX::IndexBuffer::release( )
{
	// Release the buffer
	if( indexBuffer ) 
	{
		if( m_options.Pool == D3DPOOL_DEFAULT ) 
			DirectX::Manager::instance( )->deleteResource( this );

		// Release the buffer
		indexBuffer->Release( );
		indexBuffer = NULL;
		pIndexBuffer = NULL;
	}

}
																																					//
// Handles device lost operations
void DirectX::IndexBuffer::onDeviceLost( )
{
	indexBuffer->GetDesc( &m_options );

	void* ib;
	indexBuffer->Lock( 0, 0, &ib, D3DLOCK_READONLY );
	pIndexBuffer = (unsigned short*)malloc( m_options.Size );
	memcpy( pIndexBuffer, ib, m_options.Size );
	indexBuffer->Unlock( );
	indexBuffer->Release( );
	indexBuffer = NULL;
}
																																					//
// Handles device reset operations
void DirectX::IndexBuffer::onDeviceReset( )
{
	// Create buffer
	LPDIRECT3DDEVICE9 d3ddev = DirectX::Manager::instance( )->getD3DDev( );
	HRESULT hr = d3ddev->CreateIndexBuffer( m_options.Size, m_options.Usage, m_options.Format, m_options.Pool, &indexBuffer, NULL );
	if( hr ) { MessageBox( NULL, L"Creation of index buffer has failed.", L"DirectX Engine", NULL ); DebugBreak( ); exit( 0 ); }

	void* ib;
	indexBuffer->Lock( 0, 0, &ib, D3DLOCK_DISCARD );
	memcpy( ib, pIndexBuffer, m_options.Size );
	indexBuffer->Unlock( );

	free( pIndexBuffer );
}