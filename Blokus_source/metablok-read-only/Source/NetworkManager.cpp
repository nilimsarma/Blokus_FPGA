/* ===========================================================================

	Project: Game Engine - Network Manager

	Description: Implements basic networking functionality using the Windows 
	 Winsock API:

	http://msdn.microsoft.com/en-us/library/windows/desktop/ms740673%28v=vs.85%29.aspx

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

// Winsock library
#pragma comment(lib, "Ws2_32.lib")

// Winsock networking
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

// Include header
#include "NetworkManager.h"

// Winsock API version info
static const int g_version = 2;
static const int g_highVersion = 2;

// --------------------------------------------------------
//	Calls the winsock startup functions and checks for any
//  initialization errors.
// --------------------------------------------------------
DirectX::NetInitError DirectX::NetworkManager::startup( )
{
	WSADATA wsaData; 
	WORD wVersion = MAKEWORD( g_version, g_highVersion );
	m_initError = (NetInitError)WSAStartup( wVersion, &wsaData );
	if( m_initError != NIE_NONE ) WSACleanup( );
	return m_initError;
}
//
// --------------------------------------------------------
//	Shutsdown the networking manager and invalidates any
//  dangling connection handles.
// --------------------------------------------------------
void DirectX::NetworkManager::shutdown( )
{
	WSACleanup( );
}
//
// --------------------------------------------------------
//  Creates a server socket and binds it to the specified
//  port for client connection initialization. After the
//  socket is bound the process will automatically begin
//  listening for incoming connections.
// --------------------------------------------------------
DirectX::Socket DirectX::NetworkManager::createServer( ConnectionType ct, const wchar_t* port )
{
	struct addrinfoW *result = NULL, hints;

	// Construct address info structure
	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;

	// Determine socket type
	switch( ct )
	{
		case NCT_TCP: hints.ai_socktype = SOCK_STREAM; break;
		case NCT_UDP: hints.ai_socktype = SOCK_DGRAM; break;
	}

	// Resolve the local address and port to be used by the server
	int iResult = GetAddrInfoW( NULL, port, &hints, &result );

	// Detect getaddrinfo error
	if( iResult != 0 ) 
	{
		WSACleanup( );
		return NULL;
	}

	// Establish socket for detection of incoming connections
	SOCKET listenSocket = socket( result->ai_family, result->ai_socktype, result->ai_protocol );

	// Detect socket initialization failure
	if( listenSocket == INVALID_SOCKET ) 
	{
		FreeAddrInfoW( result );
		WSACleanup( );
		return NULL;
	}

	// Bind the listening socket
    iResult = bind( listenSocket, result->ai_addr, (int)result->ai_addrlen );

	// Free address info memory
	FreeAddrInfoW( result );

	// Detect binding failure
    if( iResult == SOCKET_ERROR ) 
	{
        closesocket( listenSocket );
        return NULL; 
	}

	// Begin listening on the newly initialized socket
	if( listen( listenSocket, SOMAXCONN ) == SOCKET_ERROR ) 
	{
		closesocket( listenSocket );
		WSACleanup( );
		return NULL;
	}

	return (Socket)listenSocket;
}
//
// --------------------------------------------------------
//  Accepts the next available client on the specified
//  socket. The socket must have been created from a call 
//  to createServer or the attempt will fail.
// --------------------------------------------------------
DirectX::Socket DirectX::NetworkManager::acceptConnection( Socket serverSocket )
{
	// Accept a client socket 
	SOCKET clientSocket = accept( serverSocket, NULL, NULL );

	// Detect connection failure
	if( clientSocket == INVALID_SOCKET ) return NULL;

	return (Socket)clientSocket;
}
//
// --------------------------------------------------------
//  Closes the specified socket and terminates the 
//  connection. Before calling this function you may want
//  to shutdown the socket using the shutdownConnection
//  function.
// --------------------------------------------------------
int DirectX::NetworkManager::closeConnection( Socket connection )
{
	return closesocket( (SOCKET)connection );
}
//
// --------------------------------------------------------
//  Enables non-blocking connection attempts on the socket.
// --------------------------------------------------------
int DirectX::NetworkManager::makeNonBlocking( DirectX::Socket connection )
{
	unsigned long imode = 1;
	return ioctlsocket( (SOCKET)connection, FIONBIO, &imode );
}
//
// --------------------------------------------------------
//  Enables blocking connection attempts on the socket.
// --------------------------------------------------------
int DirectX::NetworkManager::makeBlocking( DirectX::Socket connection )
{
	unsigned long imode = 0;
	return ioctlsocket( (SOCKET)connection, FIONBIO, &imode );
}
//
// --------------------------------------------------------
//  Returns a socket operation error code.
// --------------------------------------------------------
DirectX::SocketOpError DirectX::NetworkManager::getSocketError( Socket connection )
{
	return SOE_NONE;
}