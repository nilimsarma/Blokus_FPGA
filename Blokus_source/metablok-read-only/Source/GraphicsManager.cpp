/* ===========================================================================

	Project: DirectX Engine - GraphicsManager

	Description:
	 DirectX Engine for loading and managing Direct3D and handling video
	 memory resources. Also creates and manages the application window.

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
#include "GraphicsManager.h"

// Window styles
#define FULLSCREEN_EX ( WS_EX_TOPMOST )
#define FULLSCREEN	  ( WS_POPUP | WS_CLIPSIBLINGS | WS_VISIBLE )
#define WINDOWED_EX   ( NULL )
#define WINDOWED      ( WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_VISIBLE )
#define NO_BORDER_EX  ( NULL )
#define NO_BORDER	  ( WS_BORDER | WS_CLIPSIBLINGS | WS_VISIBLE )

// --------------------------------------------------------
//  Windows message callback for device window.
// --------------------------------------------------------
namespace {
	long CALLBACK winMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		// Acquire singleton manager handle
		DirectX::Manager* manager = DirectX::Manager::instance( );

		// Forward message to DirectX Manager
		return manager->messagePump( hWnd, uMsg, wParam, lParam );
	}
}
//
// --------------------------------------------------------
// Initialize settings variables to default on contruction.
// --------------------------------------------------------
DirectX::Manager::Manager( ) 
{ 
	// Default settings
	fitBufferToWindow = false;
	pauseAppWhenDeviceLost = true;
	altEnterToToggleFullscreen = true;
	pauseToToggleTimePause = false;
	closeButtonToQuit = true;
	escapeToQuit = true;

	// Nullify pointers
	mousedOverControl = NULL;
	focusedControl = NULL;
	d3dsprite = NULL;
	d3dtext = NULL;
	d3dhinstance = NULL;
	d3dhwnd = NULL;
	d3d = NULL;
	d3ddev = NULL;

	// Input array initialization
	ZeroMemory( keyLock, sizeof( BOOL ) * 256 );
	ZeroMemory( keyboardState, 25 );
	ZeroMemory( pressedKeys, 256 );

	// Display information struct
	ZeroMemory( &d3ddisplay, sizeof(d3ddisplay) );
	d3ddisplay.Format = D3DFMT_X8R8G8B8;
	d3ddisplay.RefreshRate = 0;
	d3ddisplay.Height = 600;
	d3ddisplay.Width = 800;

	// Default backdrop color
	backdropClr = D3DCOLOR_XRGB( 0, 40, 100 );

	// Device information struct
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = true;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.BackBufferFormat = d3ddisplay.Format;
	d3dpp.BackBufferHeight = d3ddisplay.Height;
	d3dpp.BackBufferWidth = d3ddisplay.Width;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.Flags = NULL;
	d3dpp.BackBufferCount = 1;

	// GUI buffer
	messages = 0;
}
//
// --------------------------------------------------------
// Ensures all device memory has been released on destroy.
// --------------------------------------------------------
DirectX::Manager::~Manager( ) 
{ }
//
// --------------------------------------------------------
//  Handles device lost operations.
// --------------------------------------------------------
void DirectX::Manager::onDeviceLost( ) 
{
	// Release sprite interface
	d3dsprite->Release( );

	// Call lost method on DirectX resources
	if( !resources.empty( ) )
	for( std::list<Resource*>::iterator i=resources.begin( ); i!=resources.end( ); ++i ) (*i)->onDeviceLost( );

	// Run device reset method for active controllers
	if( !controllers.empty( ) )
	for( std::list<Controller*>::iterator i=controllers.begin( ); i!=controllers.end( ); ++i ) (*i)->onDeviceLost( );
}
//
// --------------------------------------------------------
//  Handles device reset operations.
// --------------------------------------------------------
void DirectX::Manager::onDeviceReset( ) 
{
	// Run device reset method for active controllers
	for( std::list<Controller*>::iterator i=controllers.begin( ); i!=controllers.end( ); ++i ) (*i)->onDeviceReset( );

	// Call reset method on DirectX resources
	for( std::list<Resource*>::iterator i=resources.begin( ); i!=resources.end( ); ++i ) (*i)->onDeviceReset( );

	// Recreate sprite interface
	D3DXCreateSprite( d3ddev, &d3dsprite );

	// Reload render states
	DWORD ambientLevel = D3DCOLOR_XRGB( 192, 192, 192 );	 // Ambient lighting level ( DWORD COLOR )
	d3ddev->SetRenderState( D3DRS_LIGHTING, TRUE );			 // Enable 3D lighting ( TRUE / FALSE )
	d3ddev->SetRenderState( D3DRS_AMBIENT, ambientLevel );	 // Set ambient lighting lvl ( D3DCOLOR_RGBA )
	d3ddev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );   // Enable bk-face culling ( D3DCULL_CCW / D3DCULL_CW / D3DCULL_NONE )
	d3ddev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID ); // Set render fill mode ( D3DFILL_SOLID / D3DFILL_WIREFRAME / D3DFILL_POINT )
	d3ddev->SetRenderState( D3DRS_ZENABLE, TRUE );			 // Enable z-buffering ( TRUE / FALSE )
	d3ddev->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE ); // Allow DirectX to manage normals ( TRUE / FALSE )
}
//
// --------------------------------------------------------
//  Returns true if the device is lost, false otherwise
//  If the device is lost and can be reset, it resets the
//  device, calling the appropriate preprocesses.
// --------------------------------------------------------
bool DirectX::Manager::checkDeviceLost( ) 
{
	// Check if the device is lost
	HRESULT hr; if( FAILED(hr = d3ddev->TestCooperativeLevel( )) )
	{
		// The device has been lost but cannot be reset at this time
		if( hr == D3DERR_DEVICELOST ) return true;

		// The device has been lost but it can be reset at this time
		if( hr == D3DERR_DEVICENOTRESET )
		{					
			// If the device can be restored, the application prepares the 
			// device by destroying all video-memory resources and any 
			// swap chains. 
			onDeviceLost( );

			// Reset is the only method that has an effect when a device 
			// is lost, and is the only method by which an application can 
			// change the device from a lost to an operational state. 
			// Reset will fail unless the application releases all 
			// resources that are allocated in D3DPOOL_DEFAULT, including 
			// those created by the IDirect3DDevice9::CreateRenderTarget 
			// and IDirect3DDevice9::CreateDepthStencilSurface methods.
			hr = d3ddev->Reset( &d3dpp ); if( FAILED( hr ) )
				{ MessageBox( d3dhwnd, L"Call to device reset has failed!", L"DirectX Engine", NULL ); exit(1); }

			// A lost device must also re-create resources (including  
			// video memory resources) after it has been reset.
			onDeviceReset( ); return false;
		}
	}
 
	return false;
}
//
// --------------------------------------------------------
//  Windows message callback function for device window.
// --------------------------------------------------------
long DirectX::Manager::messagePump( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Consolidate the keyboard messages and pass them to engine controllers
	if( uMsg == WM_KEYDOWN ||
		uMsg == WM_SYSKEYDOWN ||
		uMsg == WM_KEYUP ||
		uMsg == WM_SYSKEYUP )
	{
		bool bKeyDown = ( uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN );
		DWORD dwMask = ( 1 << 29 );
		bool bAltDown = ( ( lParam & dwMask ) != 0 );
		int key = MapVirtualKey( ( BYTE )( wParam & 0xFF ), MAPVK_VK_TO_VSC );
		pressedKeys[ key ] = bKeyDown;

		// Pass the message to the focused window 
		if( focusedControl ) focusedControl->onKeyboardEvent( uMsg, wParam, lParam );

		// Run keyboard msg method for active controllers
		for( std::list<Controller*>::iterator i = controllers.begin( ); 
			i != controllers.end( ); ++i ) (*i)->onKeyboardEvent( uMsg, wParam, lParam );

		return 0;
	}

	// Consolidate the mouse button messages
	if( uMsg == WM_LBUTTONDOWN ||
		uMsg == WM_LBUTTONUP ||
		uMsg == WM_LBUTTONDBLCLK ||
		uMsg == WM_MBUTTONDOWN ||
		uMsg == WM_MBUTTONUP ||
		uMsg == WM_MBUTTONDBLCLK ||
		uMsg == WM_RBUTTONDOWN ||
		uMsg == WM_RBUTTONUP ||
		uMsg == WM_RBUTTONDBLCLK ||
		uMsg == WM_XBUTTONDOWN ||
		uMsg == WM_XBUTTONUP ||
		uMsg == WM_XBUTTONDBLCLK ||
		uMsg == WM_MOUSEWHEEL ||
		uMsg == WM_MOUSEMOVE )
	{
		mouseMove = mousePos;
		mousePos.x = ( short )LOWORD( lParam );
		mousePos.y = ( short )HIWORD( lParam );

		if( uMsg == WM_MOUSEWHEEL ) ScreenToClient( d3dhwnd, &mousePos );

		int nMouseWheelDelta = 0;
		if( uMsg == WM_MOUSEWHEEL )
			nMouseWheelDelta = ( short )HIWORD( wParam );

		int nMouseButtonState = LOWORD( wParam );
		bool bLeftButton   = ( ( nMouseButtonState & MK_LBUTTON ) != 0 );
		bool bRightButton  = ( ( nMouseButtonState & MK_RBUTTON ) != 0 );
		bool bMiddleButton = ( ( nMouseButtonState & MK_MBUTTON ) != 0 );
		bool bSideButton1  = ( ( nMouseButtonState & MK_XBUTTON1 ) != 0 );
		bool bSideButton2  = ( ( nMouseButtonState & MK_XBUTTON2 ) != 0 );

		mouseButtons[0] = bLeftButton;
		mouseButtons[1] = bMiddleButton;
		mouseButtons[2] = bRightButton;
		mouseButtons[3] = bSideButton1;
		mouseButtons[4] = bSideButton2;

		// Acquire the new moused over control
		if( uMsg == WM_MOUSEMOVE )
		{
			// Search active controls for topmost moused over control
			GUI::DisplayPanel* updateMoused = NULL; int mousedDepth = INT_MAX;
			std::list<GUI::DisplayPanel*>::iterator i = controls.begin( );
			while( i != controls.end( ) && !updateMoused ) {
				if( (*i)->contains(&mousePos) ) updateMoused = (*i); i++; }

			// Check for moused over change
			if( updateMoused != mousedOverControl ) {
				if( mousedOverControl ) mousedOverControl->onMouseLeave( );
				if( updateMoused ) updateMoused->onMouseEnter( );
				mousedOverControl = updateMoused; }
		}

		// Check if the mouse is over the focused control
		bool overFocused = (mousedOverControl == focusedControl);

		// Acquire the new focused control
		if( (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN) && !overFocused ) {	
			if( focusedControl ) focusedControl->onLoseFocus( );
			if( mousedOverControl ) mousedOverControl->onGainFocus( );
			focusedControl = mousedOverControl; overFocused = true; }
	
		// Pass the message to the relevant controls
		if( focusedControl ) focusedControl->onMouseEvent( uMsg, wParam, lParam );
		if( mousedOverControl && !overFocused ) 
			mousedOverControl->onMouseEvent( uMsg, wParam, lParam );

		// Run mouse msg method for active controllers
		for( std::list<Controller*>::iterator i=controllers.begin( ); i!=controllers.end( ); ++i ) 
			(*i)->onMouseEvent( uMsg, wParam, lParam );

		// Pass buffered GUI messages to active controllers
		while( (--messages) >= 0 )
		{
			// Pass the message to all active controllers
			for( std::list<Controller*>::iterator i = controllers.begin( );
				i != controllers.end( ); i++ ) 
			{
				(*i)->onGuiEvent( guiMsgBuffer[messages].control, 
				guiMsgBuffer[messages].cmd, guiMsgBuffer[messages].data ); 
			}
		}
		messages = 0;

		return 0;
	}

	// Pass window messsage to active controllers
	for( std::list<Controller*>::iterator i = controllers.begin( ); 
		i != controllers.end( ); ++i ) (*i)->onMessageEvent( uMsg, wParam, lParam );

	// Window operations
	PAINTSTRUCT ps; HDC hdc;
	switch( uMsg )
	{
		// Render on window paint
		case WM_PAINT:
			hdc = BeginPaint( hWnd, &ps );
			if(presentOnWindowPaint) 
				DirectX::Manager::renderFrame( );
			EndPaint( hWnd, &ps );
			break;

		// On window close
		case WM_DESTROY: 
			if( closeButtonToQuit ) 
			{ PostQuitMessage(0); exit(0); } 
			break;

		// Block automatic resizing
		case WM_GETMINMAXINFO: {
			MINMAXINFO* infos = (MINMAXINFO*)lParam;
			infos->ptMaxTrackSize.x = 2000;
			infos->ptMaxTrackSize.y = 2000;
			return DefWindowProc( hWnd, uMsg, wParam, lParam ); }
			
		// Default window process
		default: return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}

	return 0;
}
//
// --------------------------------------------------------
//	Creates a window for the device using the current 
//  window display settings. :TODO: Fix window size.
// --------------------------------------------------------
void DirectX::Manager::createWindow( HINSTANCE hInstance, WCHAR* strWindowTitle, 
									HICON hIcon, HMENU hMenu, int x, int y )
{	
	// Register new window class
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof( WNDCLASSEX );
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= &winMsg;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon( wcex.hInstance, MAKEINTRESOURCE(IDI_LARGE) );
	wcex.hCursor		= LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= L"DirectX Engine";
	wcex.hIconSm		= LoadIcon( wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL) );
	RegisterClassEx( &wcex );

	// Create window 
	d3dhinstance = hInstance;
	unsigned long style = isWindowed( ) ? WINDOWED : FULLSCREEN;
	unsigned long styleEx = isWindowed( ) ? WINDOWED_EX : FULLSCREEN_EX;
	d3dhwnd = CreateWindowExW( styleEx, L"DirectX Engine", L"MetaBlok", style, 0, 0, 
		d3ddisplay.Width, d3ddisplay.Height, NULL, NULL, hInstance, NULL );
	ShowWindow( d3dhwnd, SW_NORMAL ); UpdateWindow( d3dhwnd );

	// Set as the device window
	d3dpp.hDeviceWindow = d3dhwnd;
}
//
// --------------------------------------------------------
//	Creates the direct3D device using the current settings.
// --------------------------------------------------------
bool DirectX::Manager::createDevice( )
{	
	// Create the device
	d3d = Direct3DCreate9( D3D_SDK_VERSION );

	// Set display presentation params
	if ( d3dpp.Windowed ) {
		if ( fitBufferToWindow ) {
			d3dpp.BackBufferHeight = 0;
			d3dpp.BackBufferWidth = 0;
			d3dpp.BackBufferFormat = d3ddisplay.Format;
			d3dpp.FullScreen_RefreshRateInHz = 0;
		} else {
			d3dpp.BackBufferHeight = getWindowHeight( );
			d3dpp.BackBufferWidth = getWindowWidth( );
			d3dpp.BackBufferFormat = d3ddisplay.Format;
			d3dpp.FullScreen_RefreshRateInHz = 0; }
	} else {
		d3dpp.BackBufferHeight = d3ddisplay.Height;
		d3dpp.BackBufferWidth = d3ddisplay.Width;
		d3dpp.BackBufferFormat = d3ddisplay.Format;
		d3dpp.FullScreen_RefreshRateInHz = d3ddisplay.RefreshRate; }

	// Check for hardware vertex processing available
	int vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	D3DCAPS9 caps; d3d->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ); 
	if ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	// Run device acceptable method for active controllers
	for( std::list<Controller*>::iterator i=controllers.begin( ); i!=controllers.end( ); ++i ) 
		if( !(*i)->onDeviceAcceptable( &caps, d3ddisplay.Format, d3dpp.BackBufferFormat ) )
			{ MessageBox( d3dhwnd, L"Device Settings Rejected!", L"DirectX Engine", NULL ); exit(1); }

	// Create D3D device class 
	HRESULT hr = d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dhwnd, vp, &d3dpp, &d3ddev );

	// Display error message on failure
	if( FAILED(hr) ) { MessageBox( d3dhwnd, L"Failed to Create Device!", L"DirectX Engine", NULL ); exit(1);}

	// Set default render states
	d3ddev->SetRenderState( D3DRS_LIGHTING, TRUE );
	d3ddev->SetRenderState( D3DRS_ZENABLE, TRUE );
	d3ddev->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB( 192, 192, 192 ) );
	d3ddev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	d3ddev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	d3ddev->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE );
	d3ddev->SetRenderState( D3DRS_FOGCOLOR, D3DCOLOR_XRGB( 50, 50, 50 ) );
	d3ddev->SetRenderState( D3DRS_FOGENABLE, FALSE );
	d3ddev->SetRenderState( D3DRS_FOGDENSITY, 1 );

	// Run device creation method for active controllers
	for( std::list<Controller*>::iterator i=controllers.begin( ); i!=controllers.end( ); ++i ) (*i)->onDeviceCreated( );

	// Create sprite interface
	D3DXCreateSprite( d3ddev, &d3dsprite );

	// Begin timer
	timer.start( );

	// Return
	return true;
}
//
// --------------------------------------------------------
//	Changes the device's fullscreen display mode.
// --------------------------------------------------------
void DirectX::Manager::setDisplayMode( int width, int height, int depth, int syncRate )
{
	// Display format
	if( depth == 16 ) d3ddisplay.Format = D3DFMT_X4R4G4B4;
	else d3ddisplay.Format = D3DFMT_X8R8G8B8;

	// Update display settings
	d3ddisplay.RefreshRate = syncRate;
	d3ddisplay.Height = height;
	d3ddisplay.Width = width;
}
//
// --------------------------------------------------------
//	Deletes the current device window.
// --------------------------------------------------------
void DirectX::Manager::deleteWindow( )
{ }
//
// --------------------------------------------------------
//	Inserts a new GUI display panel to manager.
// --------------------------------------------------------
void DirectX::Manager::addDisplayPanel( GUI::DisplayPanel* control )
{ 
	std::list<DirectX::GUI::DisplayPanel*>::iterator i = controls.begin( );
	while( i != controls.end( ) && control->getDepth( ) >= (*i)->getDepth( ) ) i++;
	controls.insert( i, control );
}
//
// --------------------------------------------------------
//	Deletes the specified control and checks for any 
//  handles to the control within the manager.
// --------------------------------------------------------
void DirectX::Manager::deleteDisplayPanel( GUI::DisplayPanel *control )
{ 
	if( controls.empty( ) ) return;
	controls.remove( control ); 
	if( mousedOverControl == control ) mousedOverControl = NULL;
	if( focusedControl == control ) focusedControl = NULL;
}
//
// --------------------------------------------------------
//	Forces a resets of the Direct3D device and the window.
// --------------------------------------------------------
void DirectX::Manager::reset( )
{
	// Set and update window style if the mode has changed
	unsigned long newStyle = isWindowed( ) ? WINDOWED : FULLSCREEN;
	unsigned long newStyleEx = isWindowed( ) ? WINDOWED_EX : FULLSCREEN_EX;
	unsigned long oldStyle = SetWindowLongPtr( d3dhwnd, GWL_STYLE, newStyle );
	unsigned long oldStyleEx = SetWindowLongPtr( d3dhwnd, GWL_EXSTYLE, newStyleEx );
	SetWindowPos( d3dhwnd, HWND_NOTOPMOST, 0, 0, d3ddisplay.Width, 
		d3ddisplay.Height, SWP_NOMOVE );
	
	// Update window size constraints
	RECT window; GetWindowRect( d3dhwnd, &window );
	RECT buffer; GetClientRect( d3dhwnd, &buffer );
	int sizex = d3ddisplay.Width  + (window.right - window.left) - buffer.right;
	int sizey = d3ddisplay.Height + (window.bottom - window.top) - buffer.bottom;
	SetWindowPos( d3dhwnd, HWND_NOTOPMOST, 0, 0, sizex, sizey, SWP_NOMOVE );

	// Update display params
	if ( d3dpp.Windowed ) d3dpp.FullScreen_RefreshRateInHz = 0;
	else d3dpp.FullScreen_RefreshRateInHz = d3ddisplay.RefreshRate;
	d3dpp.BackBufferHeight = d3ddisplay.Height;
	d3dpp.BackBufferWidth = d3ddisplay.Width;
	d3dpp.BackBufferFormat = d3ddisplay.Format;

	// Reset the DirectX device and perform loss/reset processes
	onDeviceLost( ); d3ddev->Reset( &d3dpp ); onDeviceReset( );
}
//
// --------------------------------------------------------
//	Renders the scene for each of the active cameras and
//	then swaps the display buffers and updates the screen.
//  Also renders GUI controls belonging to a visible 
//  display panel.
// --------------------------------------------------------
bool DirectX::Manager::renderFrame( )
{
	// Check if the device is lost before
	// attempting to render to the buffer
	if( checkDeviceLost( ) ) return false;

	// Clear the screen and z-buffer using the current backdrop color
	d3ddev->Clear( 0, NULL, D3DCLEAR_TARGET,  backdropClr, 1.0f, 0 );
	d3ddev->Clear( 0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

	// Get new timer data
	timer.update( );

	// Begin new scene
	d3ddev->BeginScene( );

	// Render to nullCam for applications which do not utilized the internal camera system
	if( cameras.empty( ) ) for( std::list<Controller*>::iterator i = controllers.begin( ); 
		i != controllers.end( ); ++i ) (*i)->onRenderFrame( NULL );

	// Otherwise render the 3D scene for each camera
	else for( std::list<Camera*>::iterator i=cameras.begin( ); i!=cameras.end( ); ++i )
	{		
		// Dereference to cam ptr
		Camera* camera = *i;

		// Set view and projection transforms
		d3ddev->SetTransform( D3DTS_VIEW,		camera->getViewMatrix( ) );
		d3ddev->SetTransform( D3DTS_PROJECTION, camera->getProjMatrix( ) );

		// Set the device viewport
		d3ddev->SetViewport( camera->getViewport( ) );

		// Call render method on renderable objects
		for( std::list<Renderable*>::iterator i=renderables.begin( ); i!=renderables.end( ); ++i ) (*i)->render( );

		// Run render frame method for active controllers
		for( std::list<Controller*>::iterator i=controllers.begin( ); i!=controllers.end( ); ++i ) (*i)->onRenderFrame( camera );
	}

	// Render GUI elements
	if( !controls.empty( ) )
	{
		// Disable zbuffer for sprite render
		d3ddev->SetRenderState( D3DRS_ZENABLE, false );	

		// Setup render states for 2d rendering
		d3dsprite->Begin( D3DXSPRITE_ALPHABLEND );
		
		// No offset specifier
		POINT pos = { 0, 0 };

		// Render objects according to depth ordering. Recently inserted objects of same depth
		// will be rendered after more recently updated objects in the case of collision.
		for( std::list<GUI::DisplayPanel*>::iterator i = controls.begin( ); 
			i != controls.end( ); i++ ) (*i)->render( d3dsprite, &pos );
		
		// Restore render states
		d3dsprite->End( );
		
		// Re-enable the zbuffer for next frame
		d3ddev->SetRenderState( D3DRS_ZENABLE, true );	
	}

	// End scene render
	d3ddev->EndScene( );

	// Update the screen
	d3ddev->Present( NULL, NULL, NULL, NULL );

	// Return success
	return true;
}
//
// --------------------------------------------------------
//	Releases the Direct3D device and video resources.
// --------------------------------------------------------
void DirectX::Manager::release( )
{
	// Run device destroyed method for active controllers
	for( std::list<Controller*>::iterator i = controllers.begin( ); 
		i != controllers.end( ); ++i ) (*i)->onDeviceDestroyed( );

	// Call release method on leftover DirectX resources
	while( !resources.empty( ) ) resources.front( )->release( );

	// Release D3D device
	if( d3d ) d3d->Release( );
	if( d3dsprite ) d3dsprite->Release( );
	if( d3ddev ) d3ddev->Release( );
	
	// Stop the timer
	timer.stop( );
}