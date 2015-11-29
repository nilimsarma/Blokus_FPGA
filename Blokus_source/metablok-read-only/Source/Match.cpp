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

// Standard includes
#include "MetaBlok.h"

// Include header
#include "Match.h"

// System timer
#include <time.h>

// Debug AI modules
#define DEBUG_AIS  0

// Selected piece
#define NONE  -1

// Waiting states
#define WAIT_NONE	  0
#define WAIT_HALTED   1
#define WAIT_STEP	  2

// --------------------------------------------------------
//	Initializes the state.
// --------------------------------------------------------
void Match::startup( )
{	
	// Store handle to DirectX manager
	m_manager = DirectX::Manager::instance( );

	// Load game piece layouts 
	m_gamePieceLayouts = PieceSet::instance( );

	// Register this state as a controller
	m_manager->addController( this );

	// Initialize variables
	for( int i = 0; i < 4; i++ ) 
		m_pinfo[i].hProcess = NULL;
	m_debugText = FALSE;

	// Build memory map
	buildMemoryMap( );

	// Load video resources
	load3dPiecePositions( );
	load3dGameResources( ); 

	// Initialize settings
	applyMatchSettings( );

	// Begin initial match
	beginNewMatch( NULL );
}
//
// --------------------------------------------------------
//  Manages player move selection and prompting for AI move
//  selection through memory mapped files.
// --------------------------------------------------------
void Match::main( )
{
	// Strafe camera on keyboard input
	if( !m_manager->isFocusedControl( ) && !m_manager->getKeyState(KEY_LCTRL) ) {
		bool updateCamera = false; float moveDist = 20.0f*m_manager->getElapsedTime( );
		if ( m_manager->getKeyState( KEY_D ) ) { m_camera.moveX(  moveDist ); updateCamera = true; }
		if ( m_manager->getKeyState( KEY_A ) ) { m_camera.moveX( -moveDist ); updateCamera = true; }
		if ( m_manager->getKeyState( KEY_W ) ) { m_camera.moveZ(  moveDist ); updateCamera = true; }
		if ( m_manager->getKeyState( KEY_S ) ) { m_camera.moveZ( -moveDist ); updateCamera = true; }
		if( updateCamera ) { m_camera.updateViewMatrix( ); m_camera.updateFrustum( ); } }

	// Get camera mouse ray data
	DirectX::Vector3 mouseposition; m_camera.getRay( NULL, &mouseposition, &m_mouseDirection );
	m_mousePosition = mouseposition - m_mouseDirection * (mouseposition.y/m_mouseDirection.y); 

	// Return if the match is already over
	if( m_matchEnd == TRUE ) return;

	// Check if an AI player is next to move 
	if( m_agentFilename[m_currentPlayer] != L"" &&
	  ( m_waitTurn == WAIT_NONE ||
		m_waitTurn == WAIT_STEP ) )
	{
		unsigned long exitCode;

		// Verify AI still active
		GetExitCodeProcess( m_pinfo[m_currentPlayer].hProcess, &exitCode );
		if( exitCode != STILL_ACTIVE ) m_agentFilename[m_currentPlayer] = L"";

		// Check move ready
		else if( m_memoryView[m_currentPlayer]->moveReady )
			if( isValidMove( m_memoryView[m_currentPlayer]->move ) ) 
			{
				// Clear forward history
				m_undoHistory.clear( ); 

				// Make the move on the board
				m_memoryView[m_currentPlayer]->moveReady = FALSE;
				makeMove( m_memoryView[m_currentPlayer]->move );

				// Update waiting indicator
				if( m_waitTurn == WAIT_STEP ) m_waitTurn = WAIT_HALTED;
			} 
			else DebugBreak( );

		// Launch AI move selection thread
		else if( !m_memoryView[m_currentPlayer]->turnReady ) getAiMove( );
	} 
	
	// Handle player move selections
	if( m_agentFilename[m_currentPlayer] == L"" || m_waitTurn == WAIT_HALTED ) 
	{
		// Check for no piece selected
		if( m_selectedPiece == NONE ) {
			float objDist = m_camera.getMaxDist( ); m_intersectedPiece  = NONE;
			for( int i = 0; i < 21; i++ ) if( m_pieces[m_currentPlayer][i] ) {

				// Intersection buffer variable declarations
				unsigned long face; float bary1, bary2, dist; int hit;

				// Perform intersection test with the object
				m_gamePieces[m_currentPlayer][i].intersectRay( &mouseposition, 
					&m_mouseDirection, &hit, &face, &bary1, &bary2, &dist );

				// If the mesh intersected closer than last hit update info
				if( hit && objDist >= dist ) m_intersectedPiece = i; } }

		// Handle piece display
		if( m_selectedPiece != NONE )
		{
			// Determine piece boundaries for snapping
			int sizeX = m_gamePieceLayouts->getSizeX( m_selectedPiece );
			int sizeY = m_gamePieceLayouts->getSizeY( m_selectedPiece );
			if( m_playerMove.rotated == PIECE_ROTATE_270 ||
				m_playerMove.rotated == PIECE_ROTATE_90 ) {
					sizeX = m_gamePieceLayouts->getSizeY( m_selectedPiece );
					sizeY = m_gamePieceLayouts->getSizeX( m_selectedPiece ); }

			// Compute piece position offset
			DirectX::Vector3 offset; offset.y = m_mouseDragOffset.y;
			float ct = cos( m_playerMove.rotated * D3DX_PI / 2.0f );
			float st = sin( m_playerMove.rotated * D3DX_PI / 2.0f );
			offset.x = ct * m_mouseDragOffset.x - st * m_mouseDragOffset.z;
			offset.z = st * m_mouseDragOffset.x + ct * m_mouseDragOffset.z;
			if( m_playerMove.flipped ) { offset.x *= -1; offset.y += 0.2f; }

			// Compute transformed grid position
			DirectX::Vector3 position = m_mousePosition + offset;
			m_playerMove.gridX =  (int)(position.x + (float)m_boardSize/2.0f + 2.0f) - 2; 
			m_playerMove.gridY = -(int)(position.z - (float)m_boardSize/2.0f - 2.0f) - 2; 
			if( m_playerMove.rotated == PIECE_ROTATE_180 ||
				m_playerMove.rotated == PIECE_ROTATE_270 ) m_playerMove.gridX -= sizeX-1;
			if( m_playerMove.rotated == PIECE_ROTATE_90 ||
				m_playerMove.rotated == PIECE_ROTATE_180 ) m_playerMove.gridY -= sizeY-1;
			if( m_playerMove.flipped ) 
				if( m_playerMove.rotated == PIECE_ROTATE_180 ||
				m_playerMove.rotated == PIECE_ROTATE_270 )  m_playerMove.gridX += sizeX-1;
				else m_playerMove.gridX -= sizeX-1;

			// Snap piece to integer grid coordinates if it is within valid boundaries
			if( m_playerMove.gridX > -2 && m_playerMove.gridX < m_boardSize-sizeX+2 && 
				m_playerMove.gridY > -2 && m_playerMove.gridY < m_boardSize-sizeY+2 ) 
				setPiecePosition( m_playerMove, m_currentPlayer );
			else 
			{
				// Compute piece rotation parameters
				float ay = D3DX_PI/2.0f-(float)m_playerMove.rotated*D3DX_PI/2.0f;
				float az = m_playerMove.flipped ? D3DX_PI : 0.0f;

				// Reposition piece and update transforms
				m_gamePieces[m_currentPlayer][m_playerMove.pieceNumber].setPosition( &position );
				m_gamePieces[m_currentPlayer][m_playerMove.pieceNumber].setRotation( 0.0f, ay, az );
				m_gamePieces[m_currentPlayer][m_playerMove.pieceNumber].update( );
			}
		}
	}
}
//
// --------------------------------------------------------
//	Terminate AI players and deregister as a controller.
// --------------------------------------------------------
void Match::shutdown( )
{	
	// Terminate old AI agents
	for( int i = 0; i < 4; i++ )
	if( m_pinfo[i].hProcess )
		TerminateProcess( m_pinfo[i].hProcess, 0 );

	// Delete this object as a controller
	m_manager->deleteController( this );
}
//
// --------------------------------------------------------
//	Renders the game board and piece models.
// --------------------------------------------------------
void Match::onRenderFrame( DirectX::Camera* camera ) 
{
	// Render match info
	displayMatchInfo( );

	// Render debug info
	if( m_debugText ) 
	{ 
		displaySystemInfo( ); 
		displayDebugInfo( ); 
	}

	// Render game board

	// Precompute view-proj matrix
	D3DXMATRIX viewProj; D3DXMatrixMultiply( &viewProj, 
		camera->getViewMatrix( ), camera->getProjMatrix( ) ); 

	// Acquire handle to shader
	LPD3DXEFFECT shader = m_shader.get( );

	// Set shader technique to gameboard 
	shader->SetTechnique( "RenderGameBoard" );

	// Set the WVP to the VP matrix
	shader->SetMatrix( "wvp", &viewProj );

	// Acquire pointers to meshes D3D resources 
	LPDIRECT3DTEXTURE9 texture = m_gameBoardTexture.getImage( );
	LPD3DXMESH mesh = m_gameBoardMesh.getMesh( ); 

	// Load texture into shader
	shader->SetTexture( "xTexture", texture );

	// Update changes in shader
	shader->CommitChanges( );

	// Render game board
	unsigned int passes; shader->Begin( &passes, 0 );
	for( unsigned int iPass = 0; iPass < passes; iPass++ )
	{
		// Begin pass
		shader->BeginPass(iPass);

		// Render game board
		mesh->DrawSubset( 0 );

		// Endpass
		shader->EndPass( );
	} shader->End( );
}
//
// --------------------------------------------------------
//	Updates the camera settings on display mode changes.
// --------------------------------------------------------
void Match::onDeviceReset( )
{
	// Update camera view and projection transforms
	DirectX::DisplayMode displayMode = m_manager->getDisplayMode( );
	m_camera.setViewPort( displayMode.Width, displayMode.Height ); 
	m_camera.setAspectRatio( (float)displayMode.Width / 
		(float)displayMode.Height );
	
	// Apply changes
	m_camera.update( );
}
//
// --------------------------------------------------------
//	Handles hotkey functionality and debug toggling.
// --------------------------------------------------------
void Match::onKeyboardEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
	// Filter out of focus events 
	if( m_manager->isFocusedControl( ) ) return;

	// Get scancode of key
	int key = MapVirtualKey( ( BYTE )( wParam & 0xFF ), MAPVK_VK_TO_VSC );

	// Key pressed
	if( message == WM_KEYDOWN )
	{
		// Debug rendering
		if( key == KEY_TILDA ) m_debugText = 1 - m_debugText;

		// Rotate selected piece
		if( key == KEY_R ) m_playerMove.rotated = (m_playerMove.rotated+1) % 4;

		// Flip selected piece
		if( key == KEY_F ) m_playerMove.flipped = 1 - m_playerMove.flipped;
	}
}
//
// --------------------------------------------------------
//	Handles human piece selection and deselection.
// --------------------------------------------------------
void Match::onMouseEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
	// Filter out of focus events
	if( m_manager->isMousedOverControl( ) ) return;

	switch( message ) 
	{
		case WM_LBUTTONDOWN:

			// Select a playable piece
			if( m_selectedPiece == NONE && m_intersectedPiece != NONE )
			{
				// Select intersected piece and comput offsets
				m_selectedPiece = m_intersectedPiece;
				D3DXVec3Subtract( &m_mouseDragOffset, 
					m_gamePieces[m_currentPlayer][m_selectedPiece].getPosition( ), 
					&m_mousePosition );

				// Construct partial move 
				m_playerMove.pieceNumber = m_selectedPiece;
				m_playerMove.rotated = 0;
				m_playerMove.flipped = 0;
			}

			break;

		case WM_LBUTTONUP:

			// Deselect/Drop the piece
			if( m_selectedPiece != NONE ) 
			{
				// Check if the move is valid
				if( isValidMove( m_playerMove ) ) {

					// Make the move on the board
					makeMove( m_playerMove );

				} else {
					Move defaultPos = m_piecePosition[MODE_DUO][m_selectedPiece];
					if( m_currentPlayer == PLAYER_RED ) defaultPos.gridX += 30;
					setPiecePosition( defaultPos, m_currentPlayer );
				}

				// Clear selection 
				m_intersectedPiece = NONE;
				m_selectedPiece = NONE;
			}

			break;
	}
}
//
// --------------------------------------------------------
//	Builds the meshes and objects for the game grid and 
//  piece models and loads the shader.
// --------------------------------------------------------
void Match::load3dGameResources( )
{
	// Get direct3D device display settings
	int displayX = m_manager->getDisplayWidth( );
	int displayY = m_manager->getDisplayHeight( );

	// Create primary scene camera
	m_manager->addCamera( &m_camera );
	m_camera.setViewPort( displayX, displayY );
	m_camera.setAspectRatio( (float)displayX / (float)displayY );
	m_camera.setPosition( 0.0f, 20.0f, 0.0f );
	m_camera.point( &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
	m_camera.update( ); 

	// Load effect shader
	m_shader.create( L"Shader.fx" );
	m_shader.get( )->SetValue( "xLightPosition", &DirectX::Vector3( 7.0f, 7.0f, 7.0f ), 12 );
	m_shader.get( )->SetVector( "xLightPower", &DirectX::Vector4( 0.7f, 0.7f, 0.7f, 0.0f ) );
	m_shader.get( )->SetVector( "xAmbient", &DirectX::Vector4( 0.3f, 0.3f, 0.3f, 1.0f ) );

	// Load board mesh and texture 
	m_gameBoardMesh.create( L"Models/Game Board/BlokusBoardBasic.x" );
	m_gameBoardTexture.create( L"Models/Game Board/BlokusBoardBasic.bmp" );

	// Load ominoe meshes and textures 
	std::wstring path( L"Models/Pieces/Piece " ); std::wstring type( L".x" );
	for( int i = 0; i < 21; i++ ) {
		std::wstringstream file; file << path; file << i; file << type;
		m_gamePieceMeshes[0][i].create( file.str( ).c_str( ) );
		m_gamePieceMeshes[0][i].setColor( 0.0f, 0.0f, 1.0f );
		m_gamePieces[0][i].create( &m_gamePieceMeshes[0][i] );
		m_gamePieceMeshes[1][i].create( file.str( ).c_str( ) );
		m_gamePieceMeshes[1][i].setColor( 1.0f, 0.0f, 0.0f );
		m_gamePieces[1][i].create( &m_gamePieceMeshes[1][i] );
		m_gamePieceMeshes[2][i].create( file.str( ).c_str( ) );
		m_gamePieceMeshes[2][i].setColor( 0.0f, 1.0f, 0.0f );
		m_gamePieces[2][i].create( &m_gamePieceMeshes[2][i] );
		m_gamePieceMeshes[3][i].create( file.str( ).c_str( ) );
		m_gamePieceMeshes[3][i].setColor( 1.0f, 0.0f, 1.0f );
		m_gamePieces[3][i].create( &m_gamePieceMeshes[3][i] ); }

	// Create start pointer meshes
	for( int i = 0; i < 4; i++ )
		m_startMarkerMesh[i].createSphere( 0.25f, 10, 10 );

	// Color code start pointers
	m_startMarkerMesh[0].setColor( 0.0f, 0.0f, 1.0f );
	m_startMarkerMesh[1].setColor( 1.0f, 0.0f, 0.0f );
	m_startMarkerMesh[2].setColor( 0.0f, 1.0f, 0.0f );
	m_startMarkerMesh[3].setColor( 0.5f, 0.0f, 0.5f );
	
	// Instance start pointers
	m_startMarker[0].create( &m_startMarkerMesh[0] );
	m_startMarker[1].create( &m_startMarkerMesh[1] );
	m_startMarker[2].create( &m_startMarkerMesh[2] );
	m_startMarker[3].create( &m_startMarkerMesh[3] );
}
//
// --------------------------------------------------------
//  Initialize default 3D piece arrangements. 
// --------------------------------------------------------
void Match::load3dPiecePositions( )
{
	// Duo mode piece layout
	m_piecePosition[MODE_DUO][0]  = Move(  0, -16,  7, 0, 0 );
	m_piecePosition[MODE_DUO][1]  = Move(  1, -16,  9, 0, 0 );
	m_piecePosition[MODE_DUO][2]  = Move(  2,  -5,  9, 0, 0 );
	m_piecePosition[MODE_DUO][3]  = Move(  3, -14,  7, 0, 0 );
	m_piecePosition[MODE_DUO][4]  = Move(  4, -16, -1, 0, 0 );
	m_piecePosition[MODE_DUO][5]  = Move(  5,  -5, 11, 0, 0 );
	m_piecePosition[MODE_DUO][6]  = Move(  6, -11,  7, 0, 0 );
	m_piecePosition[MODE_DUO][7]  = Move(  7,  -7,  6, 0, 0 );
	m_piecePosition[MODE_DUO][8]  = Move(  8, -16, 11, 0, 0 );
	m_piecePosition[MODE_DUO][9]  = Move(  9, -16,  4, 0, 0 );
	m_piecePosition[MODE_DUO][10] = Move( 10,  -9, 11, 0, 0 );
	m_piecePosition[MODE_DUO][11] = Move( 11, -13, 10, 0, 0 );
	m_piecePosition[MODE_DUO][12] = Move( 12, -11,  9, 0, 0 );
	m_piecePosition[MODE_DUO][13] = Move( 13,  -6,  4, 0, 0 );
	m_piecePosition[MODE_DUO][14] = Move( 14,  -5, -1, 0, 0 );
	m_piecePosition[MODE_DUO][15] = Move( 15, -13,  4, 0, 0 );
	m_piecePosition[MODE_DUO][16] = Move( 16,  -7,  0, 0, 0 );
	m_piecePosition[MODE_DUO][17] = Move( 17, -12,  0, 0, 0 );
	m_piecePosition[MODE_DUO][18] = Move( 18, -11, -1, 0, 0 );
	m_piecePosition[MODE_DUO][19] = Move( 19,  -9,  3, 0, 0 );
	m_piecePosition[MODE_DUO][20] = Move( 20, -15,  1, 0, 0 );

	// Classic mode piece layout
	m_piecePosition[MODE_CLASSIC][0]  = Move(  0, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][1]  = Move(  1, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][2]  = Move(  2, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][3]  = Move(  3, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][4]  = Move(  4, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][5]  = Move(  5, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][6]  = Move(  6, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][7]  = Move(  7, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][8]  = Move(  8, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][9]  = Move(  9, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][10] = Move( 10, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][11] = Move( 11, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][12] = Move( 12, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][13] = Move( 13, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][14] = Move( 14, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][15] = Move( 15, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][16] = Move( 16, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][17] = Move( 17, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][18] = Move( 18, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][19] = Move( 19, 0, 0, 0, 0 );
	m_piecePosition[MODE_CLASSIC][20] = Move( 20, 0, 0, 0, 0 );
}
//
// --------------------------------------------------------
//	Applies new match settings and starts a new game.
// --------------------------------------------------------
void Match::applyMatchSettings( )
{
	// Update match type
	m_matchMode = MODE_DUO;

	// Wait options
	m_waitTurn = WAIT_HALTED; 

	// Number of players
	if( m_matchMode == MODE_DUO ) m_numberOfPlayers = 2;
	else m_numberOfPlayers = 2;

	// Board dimensions
	if( m_matchMode == MODE_DUO ) m_boardSize = 14;
	else m_boardSize = 20;

	// Starting tile
	if( m_matchMode == MODE_DUO ) {
		m_startTile[PLAYER_BLUE][0] = 4;
		m_startTile[PLAYER_BLUE][1] = 4;
		m_startTile[PLAYER_RED] [0] = 9;
		m_startTile[PLAYER_RED] [1] = 9;
	} else {
		m_startTile[PLAYER_BLUE][0] = 0;
		m_startTile[PLAYER_BLUE][1] = 0;
		m_startTile[PLAYER_RED][0] = 19;
		m_startTile[PLAYER_RED][1] = 0;
		m_startTile[PLAYER_GREEN][0] = 19;
		m_startTile[PLAYER_GREEN][1] = 19;
		m_startTile[PLAYER_YELLOW][0] = 0;
		m_startTile[PLAYER_YELLOW][1] = 19; }

	// Set player types
	for( int i = 0; i < 4; i++ ) 
		m_agentFilename[i] = L"";

	// Terminate old AI processes
	for( int i = 0; i < 4; i++ )
	if( m_pinfo[i].hProcess )
		TerminateProcess( m_pinfo[i].hProcess, 0 );

	// Launch new AI processes
	for( int i = 0; i < m_numberOfPlayers; i++ ) 
		setAiPlayer( i, m_agentFilename[i].c_str( ) );

	// Clear records
	for( int i = 0; i < 5; i++ ) m_record[i] = 0;
}
//
// --------------------------------------------------------
//	Responds to an end match condition. 
// --------------------------------------------------------
void Match::onMatchEnd( )
{
	int winner = 0;

	// Update win/loss record keeping 
	for( int i = 1; i < m_numberOfPlayers; i++ )
		if( m_score[i] > m_score[winner] ) winner = i;
	for( int i = winner; i < m_numberOfPlayers; i++ )
		if( m_score[winner] == m_score[i] ) m_record[i]++;

	// Set the end flag
	m_matchEnd = TRUE;
}
//
// --------------------------------------------------------
//	Begins a new match using the specified game settings.
//  Returns false if the specified settings are invalid.
// --------------------------------------------------------
bool Match::beginNewMatch( MatchSettings* settings )
{
	// Clear board data
	for( int i = 0; i < 20; i++ )
	for( int j = 0; j < 20; j++ )
		m_board[i][j] = GRID_COVER_NONE;

	// Reset player scores
	for( int i = 0; i < 4; i++ )
		m_score[i] = 0;

	// Replace game pieces
	for( int i = 0; i < 4;  i++ )
	for( int j = 0; j < 21; j++ )
		m_pieces[i][j] = true;

	// Launch new AI processes
	for( int i = 0; i < m_numberOfPlayers; i++ ) 
	if( m_agentFilename[i] != L"" ) 
		setAiPlayer( i, m_agentFilename[i].c_str( ) );

	// Clear tracking variables
	m_currentPlayer = PLAYER_BLUE;
	m_intersectedPiece = NONE;
	m_selectedPiece = NONE;
	m_currentPly = 0;

	// Reset 3D game board
	reset3dGameBoard( );

	// Set match end flag
	m_matchEnd = FALSE;

	// Success
	return true;
}
//
// --------------------------------------------------------
//   Resets the 3D game board used to display the current
//	 board layout and available pieces to the human player
// --------------------------------------------------------
void Match::reset3dGameBoard( ) 
{
	// Hide unused game piece models
	for( int i = m_numberOfPlayers; i < 4; i++ ) {
		for( int j = 0; j < 21; j++ ) 
			m_gamePieces[i][j].hide( );
		m_startMarker[i].hide( ); }

	// Position game piece models
	for( int i = 0; i < 21; i++ )
	for( int j = 0; j <  4; j++ ) { 
		Move defaultPos = m_piecePosition[m_matchMode][i];
		if( j == PLAYER_RED ) defaultPos.gridX += 30;
		setPiecePosition( defaultPos, j ); }

	// Position pointers models
	for( int i = 0; i < 4; i++ ) {
		float x = -(float)m_boardSize/2.0f + 0.5f + 1.0f*m_startTile[i][0]; 
		float y =  (float)m_boardSize/2.0f - 0.5f - 1.0f*m_startTile[i][1]; 
		m_startMarker[i].setPosition( x, 0.0f, y );
		m_startMarker[i].update( ); }
}
//
// --------------------------------------------------------
//	Checks the validity of the given move for the current
//  player.
// --------------------------------------------------------
bool Match::isValidMove( Move move )
{
	// Check if piece pattern is available for use
	if( move.pieceNumber < 0 || move.pieceNumber > 21 ||
		!m_pieces[m_currentPlayer][move.pieceNumber] )
		return false;

	// Check for valid rotation angle
	if( move.rotated < PIECE_ROTATE_0 ||
		move.rotated > PIECE_ROTATE_270 )
		return false;

	// Run pattern analysis between grid and piece
	bool coversLiberty = false;
	int gx = move.gridX, gy = move.gridY;
	int x = m_gamePieceLayouts->getSizeX( move.pieceNumber );
	int y = m_gamePieceLayouts->getSizeY( move.pieceNumber );
	if( move.flipped == PIECE_UNFLIPPED ) {
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j = 0, gy = move.gridY; j < y; j++,gy++ )
			for( int i = 0, gx = move.gridX; i < x; i++,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j =   0, gx = move.gridX; j <  y; j++,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i =   0, gy = move.gridY; i <  x; i++,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy ) ) return false; }
	} else {
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j =   0, gy = move.gridY; j <  y; j++,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i =   0, gx = move.gridX; i <  x; i++,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy ) ) return false; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i = 0, gy = move.gridY; i < x; i++,gy++ )
			for( int j = 0, gx = move.gridX; j < y; j++,gx++ )
				if( !isValidPattern( move.pieceNumber, &coversLiberty, i, j, gx, gy ) ) return false; }
	}

	// Check if liberty is covered
	if( coversLiberty ) return true;
	else return false;
}
//
// --------------------------------------------------------
//	Checks if the patterns on the grid and piece are
//	compatible and updates the liberty covered flag
//	if necessary.
// --------------------------------------------------------
bool Match::isValidPattern( int pieceNumber, bool* liberty, 
						   int px, int py, int gx, int gy )
{
	// Check for outside grid bounds on a covering tile
	char piecePattern = m_gamePieceLayouts->indexOf( pieceNumber, px, py );
	if( gx < 0 || gx >= m_boardSize || gy < 0 || gy >= m_boardSize ) 
		if( piecePattern == MATCH_NOT_COVERED ) return false;
		else return true;

	// Check if local patterns are acceptable
	char gridPattern = m_board[gx][gy];
	switch( piecePattern ) {
		case MATCH_NOT_PLAYERS: 
			if( gridPattern == m_currentPlayer ) return false; break;
		case MATCH_NOT_COVERED:
			if( gridPattern != GRID_COVER_NONE ) return false; 
			else if( gx == m_startTile[m_currentPlayer][0] &&
					 gy == m_startTile[m_currentPlayer][1] ) *liberty = true; break;
		case MATCH_LIBERTY:
			if( gridPattern == m_currentPlayer ) *liberty = true; }

	return true;
}
//
// --------------------------------------------------------
//	Updates the game data to reflect the execution of the
//  valid input move.
// --------------------------------------------------------
void Match::makeMove( Move move )
{
	// Map the move to unique value
	move.rotated %= m_gamePieceLayouts->getOrients( move.pieceNumber );
	if( move.flipped && !m_gamePieceLayouts->getFlips( move.pieceNumber ) )
	{ move.flipped = FALSE; move.rotated = (move.rotated + (move.rotated%2) ? 0 : 1 )%4; }

	// Add the move to the move history
	m_moveHistory[m_currentPly] = move;
	m_currentPly++;

	// Update piece registry
	m_pieces[m_currentPlayer][move.pieceNumber] = false;

	// Iterate over piece pattern and update game board
	int gx = move.gridX, gy = move.gridY;
	int x = m_gamePieceLayouts->getSizeX( move.pieceNumber );
	int y = m_gamePieceLayouts->getSizeY( move.pieceNumber );
	if( move.flipped == PIECE_UNFLIPPED )
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j = 0, gy = move.gridY; j < y; j++,gy++ )
			for( int i = 0, gx = move.gridX; i < x; i++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j =   0, gx = move.gridX; j <  y; j++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i =   0, gy = move.gridY; i <  x; i++,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }
	} else {
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j =   0, gy = move.gridY; j <  y; j++,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i =   0, gx = move.gridX; i <  x; i++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i = 0, gy = move.gridY; i < x; i++,gy++ )
			for( int j = 0, gx = move.gridX; j < y; j++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = m_currentPlayer; }
	}

	// Position 3D game piece
	setPiecePosition( move, m_currentPlayer );

	// Update player score variable
	if     ( move.pieceNumber < 1 ) m_score[m_currentPlayer] += 1;
	else if( move.pieceNumber < 2 ) m_score[m_currentPlayer] += 2;
	else if( move.pieceNumber < 4 ) m_score[m_currentPlayer] += 3;
	else if( move.pieceNumber < 9 ) m_score[m_currentPlayer] += 4;
	else m_score[m_currentPlayer] += 5;

	// Update current player variable
	int prevPlayer = m_currentPlayer;
	m_currentPlayer++; m_currentPlayer %= m_numberOfPlayers;
	
	// Skip turns for players unable to move
	while( !isMoveAvailable( ) && prevPlayer != m_currentPlayer)
	{
		// Move to next player
		m_currentPlayer++; m_currentPlayer %= m_numberOfPlayers;

		// Put a skip move on the history
		Move skip; skip.pieceNumber = -1;
		skip.flipped = skip.gridX = skip.gridY = skip.rotated = 0;
		m_moveHistory[m_currentPly] = skip;
		m_currentPly++;
	}

	// Check for game over condition (if no one can move)
	if( prevPlayer == m_currentPlayer && !isMoveAvailable( ) ) 
	{
		// Remove extra skips
		for( int i = 0; i < m_numberOfPlayers; i++ ) 
			m_currentPly--;

		// Run end processes
		onMatchEnd( );
	}
}
//
// --------------------------------------------------------
//	Returns false if there is no available move for the 
//	current player.
// --------------------------------------------------------
bool Match::isMoveAvailable( )
{
	Move move; 

	// Cycle through available pieces
	for( int i = 0; i < 21; i++ ) 
	if( m_pieces[m_currentPlayer][i] )
	{
		move.pieceNumber = i;

		// Cycle through all orients
		for( int x = -1; x < m_boardSize-m_gamePieceLayouts->getSizeY( move.pieceNumber )+2; x++ )
		for( int y = -1; y < m_boardSize-m_gamePieceLayouts->getSizeY( move.pieceNumber )+2; y++ )
		for( int r = 0; r < m_gamePieceLayouts->getOrients( move.pieceNumber ); r++ )
		for( int f = 0; f <= m_gamePieceLayouts->getFlips( move.pieceNumber ); f++ )
		{
			move.flipped = f;
			move.rotated = r;
			move.gridX = x;
			move.gridY = y;

			if( isValidMove( move ) ) 
				return true;
		}
	}

	return false;
}
//
// --------------------------------------------------------
//	Returns the 3D world position of a piece at the 
//  specified grid coordinates and orientations.
// --------------------------------------------------------
DirectX::Vector3 Match::getPiecePosition( Move move )
{
	DirectX::Vector3 offset; offset.y = 0.0f;
	float sizeX = (float)m_gamePieceLayouts->getSizeX( move.pieceNumber );
	float sizeY = (float)m_gamePieceLayouts->getSizeY( move.pieceNumber );

	float x = -(float)m_boardSize/2.0f + 0.5f + (float)move.gridX; 
	float y =  (float)m_boardSize/2.0f - 0.5f - (float)move.gridY;
	DirectX::Vector3 position( x, move.flipped ? 0.2f : 0.0f, y );

	if( move.flipped == PIECE_UNFLIPPED ) {
			 if( move.rotated == PIECE_ROTATE_0   ) { offset.x = 0.0f; offset.z = 0.0f; }
		else if( move.rotated == PIECE_ROTATE_90  ) { offset.x = 0.0f; offset.z = 1.0f-sizeX; }
		else if( move.rotated == PIECE_ROTATE_180 ) { offset.x = sizeX-1.0f; offset.z = 1.0f-sizeY; }
		else if( move.rotated == PIECE_ROTATE_270 ) { offset.x = sizeY-1.0f; offset.z = 0.0f; }
	} else {
			 if( move.rotated == PIECE_ROTATE_0   ) { offset.x = sizeX-1.0f; offset.z = 0; }
		else if( move.rotated == PIECE_ROTATE_90  ) { offset.x = sizeY-1.0f; offset.z = 1.0f-sizeX; }
		else if( move.rotated == PIECE_ROTATE_180 ) { offset.x = 0.0f; offset.z = 1.0f-sizeY; }
		else if( move.rotated == PIECE_ROTATE_270 ) { offset.x = 0.0f; offset.z = 0.0f; }
	}

	return position+offset;
}
//
// --------------------------------------------------------
//  Correctly positions the a piece based on the input move 
//  parameters.
// --------------------------------------------------------
void Match::setPiecePosition( Move move, int player )
{
	// Compute piece position
	DirectX::Vector3 position = getPiecePosition( move );

	// Compute piece rotation angles
	float ay = D3DX_PI/2.0f-(float)move.rotated*D3DX_PI/2.0f;
	float az = move.flipped ? D3DX_PI : 0.0f;

	// Reposition piece and update transforms
	m_gamePieces[player][move.pieceNumber].setPosition( &position );
	m_gamePieces[player][move.pieceNumber].setRotation( 0.0f, ay, az );
	m_gamePieces[player][move.pieceNumber].update( );
}
//
// --------------------------------------------------------
//  Updates the AI memory mapped file for a new turn.
// --------------------------------------------------------
void Match::getAiMove( )
{
	// Copy board structure
	for( int i = 0; i < 20; i++ )
	for( int j = 0; j < 20; j++ )
		m_memoryView[m_currentPlayer]->board[i][j] = m_board[i][j];

	// Copy piece structure
	for( int i = 0; i < 4;  i++ )
	for( int j = 0; j < 21; j++ )
		m_memoryView[m_currentPlayer]->pieces[i][j] = m_pieces[i][j];

	// Copy move history structure
	for( int i = 0; i < m_currentPly; i++ )
		m_memoryView[m_currentPlayer]->moveHistory[i] = m_moveHistory[i];

	// Copy score structure
	for( int i = 0; i < 4; i++ )
		m_memoryView[m_currentPlayer]->score[i] = m_score[i];

	// Copy current move ply
	m_memoryView[m_currentPlayer]->ply = m_currentPly;

	// Copy player
	m_memoryView[m_currentPlayer]->player = m_currentPlayer;

	// Notify the ai process
	m_memoryView[m_currentPlayer]->turnReady = TRUE;
}
//
// --------------------------------------------------------
// Builds the shared memory map files for communicating 
// with AI player processes.
// --------------------------------------------------------
void Match::buildMemoryMap( )
{
	// Generate a new random seed
	srand( (unsigned int)time(NULL) );

	// Generate memory map filenames
	std::wstring basename = std::wstring( L"Blokus" );
	HINSTANCE instance = m_manager->getInstanceHandle( );
	for( int i = 0; i < 4; i++ ) {
		std::wstringstream filename; int key = rand( ); 
		filename << basename << instance 
			<< L"_" << key << L"_" << i;
		m_memoryMapName[i] = filename.str( ); }

	// Build memory maps
	for( int i = 0; i < 4; i++ )
		m_memoryMappedFile[i] = CreateFileMapping( INVALID_HANDLE_VALUE, 
			NULL, PAGE_READWRITE, 0, sizeof(GameData), m_memoryMapName[i].c_str( ) );

	// Create memory map views
	for( int i = 0; i < 4; i++ )
		m_memoryView[i] = (GameData*)MapViewOfFile( m_memoryMappedFile[i], 
			FILE_MAP_ALL_ACCESS, 0, 0, sizeof(GameData) );
}
//
// --------------------------------------------------------
//  Loads an AI player from the specified file.
// --------------------------------------------------------
void Match::setAiPlayer( int player, const wchar_t* name )
{
	// Check if the AI player is unchanged
	if( ( m_currentPlayer != player && 
		m_agentFilename[player] == name ) ||
		player > m_numberOfPlayers ) return;

	// Terminate any pre-existing AI process
	if( m_pinfo[player].hProcess ) {
		TerminateProcess( m_pinfo[player].hProcess, 0 );
		m_pinfo[player].hProcess = NULL; }

	// Check for actual AI file
	m_agentFilename[player] = name;
	if( m_agentFilename[player] == L"" ) return;
	else if( m_agentFilename[player] == L"Human" ) 
		{ m_agentFilename[player] = L""; return; }

	// Load match settings into mapped file
	m_memoryView[player]->nPlayers = m_numberOfPlayers;
	m_memoryView[player]->boardSize = m_boardSize;
	m_memoryView[player]->player = player;

	// Clear indicator variables
	m_memoryView[player]->moveReady = FALSE;
	m_memoryView[player]->matchOver = FALSE;
	m_memoryView[player]->turnReady = FALSE;

	// Load starting tiles into mapped file
	for( int i = 0; i < m_numberOfPlayers; i++ ) {
		m_memoryView[player]->startTile[i][0] = m_startTile[i][0];
		m_memoryView[player]->startTile[i][1] = m_startTile[i][1]; }

	// Copy memory map filename 
	wchar_t commandline[1024];
	commandline[m_memoryMapName[player].size( )] = L'\0';
	for( int i = 0; i < m_memoryMapName[player].size( ); i++ )
		commandline[i] = m_memoryMapName[player].at( i );

	// Get application startup info
	STARTUPINFO sinfo; GetStartupInfo( &sinfo ); 
	std::wstring exename = std::wstring( L"Players//" ) 
		+ m_agentFilename[player];

	// Launch AI agent process
	CreateProcess( exename.c_str( ), commandline, NULL, NULL,
		FALSE, DEBUG_AIS ? NULL : CREATE_NO_WINDOW, NULL, NULL, 
		&sinfo, m_pinfo+player );
}
//
// --------------------------------------------------------
//  Enables non-blocking move making by the AI players.
// --------------------------------------------------------
void Match::play( )
{ 
	// Detrigger the waiting mode option if enabled
	if( m_waitTurn == WAIT_HALTED ) m_waitTurn = WAIT_NONE; 
}
//
// --------------------------------------------------------
//  Enables a single AI move to be made.
// --------------------------------------------------------
void Match::step( )
{ 
	// If play is halted, enable a single AI move
	if( m_waitTurn == WAIT_HALTED ) m_waitTurn = WAIT_STEP;
}
//
// --------------------------------------------------------
//  Stops execution of current and subsequent AI moves.
// --------------------------------------------------------
void Match::stop( )
{ 
	// Enable waiting mode option
	if( m_waitTurn == WAIT_NONE ) 
		m_waitTurn = WAIT_HALTED; 

	// Stop AI player execution
	setAiPlayer( m_currentPlayer, 
		m_agentFilename[m_currentPlayer].c_str( ) );
}
// 
// --------------------------------------------------------
//  Undoes the previous move.
// --------------------------------------------------------
void Match::undoMove( )
{
	// Mark match incomplete
	m_matchEnd = FALSE;

	// Check if the is a prev move
	if( m_currentPly == 0 ) return;

	// Remove the move from the history
	m_currentPly--; Move move = m_moveHistory[m_currentPly];

	// Terminate the ai process and reload
	if( m_agentFilename[m_currentPlayer] != L"" )
		setAiPlayer( m_currentPlayer, m_agentFilename[m_currentPlayer].c_str( ) );

	// Return to the previous player
	if( !m_currentPlayer ) 
		m_currentPlayer = m_numberOfPlayers;
	m_currentPlayer--;

	// Check if the move was a skip
	if( move.pieceNumber == -1 ) { undoMove( ); return; }
	m_undoHistory.push_front( move );

	// If the next player is AI pause move
	if( m_agentFilename[m_currentPlayer] != L"" && 
		m_waitTurn == WAIT_NONE ) m_waitTurn = WAIT_HALTED;

	// Move the piece off the board
	int x = m_gamePieceLayouts->getSizeX( move.pieceNumber );
	int y = m_gamePieceLayouts->getSizeY( move.pieceNumber );
	if( move.flipped == PIECE_UNFLIPPED )
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j = 0, gy = move.gridY; j < y; j++,gy++ )
			for( int i = 0, gx = move.gridX; i < x; i++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = GRID_COVER_NONE; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j =   0, gx = move.gridX; j <  y; j++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = GRID_COVER_NONE; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = GRID_COVER_NONE; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i =   0, gy = move.gridY; i <  x; i++,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = GRID_COVER_NONE; }
	} else 
	{
		if( move.rotated == PIECE_ROTATE_0 ) {
			for( int j =   0, gy = move.gridY; j <  y; j++,gy++ )
			for( int i = x-1, gx = move.gridX; i >= 0; i--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = GRID_COVER_NONE; }

		else if( move.rotated == PIECE_ROTATE_90 ) {
			for( int i = x-1, gy = move.gridY; i >= 0; i--,gy++ )
			for( int j = y-1, gx = move.gridX; j >= 0; j--,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = GRID_COVER_NONE; }

		else if( move.rotated == PIECE_ROTATE_180 ) {
			for( int j = y-1, gy = move.gridY; j >= 0; j--,gy++ )
			for( int i =   0, gx = move.gridX; i <  x; i++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = GRID_COVER_NONE; }

		else if( move.rotated == PIECE_ROTATE_270 ) {
			for( int i = 0, gy = move.gridY; i < x; i++,gy++ )
			for( int j = 0, gx = move.gridX; j < y; j++,gx++ )
				if( m_gamePieceLayouts->indexOf( move.pieceNumber, i, j ) == MATCH_NOT_COVERED ) 
					m_board[gx][gy] = GRID_COVER_NONE; }
	}

	// Move physical piece off the board
	Move defaultPos = m_piecePosition[m_matchMode][move.pieceNumber];
	if( m_currentPlayer == PLAYER_RED ) defaultPos.gridX += 30;
	setPiecePosition( defaultPos, m_currentPlayer );

	// Update piece registry
	m_pieces[m_currentPlayer][move.pieceNumber] = true;

	// Update player score variable
	if     ( move.pieceNumber < 1 ) m_score[m_currentPlayer] -= 1;
	else if( move.pieceNumber < 2 ) m_score[m_currentPlayer] -= 2;
	else if( move.pieceNumber < 4 ) m_score[m_currentPlayer] -= 3;
	else if( move.pieceNumber < 9 ) m_score[m_currentPlayer] -= 4;
	else m_score[m_currentPlayer] -= 5;
}
//
// --------------------------------------------------------
//  Redoes the previous move.
// --------------------------------------------------------
void Match::redoMove( )
{
	// Check if a move was undone
	if( m_undoHistory.empty( ) ) return;

	// Remove the move from the undo history
	Move move = m_undoHistory.front( );
	m_undoHistory.pop_front( );

	// Check if the last move was a skip
	if( move.pieceNumber == NONE ) return;

	// If the next player is AI pause
	if( m_agentFilename[m_currentPlayer] != L"" && 
		m_waitTurn == WAIT_NONE ) m_waitTurn = WAIT_HALTED;
	else if( m_waitTurn == WAIT_HALTED  ) m_waitTurn = WAIT_NONE;

	// Redo the move 
	makeMove( move );
}
//
// --------------------------------------------------------
//   Saves a game to disk with the specified filename,
//   overwriting an existing file if necessary. If the
//   save attempt fails, the function will return zero.
// --------------------------------------------------------
int Match::saveToDisk( const wchar_t* filename )
{
	// Assert valid filename 
	if( *filename == '\0' ) return FALSE;

	// Compose filename string from directory information
	std::wstring name = std::wstring( filename );
	std::wstring directory = std::wstring( L"Saves" );
	std::wstring filepath = directory + L"\\" + name + L".sav";

	// Create directory if it does not exist
	CreateDirectoryW( directory.c_str( ), NULL );

	// Open the specified file for rewriting, creating it if necessary
	std::fstream file( filepath.c_str( ), std::ios::in | std::ios::out | std::ios::trunc );

	// Check for failure
	if( !file.is_open( ) ) return FALSE;
	 
	// Write version header into the file
	file << "// --------------------------------\n";
	file << "//        Blokus Save File\n";
	file << "// --------------------------------\n";

	// Write match settings into the file
	file << "\n// MatchSettings";
	file << "\nNumber_Of_Players " << m_numberOfPlayers;
	file << "\nSize_Of_Board "	   << m_boardSize;

	// Write start tile data into the file
	file << "\n\n// Start Tiles";
	for( int i = 0; i < m_numberOfPlayers; i++ )
		file << "\nStart_Tile " << i
			<< " " << m_startTile[i][0]
			<< " " << m_startTile[i][1];

	// Write move history into the file
	file << "\n\n// Move History";
	for( int i = 0; i < m_currentPly; i++ ) {
		Move move = m_moveHistory[i];
		if( move.pieceNumber == NONE ) continue;
		file << "\nMove" 
			 << " " << move.pieceNumber 
			 << " " << move.gridX
			 << " " << move.gridY
			 << " " << move.rotated
			 << " " << move.flipped; }

	// Write available pieces into the file
	file << "\n\n// Available Pieces";
	for( int i = 0; i < m_numberOfPlayers; i++ ) {
		file << "\nPieces " << i << " ";
		for( int j = 0; j < 21; j++ )
			file << m_pieces[i][j]; }

	// Write endl
	file << "\n";

	// Close file
	file.close( );

	// Return OK
	return TRUE;
}
//
// --------------------------------------------------------
//   Loads the game from disk with the specified filename.
//	 If the load is unsuccessful or corrupt the function
//	 will return zero.
// --------------------------------------------------------
int Match::loadFromDisk( const wchar_t* filename )
{
	// Compose filename string from directory information
	std::wstring name = std::wstring( filename );
	std::wstring directory = std::wstring( L"Saves" );
	std::wstring filepath = directory + L"\\" + name + L".sav";

	// Create directory if it does not exist
	CreateDirectoryW( directory.c_str( ), NULL );

	// Open the specified file for reading
	std::fstream file( filepath.c_str( ), std::ios::in );

	// Check for failure
	if( !file.is_open( ) ) return FALSE;
	else { for( int i = 0; i < 4; i++ )
			  m_agentFilename[i] = L"";
		   beginNewMatch( NULL ); }

	// Parse file using newline and space delimiters 
	std::string line; while( std::getline( file, line ) )
	{
		// Put line data onto stream for delimination
		std::string token; std::stringstream iss; iss << line;

		// Parse leading token on line
		while( std::getline( iss, token, ' ' ) )
		{
			// Ignore line comments
			if( token.substr( 0, 2 ) == "//" ) break;

			// Read simple state and settings data
			else if( token == "Number_Of_Players" ) iss >> m_numberOfPlayers;
			else if( token == "Size_Of_Board" )		iss >> m_boardSize;
			else if( token == "Wait_Turn" )	;
			else if( token == "Wait_Game" )	;
			else if( token == "Score" ) ; // Score, used for neural network

			// Read starting tiles
			else if( token == "Start_Tile" )
			{
				// Read in player start tile data
				int playerNumber; iss >> playerNumber;
				iss >> m_startTile[playerNumber][0];
				iss >> m_startTile[playerNumber][1];
			}

			// Read player pieces
			else if( token == "Pieces" )
			{
				// Get the player number
				int playerNumber; iss >> playerNumber;

				// Read the players piece bitstring
				std::string bitStr; iss >> bitStr;
				if( bitStr.size( ) != 21 ) return FALSE;
				for( int i = 0; i < bitStr.size( ); i++ )
					if( bitStr.at( i ) == '0' ) 
						m_pieces[playerNumber][i] = false;
					else m_pieces[playerNumber][i] = true;
			}

			// Read in move 
			else if( token == "Move" ) 
			{
				Move move; 

				// Get the move definition
				iss >> move.pieceNumber;
				iss >> move.gridX;
				iss >> move.gridY;
				iss >> move.rotated;
				iss >> move.flipped;

				// Convert to newer version
				// if necessary to do so
				move.rotated %= m_gamePieceLayouts->getOrients( move.pieceNumber );
				if( move.flipped && !m_gamePieceLayouts->getFlips( move.pieceNumber ) )
					move.flipped = FALSE;

				// Make the move
				makeMove( move );
			}

			// Unexpected token
			else return FALSE;
		}
	}

	// Close file
	file.close( );

	// Return OK
	return TRUE;
}
//
// --------------------------------------------------------
//  Displays system information including processor info
//  Based on code taken from msdn at the following page:
//  http://msdn.microsoft.com/en-us/library/
// --------------------------------------------------------
void Match::displaySystemInfo( ) 
{
	SYSTEM_INFO siSysInfo;
	GetSystemInfo( &siSysInfo ); 

	std::wstringstream systemInfoStream;
	systemInfoStream << L"Hardware information: \n"
					 << L"  OEM ID: " << siSysInfo.dwOemId << L"\n" 
					 << L"  Number of processors: " << siSysInfo.dwNumberOfProcessors << L"\n"
					 << L"  Processor level: " << siSysInfo.wProcessorLevel << L"\n"
					 << L"  Processor revision: " << siSysInfo.wProcessorRevision << L"\n"
					 << L"  Page size: " << siSysInfo.dwPageSize << L"\n" 
					 << L"  Processor type: " << siSysInfo.dwProcessorType << L"\n"
					 << L"  Minimum application address: " << siSysInfo.lpMinimumApplicationAddress << L"\n"
					 << L"  Maximum application address: " << siSysInfo.lpMaximumApplicationAddress << L"\n"
					 << L"  Active processor mask: " << siSysInfo.dwActiveProcessorMask;
	
	DirectX::Font* font = &DirectX::EngineManager::instance( )->getWindowStyle( )->font;
	font->drawText( 10, 10, systemInfoStream.str( ).c_str( ), COLOR::WHITE );
}
//
// --------------------------------------------------------
//  Displays debug information including move history,
//  memory map filenames, and board configuration.
// --------------------------------------------------------
void Match::displayDebugInfo( )
{
	// Get the global font style from the engine
	DirectX::Font* font = &DirectX::EngineManager::instance( )->getWindowStyle( )->font; 

	// Compose move history string
	std::wstringstream strStream; strStream.str(L" ");
	for( int i = 0; i < m_currentPly; i++ ) { 
			strStream << L"( ";
			strStream << L"P:"  << m_moveHistory[i].pieceNumber;
			strStream << L" X:" << m_moveHistory[i].gridX;
			strStream << L" Y:" << m_moveHistory[i].gridY; 
			strStream << L" F:" << m_moveHistory[i].flipped;
			strStream << L" R:" << m_moveHistory[i].rotated; 
			strStream << L" )";
			strStream << "\n"; }

	// Render move history string 
	font->drawText( m_manager->getDisplayWidth( )-200, 0, strStream.str( ).c_str( ), COLOR::WHITE );

	// Render grid internals
	for( int i = 0; i < m_boardSize; i++ )
	for( int j = 0; j < m_boardSize; j++ )
	{
		std::wstring str;
		if( m_board[i][j] == GRID_COVER_BLUE   ) str = std::wstring( L"B" );
		if( m_board[i][j] == GRID_COVER_RED    ) str = std::wstring( L"R" );
		if( m_board[i][j] == GRID_COVER_GREEN  ) str = std::wstring( L"G" );
		if( m_board[i][j] == GRID_COVER_YELLOW ) str = std::wstring( L"Y" );
		if( m_board[i][j] == GRID_COVER_NONE   ) str = std::wstring( L"-" );
		font->drawText( 50+15*i, 200+15*j, str.c_str( ), COLOR::WHITE );
	}

	// Render keys for AI memory maps
	font->drawText( 10, 440, m_memoryMapName[0].c_str( ), COLOR::WHITE );
	font->drawText( 10, 460, m_memoryMapName[1].c_str( ), COLOR::WHITE );
	font->drawText( 10, 480, m_memoryMapName[2].c_str( ), COLOR::WHITE );
	font->drawText( 10, 500, m_memoryMapName[3].c_str( ), COLOR::WHITE );
}
//
// --------------------------------------------------------
//  Display match data information including scores and
//  records along with any status messages.
// --------------------------------------------------------
void Match::displayMatchInfo( )
{
	// String stream for formatting
	std::wstringstream strStream; 

	// Get the global font style from the engine
	DirectX::Font* font = &DirectX::EngineManager::instance( )->getWindowStyle( )->font;

	// Render score value text
	strStream.str(L" "); strStream << L"Scores - B: " << m_score[0] << L" R: " << m_score[1];
	strStream << L"G: " << m_score[3] << L" Y: " << m_score[4];
	font->drawText( 300, 10, strStream.str( ).c_str( ), COLOR::WHITE );

	// Render record value text
	strStream.str(L" "); strStream << L"Record - B: " << m_record[0] << L" R: " << m_record[1];
	strStream << L"G: " << m_record[3] << L" Y: " << m_record[4];
	font->drawText( 300, 25, strStream.str( ).c_str( ), COLOR::WHITE );

	// Render current player
	std::wstring playerToMove;
	if( m_currentPlayer == PLAYER_BLUE   ) playerToMove = std::wstring( L"Next to move: Blue" );
	if( m_currentPlayer == PLAYER_RED    ) playerToMove = std::wstring( L"Next to move: Red" );
	if( m_currentPlayer == PLAYER_GREEN  ) playerToMove = std::wstring( L"Next to move: Green" );
	if( m_currentPlayer == PLAYER_YELLOW ) playerToMove = std::wstring( L"Next to move: Yellow" );
	font->drawText( 300, 40, playerToMove.c_str( ), COLOR::WHITE );

	// Display blinking wait messages for status indication
	if( (m_manager->getRunningTime( ) - (float)(int)m_manager->getRunningTime( )) > 0.25f )
		if( m_matchEnd ) 
			font->drawText( 300, 55, L"Press ENTER to continue", COLOR::WHITE );
		else if( m_waitTurn == WAIT_HALTED && m_agentFilename[m_currentPlayer] != L"" ) 
			font->drawText( 300, 55, L"Press SPACE to continue", COLOR::WHITE );
}
