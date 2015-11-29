/* ===========================================================================

	Project: MetaBlok - Match

	Description: Simulator for Blockus matches

    Copyright (C) 2011 Lucas Sherman, David Gloe, Mary Southern, Tobias Gulden

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
#ifndef MATCH_H
#define MATCH_H

// Match settings structure
struct MatchSettings
{
	// Match configurations
	std::wstring aiName[4];	//< AIs player filenames
	int matchType;			//< Type of match to play
	int matchTime;			//< Match turn timer, in minutes
};

// Match game state 
class Match : public DirectX::Controller
{
public:
	// Constructor
	Match( ) { }

	// Primary methods
	void startup( );
	void main( );
	void shutdown( );

	// Overridden controller functions
	void onKeyboardEvent( UINT message, WPARAM wParam, LPARAM lParam );
	void onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam );
	void onRenderFrame( DirectX::Camera* camera );
	void onDeviceReset( );

	// Initiates a new match
	bool beginNewMatch( MatchSettings* settings );

	// Load/save from file operations
	int loadFromDisk( const wchar_t* filename );
	int saveToDisk( const wchar_t* filename );

	// Undo / Redo operations
	void undoMove( ); void redoMove( );

	// Launches the specified AI player file
	void setAiPlayer( int player, const wchar_t* name );

	// Controls match execution
	void stop( ); void play( ); void step( ); 

	// Returns TRUE if the match is over
	int isOver( ) { return m_matchEnd; }
						
private:
	// DirectX Engine Components
	DirectX::Manager* m_manager;
	DirectX::Camera m_camera;
	DirectX::Effect m_shader;
	DirectX::Timer m_timer;

	// Game Board and Pieces
	DirectX::Mesh m_gameBoardMesh;
	DirectX::Object m_gamePieces[4][21];
	DirectX::Mesh m_gamePieceMeshes[4][21];
	DirectX::Image m_gamePieceTexture;
	DirectX::Image m_gameBoardTexture;
	DirectX::Image m_shadowMapTexture;

	// Game Board Markers
	DirectX::Object m_startMarker[4];
	DirectX::Mesh m_startMarkerMesh[4];

	// 3D Mouse Information
	DirectX::Vector3 m_mousePosition;	//< Current 3D Mouse position
	DirectX::Vector3 m_mouseDirection;  //< Current 3D Mouse direction

	// Piece layouts singleton
	PieceSet* m_gamePieceLayouts;	

	// Match State
	Move m_moveHistory[42];			//< History of each move this game
	int m_currentPly;				//< Current move ply
	int m_startTile[4][2];			//< Start tile of each player
	char m_board[20][20];			//< Game board state
	int m_boardSize;				//< Size in tiles of game board
	bool m_pieces[4][21];			//< Pieces remaining for each player
	int m_currentPlayer;			//< Current player to move
	int m_numberOfPlayers;			//< Number of players in this match
	int m_matchMode;				//< Match mode (duo or classic)
	int m_score[4];					//< Current score of each player
	int m_matchEnd;					//< Match completed indicator

	// Training and Analysis
	int m_debugText;				//< Display extra debug information
	int m_waitTurn;					//< Wait for keypress for AI turn
	int m_record[4];				//< Record of matches won/lost
	int m_matches;					//< Number of matches played

	// Player move selection
	DirectX::Vector3 m_mouseDragOffset;
	int m_intersectedPiece, m_selectedPiece;
	Move m_piecePosition[2][21];
	Move m_playerMove;

	// AI player memory map and views
	std::wstring m_agentFilename[4];
	std::wstring m_memoryMapName[4];
	PROCESS_INFORMATION m_pinfo[4];
	HANDLE m_memoryMappedFile[4];
	GameData* m_memoryView[4];

	// Initialization Processes
	void load3dPiecePositions( );
	void load3dGameResources( );

	// Applies Modified Settings
	void applyMatchSettings( );

	// Match Flow Management
	void reset3dGameBoard( );
	void onMatchEnd( );

	// Forward history for redo move
	std::list<Move> m_undoHistory;

	// Game play functions
	void makeMove( Move move );
	bool isValidMove( Move move );
	bool isMoveAvailable( );

	// AI player I/O
	void buildMemoryMap( );
    void getAiMove( );

	// Piece processing helper functions
	void setPiecePosition( Move move, int player );
	DirectX::Vector3 getPiecePosition( Move move );
	bool isValidPattern( int pieceNumber, bool* liberty, 
			int px, int py, int gx, int gy );

	// Debug Rendering functions
	void displaySystemInfo( );
	void displayMatchInfo( );
	void displayDebugInfo( );
};

// End definition
#endif
