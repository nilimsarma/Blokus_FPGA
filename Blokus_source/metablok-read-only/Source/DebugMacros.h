/* ===========================================================================

	Project: DirectX Engine - DebugMacros

	Description:
	 Defines some debug macros to be embedded in _DEBUG flagged builds.

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

// Windows headers
#include <windows.h>

// C++ Standard IO
#include <iostream>

// Debug flag
#define DX_DEBUG _DEBUG

// Break into debugger
#define DX_BREAK( ) { if( DX_DEBUG ) DebugBreak( ); }

// Verify / Force eval
#define DX_VERIFY(expr) {							\
	if(!(expr)) if( DX_DEBUG ) {					\
		std::cout << "Verify fails on line "		\
		<< __LINE__ << " with expr \"" << #expr		\
		<< "\"\n"; system("pause"); } }

// Assertion
#define DX_ASSERT(expr) {							\
	if( DX_DEBUG ) if(!(expr)) {					\
		std::cout << "Assertion fails on line "		\
		<< __LINE__ << " with expr \"" << #expr		\
		<< "\"\n"; system("pause"); } }

// Assertion with msg
#define DX_ASSERT_MSG(expr,msg) {	\
	if( DX_DEBUG ) if(!(expr)) {		\
		std::cout << (msg) << "\n";	\
		system("pause"); } }

// Trap / Halt Execution
#define DX_TRAP( ) {							\
	if( DX_DEBUG ) {							\
		std::cout << "Trap reached on line "	\
			<< __LINE__ << "\n";				\
		system("pause"); } }

// Conditional
#define DX_IF_DBG(stmt) { if( DX_DEBUG ) {stmt} }