/* ===========================================================================

	Project: DirectX Engine - Sprite

	Description:
	 Provides an implementation of a simple sprite control. Does not interact
	 with user input but provides management for animated GUI components in
	 the default GUI framework.

    Copyright (C) 2011 Lucas Sherman

	Lucas Sherman, email: LucasASherman@gmail.com, phone: 763-786-6916

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
#ifndef DX_ENGINE_SPRITE
#define DX_ENGINE_SPRITE

// Forward declaration
namespace DirectX {
namespace GUI {

	// Simple sprite controls
	class Sprite : public Control
	{

	public:
		// Constructor / Destructor 
		Sprite( ); ~Sprite( );

		// Construct on creation
		Sprite( Image* image, bool managed = true );

		// Creation
		void create( Image* image, RECT* rect = NULL, bool managed = true );

		// Depth changes
		void setDepth( int depth );

		// Release method
		void release( );

		// Sprite Scaling
		void scale( float x, float y ) { m_scaling.x *= x; m_scaling.y *= y; }
		void scale( D3DXVECTOR2* scale ) { m_scaling.x *= scale->x; m_scaling.y *= scale->y; }
		void setScale( float x, float y ) { m_scaling.x = x; m_scaling.y = y; }
		void setScaleX( float scale ) { m_scaling.x = scale; }
		void setScaleY( float scale ) { m_scaling.y = scale; }
		void setScale( D3DXVECTOR2* scale ) { m_scaling = *scale; }

		// Rectangle
		const RECT* getRectangle( ) { return &m_rect; }
		void setRect( RECT* rect ) { m_rect = *rect; }

		// Update transformation matrix
		void update( ) { D3DXMatrixTransformation2D( &m_transform, NULL, 0.0, &m_scaling, &m_rotationCenter, m_rotation, &Vector2((float)m_posX,(float)m_posY) ); }
		D3DXMATRIX* getTransform( ) { return &m_transform; } 

		// Image pointer
		Image* getImage( ) { return m_sprite; }
		void setImage( Image* image ) { m_sprite = image; }

		// Visibility
		void hide( ) { m_hidden = true; }
		void show( ) { m_hidden = false; }
		bool isHidden( ) { return m_hidden; }

		// Renders the sprite
		void render( LPD3DXSPRITE d3dsprite );

	protected:
		// Transform values
		D3DXMATRIX m_transform;
		D3DXVECTOR2 m_rotationCenter;
		D3DXVECTOR2 m_scalingCenter;
		D3DXVECTOR2 m_scaling;
		float m_rotation;

		Image *m_sprite;
		RECT m_rect;
	};

} }

// End definition
#endif
