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

// Standard includes
#include "DirectX.h"

// Header include
#include "Object.h"

// Constructor
DirectX::Object::Object( )
{
	// Default to identity transform
	D3DXMatrixIdentity( &m_transform );
	m_rotation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_position = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_objScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );

	m_visible = true;
	m_mesh = NULL;
}
																																					//
// Destructor
DirectX::Object::~Object( )
{
	release( );
}
																																					//
// Creation constructor
DirectX::Object::Object( Mesh* instance )
{
	// Set transform to identity
	D3DXMatrixIdentity( &m_transform );

	// Initialize vectors
	m_rotation = D3DXVECTOR3( 0, 0, 0 );
	m_position = D3DXVECTOR3( 0, 0, 0 );
	m_objScale = D3DXVECTOR3( 1, 1, 1 );

	// Set mesh
	m_visible = true;
	m_mesh = instance;

	// Allow the object to be rendered
	DirectX::Manager::instance( )->addRenderable( this );
}
																																					//
// Creation
void DirectX::Object::create( Mesh *instance, bool managed )
{
	// Set mesh
	m_visible = true;
	m_mesh = instance;

	// Allow the object to be rendered
	m_managed = managed;
	if( managed ) DirectX::Manager::instance( )->addRenderable( this );
}
																																					//
// Destruction
void DirectX::Object::release( )
{
	// Allow the object to be rendered
	if( m_mesh != NULL )
	{
		m_mesh = NULL;
		if( m_managed ) DirectX::Manager::instance( )->deleteRenderable( this );
	}
}
																																					//
// Renders the object
void DirectX::Object::render( )
{
	// Check if the object is hidden
	if( m_visible )
	{
		// Set the transform
		DirectX::Manager::instance( )->getD3DDev( )->SetTransform( D3DTS_WORLD, &m_transform );

		// Render the object
		m_mesh->render( );
	}
}
																																					//
// Updates the object transform
void DirectX::Object::update( ) 
{ 
	// Update object transform
	D3DXMatrixIdentity( &m_transform ); D3DXMATRIX buffer;
	D3DXMatrixRotationX( &buffer, m_rotation.x ); m_transform *= buffer;
	D3DXMatrixRotationY( &buffer, m_rotation.y ); m_transform *= buffer;
	D3DXMatrixRotationZ( &buffer, m_rotation.z ); m_transform *= buffer;
	D3DXMatrixScaling( &buffer, m_objScale.x, m_objScale.y, m_objScale.z ); m_transform *= buffer;
	D3DXMatrixTranslation( &buffer, m_position.x, m_position.y, m_position.z ); m_transform *= buffer;

	// Compute OBB
	D3DXVECTOR3 boxCoords[8];
	m_boxMin = *(m_mesh->getBoxMin( ));
	m_boxMax = *(m_mesh->getBoxMax( ));

	// We have min and max values, use these to get the 8 corners of the bounding box
	boxCoords[0] = D3DXVECTOR3( m_boxMin.x, m_boxMin.y, m_boxMin.z ); // xyz
	boxCoords[1] = D3DXVECTOR3( m_boxMax.x, m_boxMin.y, m_boxMin.z ); // Xyz
	boxCoords[2] = D3DXVECTOR3( m_boxMin.x, m_boxMax.y, m_boxMin.z ); // xYz
	boxCoords[3] = D3DXVECTOR3( m_boxMax.x, m_boxMax.y, m_boxMin.z ); // XYz
	boxCoords[4] = D3DXVECTOR3( m_boxMin.x, m_boxMin.y, m_boxMax.z ); // xyZ
	boxCoords[5] = D3DXVECTOR3( m_boxMax.x, m_boxMin.y, m_boxMax.z ); // XyZ
	boxCoords[6] = D3DXVECTOR3( m_boxMin.x, m_boxMax.y, m_boxMax.z ); // xYZ
	boxCoords[7] = D3DXVECTOR3( m_boxMax.x, m_boxMax.y, m_boxMax.z ); // XYZ

	// Transform the corners of the bounding box
	for( int i=0; i<8; i++ )
      D3DXVec3TransformCoord( &boxCoords[i], &boxCoords[i], &m_transform );

	// Compute the AABB from the OBB
	m_boxMin.x = m_boxMax.x = boxCoords[0].x;
	m_boxMin.y = m_boxMax.y = boxCoords[0].y;
	m_boxMin.z = m_boxMax.z = boxCoords[0].z;

	for( int i=1; i<8; i++ )
	{
		   if( boxCoords[i].x < m_boxMin.x ) m_boxMin.x = boxCoords[i].x;
		   if( boxCoords[i].x > m_boxMax.x ) m_boxMax.x = boxCoords[i].x;
		   if( boxCoords[i].y < m_boxMin.y ) m_boxMin.y = boxCoords[i].y;
		   if( boxCoords[i].y > m_boxMax.y ) m_boxMax.y = boxCoords[i].y;
		   if( boxCoords[i].z < m_boxMin.z ) m_boxMin.z = boxCoords[i].z;
		   if( boxCoords[i].z > m_boxMax.z ) m_boxMax.z = boxCoords[i].z;
	}

	// Compute the scaled sphere radius and transformed origin
	float scale = m_objScale.x; 
	if( scale < m_objScale.y ) scale = m_objScale.y;
	if( scale < m_objScale.z ) scale = m_objScale.z;
	m_sphereRadius = m_mesh->getSphereRadius( ) * scale;
	D3DXVec3Add( &m_sphereOrigin, m_mesh->getSphereOrigin( ), &m_position );
}
																																					//
// Performs a collision test with the mesh against the input ray
void DirectX::Object::intersectRay( DirectX::Vector3* rayOrigin, DirectX::Vector3* rayDir, int* hit, unsigned long* face, float* bary1, float* bary2, float* dist )
{
	// Filter out objects with a bounds collision check
	if( /*D3DXSphereBoundProbe( &m_sphereOrigin, m_sphereRadius, rayOrigin, rayDir ) 
		&&*/ D3DXBoxBoundProbe( &m_boxMin, &m_boxMax, rayOrigin, rayDir ) ) 
	{
			// Compute the inverse matrix
			D3DXMATRIX matInverse;
			D3DXMatrixInverse( &matInverse, NULL, &m_transform );

			// Transform ray origin and direction by inv matrix
			D3DXVECTOR3 rayObjOrigin, rayObjDirection;
			D3DXVec3TransformCoord( &rayObjOrigin, rayOrigin, &matInverse);
			D3DXVec3TransformNormal( &rayObjDirection, rayDir, &matInverse );
			D3DXVec3Normalize( &rayObjDirection, &rayObjDirection );

			// Perform D3DX intersection test with the mesh
			D3DXIntersect( m_mesh->getMesh( ), &rayObjOrigin, &rayObjDirection, hit, face, bary1, bary2, dist, NULL, NULL );
	} 
	else *hit = 0;
}