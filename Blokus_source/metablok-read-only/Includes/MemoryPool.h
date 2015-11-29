/* ===========================================================================

	Project: AI player for Blokus

	Description:
	  Simple memory pool class for allocating constant size block of memory.

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
#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

// Memory pool class
class MemoryPool
{
public:
	// Contruction and destruction
	MemoryPool( ); ~MemoryPool( );

	// Allocation of pool memory
	void allocateMemory( unsigned int chunkSize, unsigned int nChunks );

	// Deallocate pool memory
	void deallocateMemory( ); 

	// Handle chunk requests
	void freeChunk( void* chunk );
	void* getChunk( );

protected:
	// Pointer to allocated mem
	void* m_memoryPtr;

	// Pointer to first chunk
	void* m_firstChunk;
};

// End definition 
#endif