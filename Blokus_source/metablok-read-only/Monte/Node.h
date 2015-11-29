/* ===========================================================================

	Project: Monte AI for Blokus

	Description:
	 Defines the structure of a node in the Monte Carlo search tree.

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
#ifndef NODE_H
#define NODE_H

// Define Node
class Node
{
public:

private:
    MoveLists m_moveLists; //< Stores this nodes enumerated move lists
	float m_nVisits;	   //< Number of times this node has been visited
	float m_value;		   //< The estimated value of this node

};

// End definition
#endif