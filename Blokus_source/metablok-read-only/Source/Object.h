/* ===========================================================================

	Project: DirectX Engine - Object

	Description:
	 Manages the rendering of common meshes with different transforms.

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
#ifndef DX_ENGINE_OBJECT
#define DX_ENGINE_OBJECT
		
// DirectX namespace
namespace DirectX
{

	// Mesh instance encapsulating class
	class Object : public Renderable
	{
	public:
		// Constructor and Destructor
		Object( ); ~Object( );

		// Creation constructors
		Object( Mesh* instance );

		// Creation
		void create( Mesh* instance, bool managed = true );

		// Deletion
		void release( );

		// Rotation
		void rotate( float x, float y, float z ) { m_rotation.x += x; m_rotation.y += y; m_rotation.z += z; }
		void setRotation( float x, float y, float z ) { m_rotation.x = x; m_rotation.y = y; m_rotation.z = z; }
		void setRotation( D3DXVECTOR3* rotationVect );
		D3DXVECTOR3* getRotation( ) { return &m_rotation; }
		float getAngleX( ) { return m_rotation.x; }
		float getAngleY( ) { return m_rotation.y; }
		float getAngleZ( ) { return m_rotation.z; }

		// Scale
		void scale( float x, float y, float z ) { m_objScale.x *= x; m_objScale.y *= y; m_objScale.z *= z; }
		void scale( D3DXVECTOR3* scale ) { m_objScale.x *= scale->x; m_objScale.y *= scale->y; m_objScale.z *= scale->z; }
		void setScale( float x, float y, float z ) { m_objScale = D3DXVECTOR3( x, y, z ); }
		void setScale( D3DXVECTOR3* scale ) { m_objScale = *scale; }
		D3DXVECTOR3* getScale( ) { return &m_objScale; }
		float getScaleX( ) { return m_objScale.x; }
		float getScaleY( ) { return m_objScale.y; }
		float getScaleZ( ) { return m_objScale.z; }

		// Position
		void move( float x, float y, float z ) { m_position.x += x; m_position.y += y; m_position.z += z; }
		void move( D3DXVECTOR3* move ) { D3DXVec3Add( &m_position, &m_position, move ); }
		void setPosition( float x, float y, float z ) { m_position.x = x; m_position.y = y; m_position.z = z; }
		void setPosition( D3DXVECTOR3* posVect ) { m_position = *posVect; }
		D3DXVECTOR3* getPosition( ) { return &m_position; }
		float getPositionX( ) { return m_position.x; }
		float getPositionY( ) { return m_position.y; }
		float getPositionZ( ) { return m_position.z; }

		// Ray intersection testing
		void intersectRay( DirectX::Vector3* rayOrigin, DirectX::Vector3* rayDir, int* hit, unsigned long* face, float* bary1, float* bary2, float* dist );

		// Renders the object
		void render( );

		// Mesh accessor
		void setMesh( Mesh* newMesh ) { m_mesh = newMesh; }
		Mesh* getMesh( ) { return m_mesh; }

		// Bounding mesh accessors
		Vector3* getBoxMin( ) { return &m_boxMin; }
		Vector3* getBoxMax( ) { return &m_boxMax; }
		Vector3* getSphereOrigin( ) { return &m_sphereOrigin; }
		float getSphereRadius( ) { return m_sphereRadius; }

		// Visibility 
		bool isVisible( ) { return m_visible; }
		void hide( ) { m_visible = false; }
		void show( ) { m_visible = true; }

		// World transform for shader
		D3DXMATRIX* getTransform( ) { return &m_transform; }
		void setTransform( Matrix* transform ) { m_transform = *transform; }

		// Updates object transforms
		void updateTransform( );
		void updateBounds( );
		void update( );
		
	private:
		D3DXVECTOR3 m_boxMin;
		D3DXVECTOR3 m_boxMax;
		D3DXVECTOR3 m_sphereOrigin;
		float m_sphereRadius;

		D3DXVECTOR3 m_rotation;
		D3DXVECTOR3 m_position;
		D3DXVECTOR3 m_objScale;
		D3DXMATRIX m_transform;

		bool m_managed;
		bool m_visible;
		Mesh *m_mesh;

	};

}
																																					//
// End definition
#endif