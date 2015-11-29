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

// Begin definition
#ifndef DX_ENGINE_VERTEX_BUFFER
#define DX_ENGINE_VERTEX_BUFFER

// DirectX namespace
namespace DirectX
{																																						//
	// Vertex buffer encapsulation class
	class VertexBuffer : public Resource
	{

	public:
		// Constructor and destructor
		VertexBuffer( ); virtual ~VertexBuffer( );

		// Creation constructors
		VertexBuffer( unsigned long FVF, int nmbVerts, bool dynamic );

		// Creation and destruction
		void create( unsigned long FVF, int nmbVerts, bool dynamic );
		void create( unsigned long FVF, LPDIRECT3DVERTEXBUFFER9 buffer );

		// Dx object accessor
		LPDIRECT3DVERTEXBUFFER9 get( ) { return vertexBuffer; }

		// Direct memory access commands
		void* lock( DWORD flags = D3DLOCK_DISCARD ) { vertexBuffer->Lock( 0, 0, &pVertexBuffer, flags ); return pVertexBuffer; }
		void unlock( ) { vertexBuffer->Unlock( ); pVertexBuffer = NULL; }

		// Vertex data accessors and modifiers
		void setVertexPosition( int vertexID, D3DXVECTOR3* pos );
		void setVertexPositionX( int vertexID, float pos );
		void setVertexPositionY( int vertexID, float pos );
		void setVertexPositionZ( int vertexID, float pos );
		D3DXVECTOR3* getVertexPosition( int vertexID );
		float getVertexPositionX( int vertexID );
		float getVertexPositionY( int vertexID );
		float getVertexPositionZ( int vertexID );

		void setVertexNormal( int vertexID, D3DXVECTOR3* pos );
		void setVertexNormalX( int vertexID, float pos );
		void setVertexNormalY( int vertexID, float pos );
		void setVertexNormalZ( int vertexID, float pos );

		void setVertexTextureCoords( int vertexID, D3DXVECTOR2* pos );
		void setVertexTextureU( int vertexID, int pos );
		void setVertexTextureV( int vertexID, int pos );

		int getVertexStride( ) { return vertexStride; }

		std::wstring getFVFFlagString( );
		int getFVF( ) { return m_desc.FVF; }
		unsigned int getSize( ) { return m_desc.Size; }

		// Device state changes
		void onDeviceReset( );
		void onDeviceLost( );
		void release( );

	protected:
		LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
		D3DVERTEXBUFFER_DESC m_desc;
		int vertexStride;

		void* pVertexBuffer;

	};

}

// End definition
#endif