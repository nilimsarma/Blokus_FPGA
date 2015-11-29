/* ===========================================================================

	Project: DirectX Engine - Resource

	Description:
	 Provides an abstract base class for DirectX resources which might need
	 to be released and reintroduced to video memory during device resets.

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
#ifndef DX_ENGINE_RESOURCE
#define DX_ENGINE_RESOURCE

// Include Engine Header
#include "DirectX.h"

// DirectX Engine Namespace
namespace DirectX
{
	// Resource Object
	class Resource
	{
	public:
		// Virtualize destructor
		virtual ~Resource( ) { };

		// Device State Changes
		virtual void onDeviceLost( ) = 0;
		virtual void onDeviceReset( ) = 0;
		virtual void release( ) = 0;
	};
}

// End defintion
#endif
