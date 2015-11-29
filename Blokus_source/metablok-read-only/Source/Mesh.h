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

// Begin definition
#ifndef DX_ENGINE_MESH
#define DX_ENGINE_MESH

// DirectX namespace
namespace DirectX
{

	// Mesh encapsulating class
	class Mesh : public Resource
	{

	public:
		// Constructor / Destructor
		Mesh( ); virtual ~Mesh( ) { release( ); }

		// Creation constructor
		Mesh( wchar_t* filename );
		Mesh( unsigned int nFaces, unsigned int nVertices, unsigned long options, unsigned long FVF );
		Mesh( void* vertexBuffer, void* indexBuffer, unsigned long FVF, unsigned int nVertices, unsigned int nFaces );

		// Creation
		void create( const wchar_t* filename );
		void create( unsigned int nFaces, unsigned int nVertices, unsigned long fvf, unsigned long options );
		void create( void* vertexBuffer, void* indexBuffer, unsigned long FVF, unsigned int nVertices, unsigned int nFaces, unsigned long options );

		// Copying and optimization
		Mesh* cloneOutline( Mesh* out = NULL );
		Mesh* cloneTextured( Mesh* out = NULL, unsigned long options = 0xffffffff, unsigned long fvf = 0 );
		Mesh* cloneAll( Mesh* out = NULL, unsigned long options = 0xffffffff, unsigned long fvf = 0 );

		// :TODO: Transfer to DirectX::D3DX Namespace as functions
		void createPolygon( float length, unsigned int sides );
		void createSphere( float radius, int stacks, int slices );
		void createCone( float radius, float height );
		void createCylinder( float radius1, float radius2, float length, int slices, int stacks );
		void createBox( float width, float height, float depth );
		void createCube( float size );
		void createTorus( float radius1, float radius2 );

		// D3DMesh accessors
		LPD3DXMESH getMesh( ) { return m_mesh; }
		unsigned long getNumVertices( ) { return m_mesh->GetNumVertices( ); }
		unsigned long getNumFaces( ) { return m_mesh->GetNumFaces( ); }

		// Render method
		void render( int subset = 0 );

		// Lock data buffers
		void* lockVertexBuffer( DWORD flags = NULL ) { void* pb; m_mesh->LockVertexBuffer( flags, &pb ); return pb; }
		void* lockIndexBuffer( DWORD flags = NULL ) { void* pb; m_mesh->LockIndexBuffer( flags, &pb ); return pb; }
		unsigned long* lockAttributeBuffer( DWORD flags = NULL ) { unsigned long* pb; m_mesh->LockAttributeBuffer( flags, &pb ); return pb; }
		void unlockVertexBuffer( ) { m_mesh->UnlockVertexBuffer( ); }
		void unlockIndexBuffer( ) { m_mesh->UnlockIndexBuffer( ); }
		void unlockAttributeBuffer( ) { m_mesh->UnlockAttributeBuffer( ); }

		// Texture accessors
		void setTexture( Image* texture, int subset = 0 );
		Image* getTexture( int subset = 0 ) { return m_texture; }

		// Attribute accessors
		void getAttributeTable( D3DXATTRIBUTERANGE* table, int* size )
		{ table = m_attributeTable; *size = m_nAttributes; }

		// Material modifiers
		void setColor( float r, float g, float b, float a = 1.0f );
		void setMaterial( D3DMATERIAL9* material, int subset = 0 );
		const D3DMATERIAL9* getMaterial( int subset = 0 ); 

		// Bounding mesh accessors
		Vector3* getBoxMin( ) { return &m_boxMin; }
		Vector3* getBoxMax( ) { return &m_boxMax; }
		Vector3* getSphereOrigin( ) { return &m_origin; }
		float getSphereRadius( ) { return m_radius; }

		// Resource methods
		void onDeviceLost( );
		void onDeviceReset( );
		void release( );

		// Update mesh
		void update( );

	protected:
		LPD3DXMESH m_mesh;
		D3DMATERIAL9 meshMaterial;
		D3DXVECTOR3 m_origin;
		D3DXVECTOR3 m_boxMin;
		D3DXVECTOR3 m_boxMax;
		float m_radius;
		
		// Bounding volumes generation
		void calculateBoundingSphere( );
		void calculateBoundingBox( );

		// Mesh attribute data
		D3DXATTRIBUTERANGE *m_attributeTable;
		unsigned long m_nAttributes;

		// Device loss info
		void *m_meshInfo;

		// Mesh texture
		Image* m_texture;
	};

}

// End definition
#endif