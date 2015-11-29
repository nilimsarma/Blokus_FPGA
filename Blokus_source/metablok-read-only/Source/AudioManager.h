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

// Begin definition
#ifndef DX_ENGINE_AUDIO_MANAGER_H
#define DX_ENGINE_AUDIO_MANAGER_H

// XAudio API
#include <xaudio2.h>

// Engine namespace
namespace DirectX
{

// Management class
class AudioManager
{
public:
	// Destructor
	~AudioManager( ) { shutdown( ); }

	// Startup / Shutdown processes
	int startup( ); void shutdown( );

	// Singleton accessor	
	static AudioManager* instance( )
	{
		static AudioManager gSingleton;
		return &gSingleton;
	}

private:
	AudioManager( ) { }

	IXAudio2* m_xAudio2;
	IXAudio2MasteringVoice* m_masterVoice;
};

}

// End file definition
#endif