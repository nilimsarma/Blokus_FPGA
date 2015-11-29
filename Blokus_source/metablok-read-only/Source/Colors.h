/* ===========================================================================

	Project: DirectX Engine - Colors

	Description:
	 Defines some common standard color values.

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
#ifndef DX_ENGINE_COLORS
#define DX_ENGINE_COLORS

// Include dx declarations
#include "DirectX.h"

namespace COLOR 
{
	const D3DXCOLOR RED   = D3DXCOLOR( 255, 0, 0, 255 );
	const D3DXCOLOR GREEN = D3DXCOLOR( 0, 255, 0, 255 );
	const D3DXCOLOR BLUE  = D3DXCOLOR( 0, 0, 255, 255 );

	const D3DXCOLOR BLACK = D3DXCOLOR( 0, 0, 0, 255 );
	const D3DXCOLOR DARK_GRAY = D3DXCOLOR( 64, 64, 64, 255 );
	const D3DXCOLOR GRAY  = D3DXCOLOR( 128, 128, 128, 255 );
	const D3DXCOLOR LIGHT_GRAY = D3DXCOLOR( 192, 192, 192, 255 );
	const D3DXCOLOR WHITE = D3DXCOLOR( 255, 255, 255, 255 );
}

// End definition
#endif