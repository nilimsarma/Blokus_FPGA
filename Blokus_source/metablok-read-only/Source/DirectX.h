/* ===========================================================================

	Project: DirectX Game Engine

	Description:
	 A 3D game engine utilizing Microsoft's DirectX library for the graphics
	 rendering sublayer. 

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
#ifndef DX_ENGINE
#define DX_ENGINE

// Min/Max Macros
#ifndef NOMINMAX
#define NOMINMAX
#endif//NOMINMAX

// Unicode build
#ifndef UNICODE
#define UNICODE
#endif//UNICODE

// Limited Windows Headers
#define WIN32_LEAN_AND_MEAN	

// DirectX Library Files
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// XInput library
#pragma comment(lib, "xinput.lib")

// Set required windows version
#include "TargetVersion.h"
	
// Windows Header Files
#include <windows.h>
#include <process.h>

// DirectX Headers
#include <d3d9.h>
#include <d3dx9.h>

// XInput Headers
#include <XInput.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ Standard Library
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <limits>
#include <stack>

// Debug build options
#ifdef _DEBUG

// Enhanced Memory Leak Detection
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW 

// End debug options
#endif

// Networking Sublayer
#include "NetworkManager.h"

// Input Sublayer
#include "InputManager.h"

// Window Resource File
#include "Resources.h"

// Typedef Definitions
#include "Definitions.h"

// US Keyboard Scancodes
#include "Scancodes (US).h"

// High Resolution Timer
#include "Timer.h"

// Graphics Sublayer
#include "Camera.h"
#include "Resource.h"
#include "Renderable.h"
#include "Colors.h"
#include "Image.h"
#include "Effect.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Mesh.h"
#include "Object.h"
#include "Font.h"

// GUI Sublayer
#include "WindowStyle.h" // :TODO: No WindowStyle or color dependancy it
#include "Control.h"	 //		is technically an implemented GUI feature
#include "Panel.h"		 //     ( ie non-essential to user implementation )
#include "DisplayPanel.h"

// Default GUI Controls
#ifndef DX_NOGUI
#include "Messages.h"
#include "NumericIndicator.h"
#include "NumericControl.h"
#include "Button.h"
#include "Checkbox.h"
#include "ComboBox.h"
#include "ListBox.h"
#include "EditBox.h"
#include "Window.h"
#include "Label.h"
#include "Graphic.h"
#endif//DX_NOGUI

// Graphics Sublayer
#include "GraphicsManager.h"

// Game Engine Manager
#include "EngineManager.h"

// End definition
#endif