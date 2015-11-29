/* ===========================================================================

	Project: DirectX Engine - Mesh

	Description:
	 Encapsulates a LPD3DXMESH object and manages resource allocation and 
	 deallocation.

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
#include "Mesh.h"
																																					//
// Constructor
DirectX::Mesh::Mesh( ) 
{
	// Nullify pointers
	m_meshInfo = NULL;
	m_attributeTable = NULL;
	m_texture = NULL;
	m_mesh = NULL;

	// Default material
	meshMaterial.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	meshMaterial.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	meshMaterial.Emissive = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
	meshMaterial.Specular = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
	meshMaterial.Power = 0;
}
																																					//
// Creation constructor
DirectX::Mesh::Mesh( wchar_t* filename )
{
}
																																					//
// Creates a DirectX mesh
void DirectX::Mesh::create( void* vertexBuffer, void* indexBuffer, unsigned long FVF, unsigned int nVertices, unsigned int nFaces, unsigned long options )
{
	// Calculate vertex stride
	int vertexStride =
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

	// Create mesh  of sufficient size to store vertex/index buffer data
	HRESULT hr = D3DXCreateMeshFVF( nFaces, nVertices, options, FVF, DirectX::Manager::instance( )->getD3DDev( ), &m_mesh );
	if( hr ) MessageBox( NULL, L"Call to D3DXCreateMeshFVF has failed.", L"DirectX Engine", NULL );
	LPDIRECT3DVERTEXBUFFER9 vb; LPDIRECT3DINDEXBUFFER9 ib; m_mesh->GetVertexBuffer( &vb ); m_mesh->GetIndexBuffer( &ib );
	
	// Lock the ibuffer and load the indices
	char* pVertices; vb->Lock( NULL, NULL, (void**)&pVertices, D3DLOCK_DISCARD );
	memcpy( pVertices, vertexBuffer, nVertices*vertexStride ); vb->Unlock( );

	// Lock the vbuffer and load the vertices
	char* pIndices; ib->Lock( NULL, NULL, (void**)&pIndices, D3DLOCK_DISCARD );
	memcpy( pIndices, indexBuffer, nFaces*3*sizeof(short) ); ib->Unlock( );

	// Register with the manager
	if( !(options&D3DXMESH_SYSTEMMEM) ) 
		DirectX::Manager::instance( )->addResource( this );
}
																																					//
// Creates a DirectX mesh
void DirectX::Mesh::create( unsigned int nFaces, unsigned int nVertices, unsigned long fvf, unsigned long options ) 
{
	// Create the mesh
	HRESULT hr = D3DXCreateMeshFVF( nFaces, nVertices, options, fvf, DirectX::Manager::instance( )->getD3DDev( ), &m_mesh );
	if( hr ) MessageBox( NULL, L"Call to D3DXCreateMeshFVF has failed.", L"DirectX Engine", NULL );

	// Register with the manager
	if( !(options&D3DXMESH_SYSTEMMEM) ) 
		DirectX::Manager::instance( )->addResource( this );
}
																																					//
// Creates a sphere mesh
void DirectX::Mesh::createSphere( float radius, int stacks, int slices )
{
	D3DXCreateSphere( DirectX::Manager::instance( )->getD3DDev( ), radius, slices, stacks, &m_mesh, NULL );
	update( );
	
	// Register with the manager
	DirectX::Manager::instance()->addResource( this );
}
																																					//
// Creates a polygon mesh
void DirectX::Mesh::createPolygon( float length, unsigned int sides )
{
	D3DXCreatePolygon( DirectX::Manager::instance( )->getD3DDev( ), length, sides, &m_mesh, NULL );	
	update( );
	
	// Register with the manager
	DirectX::Manager::instance()->addResource( this );
}
																																					//
// Creates a box mex
void DirectX::Mesh::createBox( float width, float height, float depth )
{
	D3DXCreateBox( DirectX::Manager::instance( )->getD3DDev( ), width, height, depth, &m_mesh, NULL );
	update( );
	
	// Register with the manager
	DirectX::Manager::instance()->addResource( this );
}
																																					//
// Sets one of the mesh textures
void DirectX::Mesh::setTexture( Image* texture, int subset )
{
	m_texture = texture;
}
																																					//
// Sets the color of a mesh subset
void DirectX::Mesh::setColor( float r, float g, float b, float a )
{
	D3DXCOLOR clr = D3DXCOLOR( r, g, b, meshMaterial.Ambient.a );
	meshMaterial.Ambient = clr;
	meshMaterial.Diffuse = clr;
}
																																					//
// Releases mesh data
void DirectX::Mesh::release( )
{
	// Delete the mesh
	if( m_mesh ) 
	{
		// Check for manager removal
		unsigned long options = m_mesh->GetOptions( );
		if( !(options&D3DXMESH_SYSTEMMEM) ) 
			DirectX::Manager::instance()->deleteResource( this );

		m_mesh->Release( ); m_mesh = NULL; 
	}
	if( m_attributeTable ) { delete[] m_attributeTable; m_attributeTable = NULL; }

	// Nullify pointers
	m_attributeTable = NULL;
	m_meshInfo = NULL;
	m_texture = NULL;
	m_mesh = NULL;

	// Default material
	meshMaterial.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	meshMaterial.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	meshMaterial.Emissive = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
	meshMaterial.Specular = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
	meshMaterial.Power = 0;
}
																																					//
// Saves mesh data on device loss
void DirectX::Mesh::onDeviceLost( )
{
	// Check if the mesh needs to be released
	unsigned long options = m_mesh->GetOptions( );
	if( !((options|D3DXMESH_SYSTEMMEM) == options) && 
		!((options|D3DXMESH_MANAGED) == options) ) {
			// Lock buffer data
			void *vb, *ib;
			m_mesh->LockVertexBuffer( D3DLOCK_READONLY, &vb );
			m_mesh->LockIndexBuffer( D3DLOCK_READONLY, &ib );

			unsigned long fvf = m_mesh->GetFVF( );
			unsigned long stride = m_mesh->GetNumBytesPerVertex( );
			unsigned int nVertices = m_mesh->GetNumVertices( );
			unsigned int nIndices = m_mesh->GetNumFaces( ) / 3;
			unsigned int vbSize = stride * nVertices;
			unsigned int ibSize = nIndices * sizeof(unsigned short);

			m_meshInfo = malloc( 3*sizeof(long) + 4*sizeof(int) + vbSize + ibSize ); 

			char* infoIter = (char*)m_meshInfo;
			*(unsigned long*)infoIter = fvf; infoIter += sizeof(long);
			*(unsigned long*)infoIter = stride; infoIter += sizeof(long);
			*(unsigned long*)infoIter = options; infoIter += sizeof(long);
			*(unsigned int*)infoIter = nVertices; infoIter += sizeof(int);
			*(unsigned int*)infoIter = nIndices; infoIter += sizeof(int);
			*(unsigned int*)infoIter = vbSize; infoIter += sizeof(int);
			*(unsigned int*)infoIter = ibSize; infoIter += sizeof(int);
			memcpy( infoIter, vb, vbSize ); infoIter += vbSize;
			memcpy( infoIter, ib, ibSize ); infoIter += ibSize;

			// Release mesh
			m_mesh->UnlockVertexBuffer( );
			m_mesh->UnlockIndexBuffer( );
			m_mesh->Release( ); m_mesh = NULL;
	}
}
																																					//
// Loads mesh after device loss
void DirectX::Mesh::onDeviceReset( )
{
	if( m_meshInfo ) {
		char* infoIter = (char*)m_meshInfo;
		unsigned long fvf = *(unsigned long*)infoIter; infoIter += sizeof(long);
		unsigned long stride = *(unsigned long*)infoIter; infoIter += sizeof(long);
		unsigned long options = *(unsigned long*)infoIter; infoIter += sizeof(long);
		unsigned int nVertices = *(unsigned int*)infoIter; infoIter += sizeof(int);
		unsigned int nIndices = *(unsigned int*)infoIter; infoIter += sizeof(int);
		unsigned int vbSize = *(unsigned int*)infoIter; infoIter += sizeof(int);
		unsigned int ibSize = *(unsigned int*)infoIter; infoIter += sizeof(int);
				
		// Create the mesh
		HRESULT hr = D3DXCreateMeshFVF( nIndices*3, nVertices, options, fvf, DirectX::Manager::instance( )->getD3DDev( ), &m_mesh );
		if( hr ) MessageBox( NULL, L"Call to D3DXCreateMeshFVF has failed.", L"DirectX Engine", NULL );

		void *vb, *ib;
		m_mesh->LockVertexBuffer( D3DLOCK_READONLY, &vb );
		m_mesh->LockIndexBuffer( D3DLOCK_READONLY, &ib );

		memcpy( vb, infoIter, vbSize ); infoIter += vbSize;
		memcpy( ib, infoIter, ibSize ); infoIter += ibSize;

		free(m_meshInfo);
	}
}
																																					//
// Renders the mesh
void DirectX::Mesh::render( int subset )
{
	// Set the device material
	DirectX::Manager::instance( )->getD3DDev( )->SetMaterial( &meshMaterial );

	// Set the device texture
	if( m_texture == NULL )
		DirectX::Manager::instance( )->getD3DDev( )->SetTexture( 0, NULL );
	else DirectX::Manager::instance( )->getD3DDev( )->SetTexture( 0, m_texture->get( ) );

	// Render the mesh
	m_mesh->DrawSubset( 0 );
}
																																					//
// Updates the mesh by calling the optimizeInPlace method
// of the D3DX object and reloading attribute information
void DirectX::Mesh::update( )
{
	DWORD* adjacencyInfo = new DWORD[m_mesh->GetNumFaces( )*sizeof(DWORD)];
	m_mesh->ConvertPointRepsToAdjacency( NULL, adjacencyInfo );
	m_mesh->OptimizeInplace( D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT, adjacencyInfo, NULL, NULL, NULL );
	delete[] adjacencyInfo;

	if( m_attributeTable ) delete[] m_attributeTable;
	m_mesh->GetAttributeTable( NULL, &m_nAttributes );
	m_attributeTable = new D3DXATTRIBUTERANGE[m_nAttributes];
	m_mesh->GetAttributeTable( m_attributeTable, &m_nAttributes );

	calculateBoundingSphere( );
	calculateBoundingBox( );
}
																																					//
// Computes the bounding box for frustum culling
void DirectX::Mesh::calculateBoundingBox( ) 
{ 
	void* vertices;

	// Lock vertex data for read operation
	m_mesh->LockVertexBuffer( D3DLOCK_READONLY, &vertices );

	D3DXComputeBoundingBox( (D3DXVECTOR3*)vertices, m_mesh->GetNumVertices(), m_mesh->GetNumBytesPerVertex(), &m_boxMin, &m_boxMax );

	// Unlock buffer
	m_mesh->UnlockVertexBuffer( );
}
																																					//
// Computes the bounding sphere for frustum culling
void DirectX::Mesh::calculateBoundingSphere( ) 
{
	void* vertices;

	// Lock vertex data for read operation
	m_mesh->LockVertexBuffer( D3DLOCK_READONLY, &vertices );

	D3DXComputeBoundingSphere( (D3DXVECTOR3*)vertices, m_mesh->GetNumVertices(), m_mesh->GetNumBytesPerVertex(), &m_origin, &m_radius );

	// Unlock buffer
	m_mesh->UnlockVertexBuffer( );
}
																																						//
// Creates a copy of the mesh which has no image or material specifications
DirectX::Mesh* DirectX::Mesh::cloneOutline( Mesh* out )
{
	if( !out ) out = new Mesh( ); out->release( );

	unsigned int nVertices = m_mesh->GetNumVertices( );
	unsigned int nFaces = m_mesh->GetNumFaces( );
	unsigned long options = m_mesh->GetOptions( );
	unsigned int fvf = m_mesh->GetFVF( );

	void* vertices = lockVertexBuffer( D3DLOCK_READONLY );
	void* indices = lockIndexBuffer( D3DLOCK_READONLY );
	out->create( vertices, indices, fvf, nVertices, nFaces, options );
	unlockVertexBuffer( ); unlockIndexBuffer( );

	out->update( ); return out;
}
//
// Creates a mesh from the specified file if it exists
void DirectX::Mesh::create( const wchar_t* filename )
{
	unsigned long nmbMaterials; LPD3DXBUFFER materialBuffer;
	D3DXLoadMeshFromX( filename, D3DXMESH_MANAGED, DirectX::Manager::instance( )->getD3DDev( ), NULL, &materialBuffer, NULL, &nmbMaterials, &m_mesh );
	materialBuffer->Release( ); update( );
}