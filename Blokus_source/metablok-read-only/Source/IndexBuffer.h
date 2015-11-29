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

// Begin definition
#ifndef DX_ENGINE_INDEX_BUFFER
#define DX_ENGINE_INDEX_BUFFER

// DirectX namespace
namespace DirectX
{

	// Vertex buffer encapsulation class
	class IndexBuffer : public Resource
	{

	public:
		// Constructors and Destructor
		IndexBuffer( ); virtual ~IndexBuffer( );

		// Creation functions
		void create( int nmbFaces, bool dynamic );
		void create( LPDIRECT3DINDEXBUFFER9 buffer );

		// Dx data accessor and type conversion function
		LPDIRECT3DINDEXBUFFER9 get( ) { return indexBuffer; }

		// Data accessors
		void setIndex( int indexNum, USHORT vertexNum ) { pIndexBuffer[indexNum] = vertexNum; }
		USHORT getIndex( int indexNum ) { return pIndexBuffer[indexNum]; }

		// Device state operations
		void onDeviceReset( );
		void onDeviceLost( );
		void release( );

		// Data management
		USHORT* lock( DWORD flag ) { indexBuffer->Lock( 0, 0, (void**)&pIndexBuffer, D3DLOCK_DISCARD ); return pIndexBuffer; }
		void unlock( ) { indexBuffer->Unlock( ); pIndexBuffer = NULL; }
		bool isLocked( ) { if( pIndexBuffer ) return true; return false; }

	protected:
		LPDIRECT3DINDEXBUFFER9 indexBuffer;
		unsigned short* pIndexBuffer;
		D3DINDEXBUFFER_DESC m_options;

	};

}

// End definition
#endif