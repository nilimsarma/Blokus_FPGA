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

// Begin definition
#ifndef DX_ENGINE_GRAPHICS_MANAGER_H
#define DX_ENGINE_GRAPHICS_MANAGER_H

// Callback Framework
#include "Controller.h"

// Storage structure for render states
typedef struct D3DRENDER_STATES {
	DWORD ambient;    // Ambient lighting color ( DWORD color )
	DWORD cullMode;   // Face culling ( D3DCULL_CCW / D3DCULL_CW / D3DCULL_NONE )
	DWORD fillMode;   // Render fill mode ( D3DFILL_SOLID / D3DFILL_WIREFRAME / D3DFILL_POINT )
	BOOL normalize;   // Allow DirectX to manage normals ( TRUE / FALSE )
	BOOL fogEnable;   // Enables/disables fog ( TRUE / FALSE )
	DWORD fogColor;   // Sets fog color ( DWORD color )
	DWORD fogDensity; // Sets fog density ( DWORD density )
} D3DRENDER_STATES;

// DirectX Engine Namespace
namespace DirectX
{
	// Management class
	class Manager
	{
	friend GUI::DisplayPanel;

	public:
		// Destructor
		~Manager( );

		// DirectX Object Accessors
		LPDIRECT3D9 getD3D( ) { return d3d; }
		D3DPRESENT_PARAMETERS* getD3DPP( ) { return &d3dpp; }
		LPDIRECT3DDEVICE9 getD3DDev( ) { return d3ddev; }
		D3DRENDER_STATES* getD3DRS( ) { return &d3drs; }
		LPD3DXSPRITE getSprite( ) { return d3dsprite; }

		// Device creation
		void startup( );
		bool createDevice( );

		// Backbuffer / device settings queries
		int getBackbufferWidth( ) { return d3dpp.BackBufferWidth; }
		int getBackbufferHeight( ) { return d3dpp.BackBufferHeight; }

		// Wait for vertical sync
		bool vSyncEnabled( ) { return (d3dpp.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE); }
		void disableVSync( ) { d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; }
		void enableVSync( ) { d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; }

		// Display mode settings
		void setAdapter( unsigned int adapter ) { m_adapter = adapter; }
		unsigned int getAdapter( ) { return m_adapter; }
		void setDisplayMode( DisplayMode mode ) { d3ddisplay = mode; }
		void setDisplayMode( int width, int height, int depth, int syncRate = 60 );
		unsigned int getDisplayWidth( ) { return d3ddisplay.Width; } 
		unsigned int getDisplayHeight( ) { return d3ddisplay.Height; }
		DisplayMode getDisplayMode( ) { return d3ddisplay; }

		// Adapter and display mode enumeration
		unsigned int getAdapterCount( ) { return d3d->GetAdapterCount( ); }
		unsigned int getAdapterDisplayModeCount( unsigned int adapter ) { return d3d->GetAdapterModeCount( adapter, D3DFMT_X8R8G8B8 ); }
		DisplayMode getAdapterDisplayMode( unsigned int adapter, unsigned int mode ) 
							 { D3DDISPLAYMODE dmodes; d3d->EnumAdapterModes( adapter, D3DFMT_X8R8G8B8, mode, &dmodes ); return dmodes; }

		// Timing data
		float getFPS( ) { return timer.getFPS( ); }
		float getElapsedTime( ) { return timer.getElapsedTime( ); }
		float getRunningTime( ) { return timer.getRunningTime( ); }

		// Backdrop color
		void setBackdropColor( unsigned long color ) { backdropClr = color; }
		unsigned long getBackdropColor( ) { return backdropClr; }

		// Device control methods
		void pause( );
		void unpause( );
		void release( );
		void reset( );

		// Render frame method
		bool renderFrame( );

		// Window accessors / modifiers
		void createWindow( HINSTANCE hInstance=0, WCHAR* strWindowTitle=L"", 
			HICON hIcon=0, HMENU hMenu=0, int x=0, int y=0 );
		HWND getWindowHandle( ) { return d3dhwnd; }
		HINSTANCE getInstanceHandle( ) { return d3dhinstance; }
		RECT getWindowRectangle( ) { RECT rc; GetClientRect( d3dhwnd, &rc ); return rc; }
		int getWindowWidth( ) { RECT rc = getWindowRectangle( ); return rc.right - rc.left; }
		int getWindowHeight( ) { RECT rc = getWindowRectangle( ); return rc.bottom - rc.top; }
		void setWindowMode( bool windowed ) { d3dpp.Windowed = windowed; }
		int isWindowed( ) { return d3dpp.Windowed; }
		void deleteWindow( );

		// Accessory option modifiers
		void setHotkeyHandling( bool altEnter, bool escape, bool pause );
		void setBufferResizing( bool fitToWindow );

		// Input Data Accessors
		int getKeyState( int key ) { return pressedKeys[key]; }
		int getMouseMoveX( ) { return mousePos.x - mouseMove.x; }
		int getMouseMoveY( ) { return mousePos.y - mouseMove.y; }
		int getMouseWheelDelta( ) { return mouseWheelDelta; }
		int getMouseX( ) { return mousePos.x; } 
		int getMouseY( ) { return mousePos.y; }
		int isMouseButtonPressed( int button ) 
			{ return mouseButtons[button]; }

		// Pumps GUI events into the internal message buffer for dispatch
		void pumpMessage( GUI::Control* control, int command, void* data )
		{ 
			guiMsgBuffer[messages].control = control;
			guiMsgBuffer[messages].cmd = command;
			guiMsgBuffer[messages].data = data;
			messages++; 
		}

		// GUI interaction detection
		bool isFocusedControl( ) { return focusedControl != NULL; }
		bool isMousedOverControl( ) { return mousedOverControl != NULL; }

		// Resource management - registration
		void addController( Controller *controller ) { controllers.push_front(controller); }
		void addCamera( Camera *camera ) { cameras.push_front(camera); }
		void addRenderable( Renderable *renderable ) { renderables.push_front(renderable); }
		void addResource( Resource *resource ) { resources.push_front(resource); }

		// Resource management - deregistration
		void deleteController( Controller *controller ) { if( !controllers.empty( ) ) controllers.remove(controller); }
		void deleteCamera( Camera *camera ) { if( !cameras.empty( ) ) cameras.remove(camera); }
		void deleteRenderable( Renderable* renderable ) { if( !renderables.empty( ) ) renderables.remove(renderable); }
		void deleteResource( Resource *resource ) { if( !resources.empty( ) ) resources.remove(resource); }

		// Message pump for device window, can also be used for simulated messages
		long messagePump( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		// Singleton accessor	
		static Manager* instance( )
		{
			static Manager gSingleton;
			return &gSingleton;
		}

	private:
		Manager( );

		// DirectX9
		LPDIRECT3D9 d3d;				 // The pointer to Direct3D interface
		LPDIRECT3DDEVICE9 d3ddev;		 // The pointer to the device class
		D3DPRESENT_PARAMETERS d3dpp;	 // Device presentation parameters
		D3DRENDER_STATES d3drs;			 // Device render states
		D3DDISPLAYMODE d3ddisplay;		 // Device display mode
		LPD3DXSPRITE d3dsprite;			 // Sprite render interface 
		LPD3DXSPRITE d3dtext;			 // Text render interface 
		D3DMATERIAL9 d3dmaterial;		 // Default material type
		D3DCAPS9 d3dcaps;				 // D3D caps for current device
		unsigned int m_adapter;			 // Current device adapter

		// Windows
		HINSTANCE d3dhinstance;			  // Window instance
		HWND d3dhwnd;					  // Window handle

		// Peripheral Input Data
		POINT mousePos, mouseMove;
		bool mouseButtons[5];
		int mouseWheelDelta;
		char keyboardState[256];
		BOOL pressedKeys[256];
		BOOL keyLock[256];

		// Managed Objects
		std::list<Controller*> controllers;
		std::list<Resource*> resources;
		std::list<Renderable*> renderables;
		std::list<Camera*> cameras;

		// GUI display panels
		std::list<GUI::DisplayPanel*> controls;
		GUI::DisplayPanel* mousedOverControl;
		GUI::DisplayPanel* focusedControl;

		// Accessory Options
		unsigned long backdropClr;
		bool presentOnWindowPaint;
		bool fitBufferToWindow;
		bool pauseAppWhenDeviceLost;
		bool altEnterToToggleFullscreen;
		bool pauseToToggleTimePause;
		bool closeButtonToQuit;
		bool escapeToQuit;

		// Device Loss Management
		bool checkDeviceLost( ); 
		void onDeviceLost( ); 
		void onDeviceReset( );

		// GUI Display Panel Management
		void deleteDisplayPanel( GUI::DisplayPanel *control );
		void addDisplayPanel( GUI::DisplayPanel *control );
		void updateDisplayPanelDepth( GUI::DisplayPanel *control ) 
			{ controls.remove( control ); addDisplayPanel( control ); }
		void giveFocus( ) { focusedControl->onLoseFocus( ); 
			focusedControl = NULL; }

		// GUI message buffer
		struct GuiEvent { GUI::Control* control; int cmd; void* data; };
		GuiEvent guiMsgBuffer[100]; int messages;

		// Internal High-Res Timer
		DirectX::Timer timer;
	};
}

// End file definition
#endif