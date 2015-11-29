/* ===========================================================================

	Project: DirectX Engine - Object

	Description:
	 Defines an abstract renderable object which implements a render method 
	 for callback by the engine manager on frame rendering.
    
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
#ifndef DX_ENGINE_RENDERABLE
#define DX_ENGINE_RENDERABLE

// Include Engine Header
#include "DirectX.h"

// DirectX Engine Namespace
namespace DirectX
{
	// Controller Object
	class Renderable
	{
	public:
		// Virtualize destructor
		virtual ~Renderable( ) { };

		// Render method
		virtual void render( ) = 0;
	};
}

// End defintion
#endif
