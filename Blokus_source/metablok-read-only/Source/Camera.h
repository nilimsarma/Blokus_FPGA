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

// Begin definition
#ifndef DX_ENGINE_CAMERA
#define DX_ENGINE_CAMERA

// DirectX namespace
namespace DirectX
{

// Forward declarations
class Object;

// Encapsulation class
class Camera
{

public:
	// Constructors and Destructor
	Camera( ); ~Camera( );

	// Accessors
    D3DXMATRIX* getViewMatrix( )  { return &m_matView; }
    D3DXMATRIX* getProjMatrix( )  { return &m_matProj; }
    D3DXVECTOR3*  getPosition( )  { return &m_position; }
	D3DVIEWPORT9* getViewport( )  { return &m_viewport; }

	// Absolute Positioning
	void setPosition( float x, float y, float z );
	void setPosition( D3DXVECTOR3 &p ) { m_position.x = p.x; m_position.y = p.y; m_position.z = p.z; }
	void translate( float x, float y, float z );
	void moveX( float dist ) { m_position.x += dist; }
	void moveY( float dist ) { m_position.y += dist; }
	void moveZ( float dist ) { m_position.z += dist; }

	// Relative Positioning
	void moveRight( float dist );
	void moveUp( float dist );
	void move( float dist );

	// Relative Orientation
	void yaw( float radians );
	void pitch( float radians );
	void roll( float radians );

	// Absolute Orientation
	void point( D3DXVECTOR3* location );
	void setAngle( float x, float y, float z );
	void rotate( float x, float y, float z );
	void rotateX( float radians );
	void rotateY( float radians );
	void rotateZ( float radians );
	void setAngleX( float radians ) { m_rotation.x = radians; }
	void setAngleY( float radians ) { m_rotation.y = radians; }
	void setAngleZ( float radians ) { m_rotation.z = radians; }
	float getAngleX( ) { return m_rotation.x; }
	float getAngleY( ) { return m_rotation.y; }
	float getAngleZ( ) { return m_rotation.z; }
 
	// Projection Accessors/Modifiers
	void setAspectRatio( float ratio )			 { m_aspect = ratio; }
	void setClippingDist( float min, float max ) { m_zmin = min; m_zmax = max; }
	void SetFOV( float fov )					 { m_FOV = fov; }

    float getFOV( )         { return m_FOV; }
    float getAspectRatio( ) { return m_aspect; }
    float getMinDist( )		{ return m_zmin; }
    float getMaxDist( )		{ return m_zmax; }

	// Camera viewport
	void setViewPort( int width, int height, int x=0, int y=0 );

	// Updating internal transform matrices
	void updateViewMatrix( );
	void updateProjMatrix( ) { D3DXMatrixPerspectiveFovLH( &m_matProj, m_FOV, m_aspect, m_zmin, m_zmax ); }
	void updateFrustum( );
	void update( ) { updateViewMatrix( ); updateProjMatrix( ); updateFrustum( ); }

	// Frustrum Culling
	bool objectInFrustum( DirectX::Object* object );
	bool sphereInSphere( D3DXVECTOR3* origin, float radius ); 
	bool sphereInCone( D3DXVECTOR3* origin, float radius );
	bool sphereInFrustum( D3DXVECTOR3* origin, float radius );
	int sphereInFrustumEx( D3DXVECTOR3* origin, float radius );
	bool boxInFrustum( D3DXVECTOR3* p1, D3DXVECTOR3* p2 );
	int boxInFrustumEx( D3DXVECTOR3* p1, D3DXVECTOR3* p2 );
	bool pointInFrustum( D3DXVECTOR3* pt );

	// Raycasting
	void getRay( D3DXVECTOR2 *screenPos, D3DXVECTOR3 *origin, D3DXVECTOR3 *dir );

private:
	// Transformation mats
	D3DVIEWPORT9 m_viewport;
	D3DXMATRIX m_matProj;
	D3DXMATRIX m_matView;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_rotation;

	// Frustum planes
	D3DXPLANE m_frustum[6];

	// Frustum cone
	D3DXVECTOR3 m_coneDir;
	D3DXVECTOR3 m_conePos;

	//orientation vects
    D3DXVECTOR3 m_right;
    D3DXVECTOR3 m_up;
    D3DXVECTOR3 m_look;
    D3DXVECTOR3 m_lookAtVect;

	// Projection parameters
	float m_FOV, m_aspect, m_zmin, m_zmax;
};

}

// End definition
#endif