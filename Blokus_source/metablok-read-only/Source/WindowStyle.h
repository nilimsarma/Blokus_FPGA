/* ===========================================================================

	Project: DirectX Engine - WindowStyle

	Description:
	 Provides a consolidated set of window style options.

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
#ifndef DX_GUI_WINDOW_STYLES
#define DX_GUI_WINDOW_STYLES

// Include header
#include "DirectX.h"

// Engine namespace
namespace DirectX {
namespace GUI {

// Window style structure
struct WindowStyle
{
	// Window style sprite sheet
	DirectX::Image spriteSheet;

	// Window style color mask
	unsigned long color;
	bool maskWithColor;

	// Window font styles
	unsigned long titleFontColor;
	unsigned long fontColor;
	DirectX::Font titleFont;
	DirectX::Font font;

	// Arrows
	RECT downArrow;
	RECT upArrow;
	RECT downArrowHover;
	RECT upArrowHover;
	RECT downArrowPressed;
	RECT upArrowPressed;

	// Scroll bars
	RECT scrollbar;
	RECT scroller;

	// Backdrops and highlights
	RECT selectionBox;
	RECT backdrop;

	// Windows
	RECT windowBorderLeft;
	RECT windowBorderRight;
	RECT windowBorderBottom;
	RECT windowBorderTop;
	RECT windowBorderTopMiddle;
	RECT windowBorderTopLeft;
	RECT windowBorderBottomLeft;
	RECT windowBorderBottomRight;
	RECT windowBorderTopRight;
	RECT windowBorderTopRightMin;
	RECT windowBorderTopRightClose;
	RECT windowBorderMiddle;
	RECT windowMiddleTransparent;
	RECT windowMiddleOpaque;
	RECT windowBorderTopLeftMin;
	RECT windowBorderTopRightMinMin;
	RECT windowBorderTopRightMinClose;
	RECT windowBorderTopRightMinNormal;

	// Toolbars
	RECT menuBar;

	// Buttons
	RECT button;
	RECT buttonHover;
	RECT buttonDown;
	RECT buttonInfo;
	bool buttonScaleable;

	// Radio Buttons
	RECT radioButtonOff;
	RECT radioButtonOn;

	// Checkboxes
	RECT checkboxOff;
	RECT checkboxOn;
};

}}

// End definition
#endif