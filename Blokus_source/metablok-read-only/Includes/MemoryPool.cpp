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
 
// Debug definitions
#include "Debug.h"

// Header include
#include "MemoryPool.h"

// --------------------------------------------------------
//	Constructor - Initializes internal pointers to NULL.
// --------------------------------------------------------
MemoryPool::MemoryPool( )
{
	m_firstChunk = NULL;
	m_memoryPtr = NULL;
}
//
// --------------------------------------------------------
//	Destructor - Deallocates any allocated memory.
// --------------------------------------------------------
MemoryPool::~MemoryPool( )
{
	// Deallocate memory
	if( m_memoryPtr != NULL )
		deallocateMemory( );
}
//
// --------------------------------------------------------
//	AllocateMemory - Allocates a block of memory for the
//  memory pool and constructs an internal list of chunks.
// --------------------------------------------------------
void MemoryPool::allocateMemory( unsigned int chunkSize, unsigned int nChunks )
{
	// Allocate memory
	m_memoryPtr = malloc( chunkSize*nChunks );
	m_firstChunk = m_memoryPtr;

	// Generate internal list of free chunks
	char* address = (char*)m_memoryPtr;
	for( unsigned int i = 0; i < nChunks-1; i++ )
		*(void**)(address+i*chunkSize) = (void*)(address+(i+1)*chunkSize);

	// Mark terminal chunk for debugging
	*(void**)(address+(nChunks-1)*chunkSize) = NULL;
}
//
// --------------------------------------------------------
//	DellocateMemory - Deallocates any memory allocated for
//  the pool and sets the internal pointers to NULL.
// --------------------------------------------------------
void MemoryPool::deallocateMemory( )
{
	if( m_memoryPtr )
	{
		free( m_memoryPtr );
		m_firstChunk = NULL;
		m_memoryPtr = NULL;
	}
}
//
// --------------------------------------------------------
//	GetChunk - Returns a free chunk from the internal list
//  of free chunks. Asserts no memory overflow occurs.
// --------------------------------------------------------
void* MemoryPool::getChunk( )
{
	// Grab chunk element off top of stack
	void* freedChunk = m_firstChunk;
	m_firstChunk = *(void**)(m_firstChunk);

	// Catch memory filled
	ASSERT( m_firstChunk )

	// Return the chunk pointer
	return freedChunk;
}
//
// --------------------------------------------------------
//	FreeChunk - Returns a chunk to the internal list
// --------------------------------------------------------
void MemoryPool::freeChunk( void* chunk )
{
	// Attach element to top of stack
	*(void**)chunk = m_firstChunk;
	m_firstChunk = chunk;
}