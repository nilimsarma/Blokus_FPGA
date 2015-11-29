/* ===========================================================================

	Project: DirectX Engine GUI - Control

	Description:
	 Defines the abstract base class for GUI controls managed by the 
	 internal manager.

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

// Standard includes
#include "DirectX.h"

// Include header
#include "Control.h"

// --------------------------------------------------------
//	Decouples the control from its panel.
// --------------------------------------------------------
void DirectX::GUI::Control::release( )
{
	if( m_panel ) 
	{
		m_panel->deleteControl( this ); 
		m_panel = NULL;
	}
}
//
// --------------------------------------------------------
//	Returns true if the control contains the specified 
//  point within its interaction region.
// --------------------------------------------------------
bool DirectX::GUI::Control::contains( const POINT *pt )
{ 
	return !(m_state&HIDDEN) &&
			pt->x <= m_posX + m_sizeX &&
			pt->y <= m_posY + m_sizeY &&
			pt->x >= m_posX &&
			pt->y >= m_posY;
}