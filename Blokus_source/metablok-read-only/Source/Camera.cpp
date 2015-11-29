/* ===========================================================================

	Project: DirectX Engine - Camera

	Description:
	 Specifies a camera which can be registered with the internal DirectX
	 manager class for automatic rendering.

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

// Include header
#include "Camera.h"

// Constructor
DirectX::Camera::Camera( )
{
	// Set camera viewport
	m_viewport.X = m_viewport.Y = 0;
	m_viewport.Height = 600;
	m_viewport.Width  = 800;
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 1.0f;

	// Set the projection params
	m_FOV = D3DXToRadian(60);
	m_aspect = (FLOAT)4/(FLOAT)3;
	m_zmin = 1.0f; m_zmax = 3000.0f;

	// Set initial camera coordinates
	m_position = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_rotation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_look = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	m_up = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	m_right = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );

	// Create transformation matrices
	updateViewMatrix( ); updateProjMatrix( );

	// Build frustrum planes
	updateFrustum( );
}
																																					//
// Destructor
DirectX::Camera::~Camera( )
{
	DirectX::Manager::instance( )->deleteCamera( this ); 
}
																																					//
// Set the camera viewport
void DirectX::Camera::setViewPort( int width, int height, int x, int y )
{
	m_viewport.X = x; m_viewport.Y = y;
	m_viewport.Height = height;
	m_viewport.Width  = width;
}
																																					//
// Strafe the camera right
void DirectX::Camera::moveRight( float dist ) 
{ m_position+=m_right*dist; }
																																					//
// Strafe the camera up
void DirectX::Camera::moveUp( float dist ) 
{ m_position+=m_up*dist; }
																																					//
// Moves the camera along its own axis
void DirectX::Camera::move( float dist )
{
	// Move camera along look vector
	m_position+=m_look*dist;
}
																																					//
// Translates the camera
void DirectX::Camera::translate( float x, float y, float z )
{
	m_position.x+=x;
	m_position.y+=y;
	m_position.z+=z;
}
																																					//
// Sets the camera position
void DirectX::Camera::setPosition( float x, float y, float z )
{ m_position.x = x; m_position.y = y; m_position.z = z; }
																																					//
// Updates the transform matrix
void DirectX::Camera::updateViewMatrix( ) 
{
	// Declare transform matrix
	D3DXMATRIX transform;

	// Reset camera orientation
	m_look = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	m_up = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	m_right = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );

	// Z-Rotation
	D3DXMatrixRotationAxis( &transform, &m_look, m_rotation.z );
	D3DXVec3TransformNormal( &m_right, &m_right, &transform ); 
	D3DXVec3TransformNormal( &m_up, &m_up, &transform );

	// Y-Rotation
	D3DXMatrixRotationAxis( &transform, &m_up, m_rotation.y );
	D3DXVec3TransformNormal( &m_right, &m_right, &transform );
	D3DXVec3TransformNormal( &m_look, &m_look, &transform ); 

	// X-Rotation
	D3DXMatrixRotationAxis( &transform, &m_right, m_rotation.x );
	D3DXVec3TransformNormal( &m_up, &m_up, &transform ); 
	D3DXVec3TransformNormal( &m_look, &m_look, &transform ); 

	// Create view matrix
	m_matView._11 = m_right.x; m_matView._12 = m_up.x; m_matView._13 = m_look.x;
	m_matView._21 = m_right.y; m_matView._22 = m_up.y; m_matView._23 = m_look.y;
	m_matView._31 = m_right.z; m_matView._32 = m_up.z; m_matView._33 = m_look.z;

	m_matView._41 = - D3DXVec3Dot( &m_position, &m_right );
	m_matView._42 = - D3DXVec3Dot( &m_position, &m_up    );
	m_matView._43 = - D3DXVec3Dot( &m_position, &m_look  );

	m_matView._14 = 0; m_matView._24 = 0;
	m_matView._34 = 0; m_matView._44 = 1;
}
																																					//
// Points the camera toward a point
void DirectX::Camera::point( D3DXVECTOR3* location )
{
	// Calculate look vector
	D3DXVec3Normalize( &m_look, &(*location - m_position) ); 

	// Calculate x and y angles
	float lookLengthOnXZ = sqrtf( m_look.z*m_look.z+m_look.x*m_look.x );
	m_rotation.x = atan2f( -m_look.y, lookLengthOnXZ );
	m_rotation.y = atan2f( m_look.x, m_look.z );
}
																																					//
// Yaws the camera
void DirectX::Camera::yaw( float radians )
{
	D3DXMATRIX trotation;
	D3DXMatrixRotationAxis( &trotation, &m_up, radians );
	D3DXVec3TransformNormal( &m_right, &m_right, &trotation );
	D3DXVec3TransformNormal( &m_look, &m_look, &trotation );

	// Calculate yaw and pitch
	float lookLengthOnXZ = sqrtf( m_look.z*m_look.z+m_look.x*m_look.x );
	m_rotation.x = atan2f( m_look.y, lookLengthOnXZ );
	m_rotation.y = atan2f( m_look.x, m_look.z );
}
																																					//
// Pitches the camera
void DirectX::Camera::pitch( float radians )
{
	D3DXMATRIX trotation;
	D3DXMatrixRotationAxis( &trotation, &m_right, radians );
	D3DXVec3TransformNormal( &m_up, &m_up, &trotation ); 
	D3DXVec3TransformNormal( &m_look, &m_look, &trotation ); 

	// Calculate yaw and pitch
	float lookLengthOnXZ = sqrtf( m_look.z*m_look.z+m_look.x*m_look.x );
	m_rotation.x = atan2f( m_look.y, lookLengthOnXZ );
	m_rotation.y = atan2f( m_look.x, m_look.z );
}
																																					//
// rolls the camera
void DirectX::Camera::roll( float radians )
{
	m_rotation.z += radians;
	m_rotation.z += ( m_rotation.z<0 )? 6.28318f : ( m_rotation.z > 6.28318f )? -6.28318f : 0;
}
																																					//
// Yaws the camera
void DirectX::Camera::rotateX( float radians )
{
	m_rotation.x += radians;
	m_rotation.x += ( m_rotation.x<0 )? 6.28318f : ( m_rotation.x > 6.28318f )? -6.28318f : 0;
}
																																					//
// Pitches the camera
void DirectX::Camera::rotateY( float radians )
{
	m_rotation.y += radians;
	m_rotation.y += ( m_rotation.y<0 )? 6.28318f : ( m_rotation.y > 6.28318f )? -6.28318f : 0;
}
																																					//
// rolls the camera
void DirectX::Camera::rotateZ( float radians )
{
	m_rotation.z += radians;
	m_rotation.z += ( m_rotation.x<0 )? 6.28318f : ( m_rotation.x > 6.28318f )? -6.28318f : 0;
}
																																					//
// Gets the mouse pick ray
void DirectX::Camera::getRay( D3DXVECTOR2 *screenPos, D3DXVECTOR3 *origin, D3DXVECTOR3 *dir )
{
	// Get handle of DirectX manager
	DirectX::Manager *dxm = DirectX::Manager::instance( );
	float mx, my;

	// Store screen position if given, otherwise load mouse coords
	if( screenPos ) { mx = screenPos->x; my = screenPos->y; }  
	else { mx = (float)dxm->getMouseX( ); my = (float)dxm->getMouseY( ); }

	// :TODO: Translate coordinates to camera screen space

    // Compute camera space pick vector
    D3DXVECTOR3 v(  ( ( ( 2.0f * mx ) / (float)m_viewport.Width  ) - 1.0f ) / m_matProj._11,
				   -( ( ( 2.0f * my ) / (float)m_viewport.Height ) - 1.0f ) / m_matProj._22, 1.0f );

    // Get the inverse view matrix for 2D to 3D projection
    D3DXMATRIX m; D3DXMatrixInverse( &m, NULL, &m_matView );

    // Transform the screen space Pick ray into 3D space
    origin->x = m._41; origin->y = m._42; origin->z = m._43;
    dir->x = v.x * m._11 + v.y * m._21 + v.z * m._31;
    dir->y = v.x * m._12 + v.y * m._22 + v.z * m._32;
    dir->z = v.x * m._13 + v.y * m._23 + v.z * m._33;
}
																																					//
// -------------------------------------------------------
// Updates the frustrum planes based on the view and
// projection matrices
// -------------------------------------------------------
void DirectX::Camera::updateFrustum( )
{
    D3DXMATRIX viewProjection;
    D3DXMatrixMultiply( &viewProjection, &m_matView, &m_matProj );

    // Left plane
    m_frustum[0].a = viewProjection._14 + viewProjection._11;
    m_frustum[0].b = viewProjection._24 + viewProjection._21;
    m_frustum[0].c = viewProjection._34 + viewProjection._31;
    m_frustum[0].d = viewProjection._44 + viewProjection._41;
 
    // Right plane
    m_frustum[1].a = viewProjection._14 - viewProjection._11;
    m_frustum[1].b = viewProjection._24 - viewProjection._21;
    m_frustum[1].c = viewProjection._34 - viewProjection._31;
    m_frustum[1].d = viewProjection._44 - viewProjection._41;
 
    // Top plane
    m_frustum[2].a = viewProjection._14 - viewProjection._12;
    m_frustum[2].b = viewProjection._24 - viewProjection._22;
    m_frustum[2].c = viewProjection._34 - viewProjection._32;
    m_frustum[2].d = viewProjection._44 - viewProjection._42;
 
    // Bottom plane
    m_frustum[3].a = viewProjection._14 + viewProjection._12;
    m_frustum[3].b = viewProjection._24 + viewProjection._22;
    m_frustum[3].c = viewProjection._34 + viewProjection._32;
    m_frustum[3].d = viewProjection._44 + viewProjection._42;
 
    // Near plane
    m_frustum[4].a = viewProjection._13;
    m_frustum[4].b = viewProjection._23;
    m_frustum[4].c = viewProjection._33;
    m_frustum[4].d = viewProjection._43;
 
    // Far plane
    m_frustum[5].a = viewProjection._14 - viewProjection._13;
    m_frustum[5].b = viewProjection._24 - viewProjection._23;
    m_frustum[5].c = viewProjection._34 - viewProjection._33;
    m_frustum[5].d = viewProjection._44 - viewProjection._43;
 
    // Normalize planes
    for ( int i = 0; i < 6; i++ )
        D3DXPlaneNormalize( &m_frustum[i], &m_frustum[i] );
}
																																					//
// -------------------------------------------------------
// Returns 0 if the sphere is outside the view frustrum.
// In the case where the sphere is within the view frustrum
// a return value of 1 specifies the entire sphere is 
// contained and 2 specifies an intersection with the planes.
// -------------------------------------------------------
int DirectX::Camera::sphereInFrustumEx( D3DXVECTOR3* origin, float radius )
{
	bool inside = (D3DXPlaneDotCoord( m_frustum, origin ) + radius < 0);
    for ( int i = 1; i < 6; i++ )
		if( inside != (D3DXPlaneDotCoord( m_frustum, origin ) + radius < 0) ) return 2;

	if( inside ) return 1; 
	else return 0;
}
																																					//
// -------------------------------------------------------
// Returns true if the sphere is within the view frustrum
// -------------------------------------------------------
bool DirectX::Camera::sphereInFrustum( D3DXVECTOR3* origin, float radius )
{
    for ( int i = 0; i < 6; i++ )
		if( D3DXPlaneDotCoord( &m_frustum[i], origin ) + radius < 0 )
			return false;

	return true;
}
																																					//
// -------------------------------------------------------
// Returns 0 if the sphere intersects the view frustrum.
// If entirely outside or inside of the frustrum, returns
// -1 or 1 respectively.
// -------------------------------------------------------
bool DirectX::Camera::pointInFrustum( D3DXVECTOR3* pt )
{
    for ( int i = 0; i < 6; i++ )
		if( D3DXPlaneDotCoord( &m_frustum[i], pt ) < 0 )
			return false;

	return true;
}
																																					//
// -------------------------------------------------------
// Returns true if the sphere is within the clipping dist
// of the camera
// -------------------------------------------------------
bool DirectX::Camera::sphereInSphere( D3DXVECTOR3* origin, float radius )
{
	return false;
}
