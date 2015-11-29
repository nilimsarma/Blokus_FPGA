/* ===========================================================================

	Project: DirectX Engine - Effect

	Description:
	 Encapsulates a D3DXEFFECT object and manages resource allocation and
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
#ifndef DX_ENGINE_EFFECT
#define DX_ENGINE_EFFECT

// DirectX namespace
namespace DirectX
{

	// Effect encapsulating class
	class Effect : public Resource
	{

	public:
		// Constructor/Destructor
		Effect( ); ~Effect( ) { release( ); }

		// Creation
		void create( WCHAR* filename );

		// Direct3D object
		LPD3DXEFFECT get( ) { return m_effect; }

		// Device state operations
		void onDeviceLost( );
		void onDeviceReset( );
		void release( );

	private:
		LPD3DXEFFECT m_effect;

	};

}

// End definition
#endif