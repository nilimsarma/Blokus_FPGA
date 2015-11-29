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

// Standard includes
#include "DirectX.h"

// Include header
#include "Effect.h"

// --------------------------------------------------------
//	Zeros the effect data pointer so existance checks return
//  false properly.
// --------------------------------------------------------
DirectX::Effect::Effect( )
{ 
	m_effect = NULL;
}
																																					//
// --------------------------------------------------------
//	Creates an effect from a file and passes the object to
//  the manager for handling device state changes
// --------------------------------------------------------
void DirectX::Effect::create( WCHAR* filename )
{
	if( FAILED( D3DXCreateEffectFromFile( DirectX::Manager::instance( )->getD3DDev( ), filename, 0, 0, 
		D3DXFX_NOT_CLONEABLE|D3DXSHADER_NO_PRESHADER, 0, &m_effect, 0 ) )) 
		{ MessageBox( NULL, L"Load effect failed!", L"DirectX Engine", NULL ); exit(1);}

	DirectX::Manager::instance( )->addResource( this );
}
																																					//
// --------------------------------------------------------
//	Calls the effect object's device lost function
// --------------------------------------------------------
void DirectX::Effect::onDeviceLost( )
{
	m_effect->OnLostDevice( );
}
																																					//
// --------------------------------------------------------
//	Calls the effect object's device reset function
// --------------------------------------------------------
void DirectX::Effect::onDeviceReset( )
{
	m_effect->OnResetDevice( );
}
																																					//
// --------------------------------------------------------
//	Releases the direct3D effect object if it exists and 
//  removes the object from the manager
// --------------------------------------------------------
void DirectX::Effect::release( )
{
	if( m_effect != NULL )
	{
		m_effect->Release( );
		m_effect = NULL;

		DirectX::Manager::instance( )->deleteResource( this );
	}
}