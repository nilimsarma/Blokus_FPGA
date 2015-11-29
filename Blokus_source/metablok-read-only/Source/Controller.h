/* ===========================================================================

	Project: DirectX Engine - Image

	Description:
	 Defines an abstract base class which recieves signals for certain
	 commands from the engine manager.

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
#ifndef DX_ENGINE_CONTROLLER_H
#define DX_ENGINE_CONTROLLER_H

// Include Engine Header
#include "DirectX.h"

// DirectX Engine Namespace
namespace DirectX
{
	// Control class prototype
	namespace GUI { class Control; }

	// Controller Object
	class Controller
	{
	public:
		// Virtualize destructor
		virtual ~Controller( ) { }

		// Device Settings Check
		virtual bool onDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat ) { return true; }
		
		// Windows message events
		virtual void onMessageEvent( UINT message, WPARAM wParam, LPARAM lParam ) { }
		virtual void onKeyboardEvent( UINT message, WPARAM wParam, LPARAM lParam ) { }
		virtual void onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam ) { }
		virtual void onGuiEvent( DirectX::GUI::Control* control, unsigned int message, void* data ) { }

		// Device State Changes
		virtual void onDeviceLost( ) { }
		virtual void onDeviceReset( ) { }
		virtual void onDeviceCreated( ) { }
		virtual void onDeviceDestroyed( ) { }
		virtual void onDevicePaused( ) { }
		virtual void onDeviceUnpaused( ) { }

		// Frame rendering
		virtual void onRenderFrame( Camera* camera ) { }
	};

}

// End defintion
#endif
