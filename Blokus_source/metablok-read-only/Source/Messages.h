/* ===========================================================================

	Project: DirectX Engine - Messages

	Description:
	 Defines message types for the default engine GUI classes. If DXNOGUI is
	 defined, this header will not be loaded by the DirectX engine.

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

// Standard Message Types
namespace DirectX {
namespace GUI {

	enum Message 
	{
		// Any control
		CM_DESTROYED,

		// Button controls
		CM_BUTTON_PRESSED,

		// Checkbox controls
		CM_CHECKBOX_CHECKED,
		CM_CHECKBOX_UNCHECKED,

		// Window panels
		CM_MINIMIZE_WINDOW,
		CM_MAXIMIZE_WINDOW,
		CM_WINDOW_CLOSED,

		// Combo box controls
		CM_COMBO_BOX_SELECTION,

		// List box controls
		CM_LIST_BOX_SELECTION,
		CM_LIST_BOX_DBLCLICK,

		// Numeric Control controls
		CM_NUMERIC_CHANGED
	};

} }