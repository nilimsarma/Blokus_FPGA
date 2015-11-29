/* ===========================================================================

	Project: AI player for Blokus

	Description:
	 Defines some debug macros for use with AI players.

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
#ifdef _DEBUG
#define DEBUG TRUE
#else 
#define DEBUG FALSE
#endif

// Break into debugger
#define BREAK( ) { if( DEBUG ) DebugBreak( ); }

// Verify / Force eval
#define VERIFY(expr) {	    					   \
	if(!(expr)) if( DEBUG ) {					   \
		std::cout << "Verify fails on line "       \
		<< __LINE__ << " with expr \"" << #expr    \
		<< "\"\n"; system("pause"); } }

// Assertion
#define ASSERT(expr) {	    					   \
	if( DEBUG ) if(!(expr)) {					   \
		std::cout << "Assertion fails on line "    \
		<< __LINE__ << " with expr \"" << #expr    \
		<< "\"\n"; system("pause"); } }

// Assertion with msg
#define ASSERT_MSG(expr,msg) {		\
	if( DEBUG ) if(!(expr)) {		\
		std::cout << (msg) << "\n"; \
		system("pause"); } }

// Trap / Halt Execution
#define TRAP( ) {							   \
	if( DEBUG ) {						   	   \
		std::cout << "Trap reached on line "   \
			<< __LINE__ << "\n";			   \
		system("pause"); } }

// Conditional
#define IF_DBG(stmt) { if( DEBUG ) {stmt} }