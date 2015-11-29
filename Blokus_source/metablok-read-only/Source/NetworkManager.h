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

// Begin definition
#ifndef DX_ENGINE_NETWORK_MANAGER_H
#define DX_ENGINE_NETWORK_MANAGER_H

// Engine namespace
namespace DirectX
{

// Connection 
typedef UINT_PTR Socket;

// Connection type
enum ConnectionType
{
	NCT_TCP,	// TCP connection
	NCT_UDP		// UDP connection
};

// Startup error codes
enum NetInitError 
{
	NIE_SYSNOTREADY,	// System not ready
	NIE_INPROGRESS,		// System already active
	NIE_PROCLIM,		// Process limit reached
	NIE_FAULT,			// Internal fault

	NIE_NONE = 0 
};

// Socket op error codes
enum SocketOpError
{
	SOE_AGAIN,	// Command interrupted

	SOE_NONE = 0
};

// Management class
class NetworkManager
{
public:
	// Destructor
	~NetworkManager( ) { shutdown( ); }

	// Startup error output
	NetInitError getInitError( ) { return m_initError; }

	// Startup / Shutdown processes
	NetInitError startup( ); void shutdown( );

	// Network connection management
	Socket createServer( ConnectionType ct, const wchar_t* port );
	Socket connect( ConnectionType ct, const wchar_t* port );
	Socket acceptConnection( Socket serverSocket );
	int closeConnection( Socket connection );

	// Socket error checking
	SocketOpError getSocketError( Socket connection ); 

	// Connection blocking
	int makeNonBlocking( Socket connection );
	int makeBlocking( Socket connection );

	// Network communication
	void sendPacket( Socket connection, char* packet, int bytes );
	void recvPacket( Socket connection, char* packet, int bytes );

	// Singleton accessor	
	static NetworkManager* instance( )
	{
		static NetworkManager gSingleton;
		return &gSingleton;
	}

private:
	NetworkManager( ) { }

	NetInitError m_initError;
};

}

// End file definition
#endif