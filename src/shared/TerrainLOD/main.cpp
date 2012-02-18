// I N C L U D E S  //////////////////////////////////////////////////////

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <process.h>
#include <io.h>
#include <iostream>

#include "..\s3dlib\screen.h"
#include "..\s3dlib\camera.h"
#include "..\s3dlib\game timer.h"
#include "..\mesh\mesh.h"
#include "..\ini\ini.h"
#include "..\defs\defs.h"
#include "terrainpatch.h"
#include "terrainmap.h"

using namespace std;

// G L O B A L S /////////////////////////////////////////////////////////

HINSTANCE g_hInstance;
HWND	  g_hWnd;
BOOL    AppActive=FALSE;

float g_LOD=0.0f;

Screen3D Screen;
GameTimer timer;
TerrainMap* terrain=NULL;
TerrainPatch * patchy=NULL;
Camera viewer;
Position cameraLocation;


// F U N C T I O N S /////////////////////////////////////////////////////
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);



//Windows Procedure
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{	

	HDC         hdc ;
	PAINTSTRUCT ps ;
	static HINSTANCE hInstance;


	switch (iMsg)
	{

		case WM_ACTIVATE:
			if (WA_INACTIVE == wParam)
			{
				AppActive  = FALSE;
				//Game->InputSystem.Unaquire();
			}
			else
			{
				AppActive  = TRUE;
				//Game->InputSystem.Reaquire();
			}


			
		break;

		//Send a Mousedown message to the gui.
		case WM_LBUTTONDOWN:

			//POINT MousePoint;
			//MousePoint.x = LOWORD(lParam);
			//MousePoint.y = HIWORD(lParam);
			

			break;


		//Send input messages to the GUI. This particular
		//message sends characters to the GUI.
		case WM_CHAR:
			

			break;


		//Also send input messages to the GUI, but
		//instead of plain characters it sends other messages.
		//(we don't do this in the main game loop
		//because that doesn't have anything to manage
		//holding down keys and such.) 
		case WM_KEYDOWN:
			
			break;


		//Reinitializes the game if you resize the window.
		case WM_SIZE:
		{
			if(SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam )
			{
				AppActive=false;
				break;
			}

		}
		break;


		//Handles switiching away from the app.
		case WM_ACTIVATEAPP :
			AppActive= (BOOL)wParam;
			break;	

	
		//Redraw the screen. 
		case WM_PAINT :
			hdc = BeginPaint (hwnd, &ps) ;	
			EndPaint (hwnd, &ps) ;
			return 0 ;
        case WM_DESTROY :
			PostQuitMessage (0) ;
			return 0 ;
	}
    return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}

void CheckKeys()
{
	Vector3D moveVector;
	float ForwardDelta=0, SideDelta = 0;
	
	viewer.Pitch = RADIAN( 45 );
	
	if(HIWORD(GetKeyState(VK_LEFT)))
		SideDelta = -5;
	
	if(HIWORD(GetKeyState(VK_RIGHT)))
		SideDelta = 5;
	if(HIWORD(GetKeyState(VK_UP)))
		ForwardDelta = 5;
	if(HIWORD(GetKeyState(VK_DOWN)))
		ForwardDelta = -5;
	if(HIWORD(GetKeyState(VK_SPACE)))
		cameraLocation.y += 10 * (float)timer.GetFrameTime();
	if(HIWORD(GetKeyState(VK_CONTROL)))
		cameraLocation.y -= 10 * (float)timer.GetFrameTime();

	if(HIWORD(GetKeyState(VK_HOME)))
		g_LOD += 1.0f * (float)timer.GetFrameTime();
	if(HIWORD(GetKeyState(VK_END)))
		g_LOD -= 1.0f * (float)timer.GetFrameTime();

	ForwardDelta *= (float)timer.GetFrameTime();
	SideDelta	 *= (float)timer.GetFrameTime();
	
	
	//moveVector.x += ( -ForwardDelta ) * (float)(*m_cos)(cameraLocation.Yaw + RADIAN(90));
	moveVector.z += ( ForwardDelta )  * (float)sin(cameraLocation.Yaw + RADIAN(90));
	moveVector.x += ( SideDelta )     * (float)cos(cameraLocation.Yaw );
	//moveVector.z += ( -SideDelta )    * (float)(*m_sin)(cameraLocation.Yaw );

	cameraLocation.MoveBy( moveVector);

}



//WinMain
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR  szCmdLine, int iCmdShow)
{
	///Declarations
    static char szAppName[] = "TerrainLOD++" ;
	WNDCLASSEX  wndclass ;
	MSG         msg ;
	char* SomeString = GetCommandLine();
	g_hInstance = hInstance;

	//Create the window class
	wndclass.cbSize        = sizeof (wndclass) ;
    wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
    wndclass.lpfnWndProc   = (long (__stdcall *)(struct HWND__ *,unsigned int,unsigned int,long)) WndProc;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon (NULL, MAKEINTRESOURCE("IDI_ICON")) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH) ;
    wndclass.lpszMenuName  = NULL ;
    wndclass.lpszClassName = szAppName ;
	wndclass.hIconSm       = LoadIcon (NULL, MAKEINTRESOURCE("IDI_ICON")) ;
    RegisterClassEx (&wndclass) ;

	//Record the values
	int WinWidth  = 1024;
	int WinHeight = 768;
		

	int screenwidth  = GetSystemMetrics( SM_CXSCREEN );
	int screenheight = GetSystemMetrics( SM_CYSCREEN );

		
	g_hWnd = CreateWindow (szAppName, szAppName, 
							WS_OVERLAPPEDWINDOW,
							screenwidth/2-WinWidth/2, screenheight/2-WinHeight/2,
							WinWidth, WinHeight,
							NULL, NULL, hInstance, NULL) ;

	

	ShowWindow (g_hWnd, SW_SHOW) ;
    UpdateWindow (g_hWnd) ;


	DisplayModeInfo info;
	info.Fullscreen = false;
	info.UseHardwareTL = true;
	info.Width  = -1;
	info.Height = -1;
	info.BPP	= 32;
	Screen.Create( g_hWnd, info, 10000 );
	cameraLocation.y = 10;
	cameraLocation.z = -10;
	
	Screen.SetViewport(0, 0, Screen.GetWidth(), Screen.GetHeight());
	viewer.Follow( cameraLocation);
	
	


	HeightGrid hg ( 129 );
	for (int y = 0; y< hg.Size(); y++ )
	{
		for ( int x = 0; x<hg.Size(); x++ )
		{
			hg.SetHeight( x, y, (float)sin(x/8.0f) * (float)cos( y/4.0f ) * 5);
		}
	}
	terrain = new TerrainMap( &Screen, &hg, 17 );
		
	patchy = new TerrainPatch( &Screen, &hg );


	//Start running the message loop
	while(1)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 	
			if (msg.message == WM_QUIT)
				break;	
			
			// translate any accelerator keys
			TranslateMessage(&msg);
			// send the message to the window proc
			DispatchMessage(&msg);
			
		}
		else
		{
			//Main loop here

			timer.CalcFrameTime();

			CheckKeys();

			Font2D* font = Screen.FM.GetFont( "Tahoma", 24 );
			Screen.SetClearColor(0, 0, 128, 255);
			Screen.Clear();
			Screen.Start3D();
			Screen.SetViewTransform( viewer.GetLookAtMatrix(0) , true );
			
			
			Screen.Wireframe(true);

			//terrain->ResetLOD();
			terrain->GetPatchAt(1, 0 )->LOD( g_LOD);
			terrain->Update();
			terrain->DrawAll();

			Screen.Wireframe(false);

			font->Draw( Screen, 0, 0, false, D3DCOLOR_ARGB( 255, 255, 255, 255 ), "FPS: %f" , timer.GetFPS() );
			//patchy->Draw();
			Screen.End3D();
			

			Screen.UpdateScreen();	

			
		}
	} 

	delete patchy;
	delete terrain;

	

	return(1);
}


