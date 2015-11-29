/* ===========================================================================

	Project: Game Engine - Audio Manager

	Description: Implements game engines audio capabilities using the Windows
	 XAudio2 API.

	http://msdn.microsoft.com/en-us/library/windows/desktop/hh405049%28v=VS.85%29.aspx

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

// Include Header
//#include "AudioManager.h"

// Engine Header
#include "DirectX.h"
/*
// --------------------------------------------------------
//	Initializes the XAudio2 API and creates the global
//  mastering voice object.
// --------------------------------------------------------
int DirectX::AudioManager::startup( )
{
	HRESULT hr;

	// Attempt to create xAudio interface object
	if ( FAILED(hr = XAudio2Create( &m_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR )) )
	{
		std::wstring estr( L"Failed to init XAudio2 engine: " ); estr = estr + hr;
		MessageBox( d3dhwnd, estr.c_str( ), L"DirectX Engine", NULL );
		return 1;
	}

	// Attempt to create mastering voice for sounds
	if ( FAILED(hr = pXAudio2->CreateMasteringVoice( &m_masterVoice )) )
	{
		std::wstring estr( L"Failed creating mastering voice: " ); estr = estr + hr;
		MessageBox( d3dhwnd, estr.c_str( ), L"DirectX Engine", NULL );
		return 1;
	}
}
*/