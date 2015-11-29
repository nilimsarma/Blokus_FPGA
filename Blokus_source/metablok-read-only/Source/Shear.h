/* ===========================================================================

	Project: Game Engine

	Description: Provides a basic interface for application components to
	 interface with other sub-engines.

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

// Include gaurds
#ifndef SHEAR_ENGINE_H
#define SHEAR_ENGINE_H

// DirectX engine
#include "DirectX.h"

// Debug build options
#ifdef _DEBUG

// Enhanced Memory Leak Detection
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW 

// End debug options
#endif


// End definition
#endif