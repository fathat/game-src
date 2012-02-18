// I N C L U D E S  //////////////////////////////////////////////////////

#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include <stdio.h>
#include <process.h>
#include <io.h>
#include <iostream>

using namespace std;


#include "app.h"
#include "..\shared\s3dlib\screen.h"
#include "..\shared\mesh\mesh.h"
#include "..\shared\gamegui\InputBox.h"
#include "..\shared\ini\ini.h"
#include "SystemObjects.h"

#include "..\shared\defs\defs.h"

// G L O B A L S /////////////////////////////////////////////////////////

HINSTANCE g_hInstance;
HWND    g_hWnd;
bool    g_ResizeEnabled=false;
BOOL    AppActive=FALSE;
bool    LaunchApp=true;

//log events?
BOOL    LogActions = FALSE;

GameApp* Game=NULL;
Screen3D* ScreenPtr;


CRITICAL_SECTION InputCS;
CRITICAL_SECTION MainCS;

// F U N C T I O N S /////////////////////////////////////////////////////
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);


void ScanForLevels( HWND hLevelCombo )
{
	//declarations
	_finddata_t fd;
	std::string SubPath;

		
	//Figure out path to search
	std::string Path = "..\\Maps";
	
	
	
	//Store old dir
	char CurrentDir[255];
	GetCurrentDirectory( 255, CurrentDir );

	//set the directory to search
	SetCurrentDirectory( Path.c_str() );
	

	//Find TEH FIRST FILE!11
	long searchhandle = _findfirst( "*.ini" , &fd);

	if(searchhandle != -1 )
	{
		//add the first file
		std::string InputString;
		if(!(fd.attrib & _A_SUBDIR))
		{
			SubPath = Path;
			SubPath += "\\";
			SubPath += fd.name;
			SendMessage(hLevelCombo, CB_ADDSTRING, 0, (LPARAM)SubPath.c_str());
		}
		
		//add all subsequent files
		bool DoneListing=false;
		while (!DoneListing)
		{
			if( _findnext ( searchhandle, &fd ) != -1)
			{
				if(!(fd.attrib & _A_SUBDIR))
				{
					SubPath = Path;
					SubPath += "\\";
					SubPath += fd.name;
					SendMessage(hLevelCombo, CB_ADDSTRING, 0, (LPARAM)SubPath.c_str());
				}

			}
			else
			{
				DoneListing = true;
			}
		}

		_findclose( searchhandle );
		
	}

	SetCurrentDirectory( CurrentDir );


}


BOOL CALLBACK VideoDlgProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch(iMsg)
	{
		case WM_INITDIALOG:			
		{			
			HWND hDefaultsList = GetDlgItem( hDlg, IDC_DEFAULTS );
			SendMessage( hDefaultsList, CB_ADDSTRING, 0, (LPARAM)"Custom");
			SendMessage( hDefaultsList, CB_ADDSTRING, 1, (LPARAM)"GeForce 6800/Radeon X800");
			SendMessage( hDefaultsList, CB_ADDSTRING, 2, (LPARAM)"Radeon 9700/GF 5700");
			SendMessage( hDefaultsList, CB_ADDSTRING, 3, (LPARAM)"DX8 Card");
			SendMessage( hDefaultsList, CB_SETCURSEL, 0, 0);


		}
		break;


		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_DEFAULT:
				{
					
					HWND hModeList = GetDlgItem( hDlg, IDC_MODELIST ); //list we want to add stuff to
					ScreenPtr = Game->GetScreen( );  //Screen which we're getting this info from
					HWND hDefaultsList = GetDlgItem( hDlg, IDC_DEFAULTS );
					int profile = SendMessage(hDefaultsList, CB_GETCURSEL, 0, 0);

					HWND hWindowed = GetDlgItem( hDlg, IDC_WINDOWED );
												

					if(profile == 0)
					{
						
					}
					else if(profile == 1)
					{
				
					}
					else if(profile == 2)
					{
				
					}					

					break;
				}

			break;
			}
	}

	return FALSE;
}

BOOL CALLBACK PlayerDlgProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	return FALSE;
}

BOOL CALLBACK BenchmarkDlgProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch(iMsg)
	{

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_TEST:
					MessageBox( hDlg, "Testing is not yet implemented", "OHNOS!", MB_OK );
					break;

			break;
			}
	}

	return FALSE;
}


//Procedure for info window
BOOL CALLBACK SettingsDlgProc2 (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//messages
	HWND hTab  = GetDlgItem( hDlg, IDC_TAB );
	static HWND hVideoWindow = NULL, hGameWindow = NULL, hBenchmarkWindow;
	if(hVideoWindow)
	{
		int currenttab = TabCtrl_GetCurSel( hTab);
		if( currenttab == 0)
		{
			ShowWindow(hGameWindow, 0 );
			ShowWindow(hVideoWindow, 1 );
			ShowWindow(hBenchmarkWindow, 0 );
		}
		else if(currenttab == 1 )
		{
			
			ShowWindow(hGameWindow, 1 );
			ShowWindow(hVideoWindow, 0 );
			ShowWindow(hBenchmarkWindow, 0 );
		}
		else if(currenttab == 2 )
		{
			
			ShowWindow(hGameWindow, 0 );
			ShowWindow(hVideoWindow, 0 );
			ShowWindow(hBenchmarkWindow, 1 );
		}

	}


	switch(iMsg)
	{
		case WM_INITDIALOG:			
		{			

			//Move the dialog to the middle of the screen.
			int screenwidth  = GetSystemMetrics( SM_CXSCREEN );
			int screenheight = GetSystemMetrics( SM_CYSCREEN );
			SetWindowPos( hDlg,HWND_TOP,  screenwidth/2-150, screenheight/2-150, 0, 0, SWP_NOSIZE );
			
			
			//Setup the tab
			TCITEM tc;
			

			//video tab
			tc.pszText  = "Video Settings";
			tc.mask     = TCIF_TEXT;
			tc.cchTextMax = strlen("Video Settings");
			tc.iImage   = -1;
			TabCtrl_InsertItem( hTab, 0, &tc );

			//player tab
			tc.pszText  = "Game Settings";
			tc.mask     = TCIF_TEXT;
			tc.cchTextMax = strlen("Game Settings");
			tc.iImage   = -1;
			TabCtrl_InsertItem( hTab, 1, &tc );

			//player tab
			tc.pszText  = "Benchmark";
			tc.mask     = TCIF_TEXT;
			tc.cchTextMax = strlen("Benchmark");
			tc.iImage   = -1;
			TabCtrl_InsertItem( hTab, 2, &tc );
			
	

			//Create the dialog windows for the various tabs
			RECT TabArea;
			GetWindowRect( hTab, &TabArea);
			hVideoWindow = CreateDialog( NULL, "IDD_VIDEO", hDlg, VideoDlgProc );
			SetWindowPos( hVideoWindow, HWND_TOP, 10, 25, 0, 0, SWP_NOSIZE );

			/*hGameWindow = CreateDialog( NULL, "IDD_GAME", hDlg, PlayerDlgProc );
			SetWindowPos( hGameWindow, HWND_TOP, 10, 25, 0, 0, SWP_NOSIZE );
			ShowWindow(hGameWindow, 0 );

			hBenchmarkWindow = CreateDialog( NULL, "IDD_BENCHMARK", hDlg, BenchmarkDlgProc );
			SetWindowPos( hBenchmarkWindow, HWND_TOP, 10, 25, 0, 0, SWP_NOSIZE );
			ShowWindow(hBenchmarkWindow, 0 );*/


			//Fill in the various display modes
			char displayinfo[255]; //holds the info string describing the mode
			HWND hModeList = GetDlgItem( hVideoWindow, IDC_MODELIST ); //list we want to add stuff to
						
			//Add the various modes
			
			//And add it to the list
			SendMessage(hModeList, LB_ADDSTRING, 0, (LPARAM)"512x384");
			SendMessage(hModeList, LB_ADDSTRING, 0, (LPARAM)"640x480");
			SendMessage(hModeList, LB_ADDSTRING, 0, (LPARAM)"800x600");
			SendMessage(hModeList, LB_ADDSTRING, 0, (LPARAM)"1024x768");
			SendMessage(hModeList, LB_ADDSTRING, 0, (LPARAM)"1152x864");
			SendMessage(hModeList, LB_ADDSTRING, 0, (LPARAM)"1280x960");
			SendMessage(hModeList, LB_ADDSTRING, 0, (LPARAM)"1280x1024");
			SendMessage(hModeList, LB_ADDSTRING, 0, (LPARAM)"Desktop Resolution");
			

			HWND hWindowed = GetDlgItem( hVideoWindow, IDC_WINDOWED );
			#ifdef _DEBUG
			SendMessage(hModeList, LB_SETCURSEL,  3,0);
			SendMessage(hWindowed, BM_SETCHECK, BST_CHECKED, 0);
			#else
			SendMessage(hModeList, LB_SETCURSEL, 7,0);
			SendMessage(hWindowed, BM_SETCHECK, BST_UNCHECKED, 0);
			#endif


			//Fill in the level's
			/*HWND hMapEdit    = GetDlgItem( hGameWindow, IDC_LEVELCOMBO );
			ScanForLevels(hMapEdit);
			SetWindowText(hMapEdit, "..\\maps\\snowisland.ini");*/
			
			
		}
		break;


		case WM_COMMAND:
		
	switch(LOWORD(wParam))
			{
				case IDOK:
				{
					//Set the video variables
					HWND hModeList = GetDlgItem( hVideoWindow, IDC_MODELIST );
					int mode = SendMessage(hModeList, LB_GETCURSEL, 0, 0);

				
					if( mode == 0)
					{
						DeviceManager::DisplayMode.Width  = 512;
						DeviceManager::DisplayMode.Height = 384;
					}
					else if( mode == 1)
					{
						DeviceManager::DisplayMode.Width  = 640;
						DeviceManager::DisplayMode.Height = 480;
					}
					else if( mode == 2)
					{
						DeviceManager::DisplayMode.Width  = 800;
						DeviceManager::DisplayMode.Height = 600;
					}
					else if( mode == 3)
					{
						DeviceManager::DisplayMode.Width  = 1024;
						DeviceManager::DisplayMode.Height = 768;
					}
					else if( mode == 4)
					{
						DeviceManager::DisplayMode.Width  = 1152;
						DeviceManager::DisplayMode.Height = 864;
					}
					else if( mode == 5)
					{
						DeviceManager::DisplayMode.Width  = 1280;
						DeviceManager::DisplayMode.Height = 960;
					}
					else if( mode == 6)
					{
						DeviceManager::DisplayMode.Width  = 1280;
						DeviceManager::DisplayMode.Height = 1024;
					}
					else if( mode == 7)
					{
						DeviceManager::DisplayMode.Width  = GetSystemMetrics( SM_CXSCREEN );
						DeviceManager::DisplayMode.Height = GetSystemMetrics( SM_CYSCREEN );
					}
					
					
					
					HWND hWindowed = GetDlgItem( hVideoWindow, IDC_WINDOWED );
					DeviceManager::WindowedMode = SendMessage(hWindowed, BM_GETCHECK, 0, 0);

					if(DeviceManager::WindowedMode)
						DeviceManager::DisplayMode.Fullscreen = false;
					else
						DeviceManager::DisplayMode.Fullscreen = true;

					HWND hHardwareTL = GetDlgItem( hVideoWindow, IDC_HARDWARETL );
					DeviceManager::HardwareVS = SendMessage(hHardwareTL, BM_GETCHECK, 0, 0);
					DeviceManager::DisplayMode.UseHardwareTL = DeviceManager::HardwareVS;

					//Set the map variables
					char TempMapBuffer[128];
					HWND hMapEdit    = GetDlgItem( hGameWindow, IDC_LEVELCOMBO );
					GetWindowText( hMapEdit, TempMapBuffer, 255);
					Game->SetMap( TempMapBuffer );

					//Done ^_^
					EndDialog(hDlg, 0);
					break;
				}
				case IDCANCEL:
					LaunchApp = false;
					PostQuitMessage(0);
					EndDialog(hDlg, 0);
					break;

			break;
			}
	}
	return FALSE;

}




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
				Game->InputSystem.Unaquire();
			}
			else
			{
				AppActive  = TRUE;
				Game->InputSystem.Reaquire();
			}


			
		break;

		//Send a Mousedown message to the gui.
		case WM_LBUTTONDOWN:

			POINT MousePoint;
			MousePoint.x = LOWORD(lParam);
			MousePoint.y = HIWORD(lParam);
			
			if( Game )
				Game->MouseDown( MousePoint, 0 );

			break;


		//Send input messages to the GUI. This particular
		//message sends characters to the GUI.
		case WM_CHAR:
			Game->CharMessage( wParam, lParam );

			break;


		//Also send input messages to the GUI, but
		//instead of plain characters it sends other messages.
		//(we don't do this in the main game loop
		//because that doesn't have anything to manage
		//holding down keys and such.) 
		case WM_KEYDOWN:
			if( Game )
				Game->KeyMessage( wParam );
			break;


		//Reinitializes the game if you resize the window.
		case WM_SIZE:
		{
			if(SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam )
			{
				AppActive=false;
				break;
			}
			if(g_ResizeEnabled)
			{
				AppActive=true;
				if(Game)
				{
					Game->InitSystemObjects(hwnd, g_hInstance);
					Game->InitRenderStates();
				}
			}
		}
		break;


		//Handles switiching away from the app.
		case WM_ACTIVATEAPP :
			AppActive= wParam;
			break;	

	
		//Redraw the screen. Note that we have to get the frame time
		//to avoid bugs with the objects (if we don't get it, the objects
		//won't be drawn because they'll already be marked as drawn
		//for that frame.)
		case WM_PAINT :
			hdc = BeginPaint (hwnd, &ps) ;	
			if( Game)
			{
				Game->BeginFrame();
				Game->RedrawScreen();
			}
			EndPaint (hwnd, &ps) ;
			return 0 ;
        case WM_DESTROY :
			PostQuitMessage (0) ;
			return 0 ;
	}
    return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}


VOID InputThread ( PVOID pvoid )
{
	while(true)
	{
		EnterCriticalSection(&InputCS);
		//Game->BeginFrame();
		Game->CheckInput();
		LeaveCriticalSection(&InputCS);
	//	Sleep(1);
	}
}

#ifdef _CONSOLE
int main()
{
	cout << "App started" << endl;
	HINSTANCE hInstance = 0;

#else 

//WinMain
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR  szCmdLine, int iCmdShow)
{

#endif
	//Initialize common controls
	InitCommonControls();


	//Create an instance of the game app
	Game = new GameApp( g_hWnd, hInstance, DeviceManager::Screen, DeviceManager::InputSystem, DeviceManager::Sound  );
	

	///Declarations
    static char szAppName[] = "Game - Alpha 7" ;
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


	//Bring up a dialog box asking for settings
	DialogBox( hInstance, "IDD_SETTINGS2", NULL, SettingsDlgProc2);
	
	if(!LaunchApp)
		return 0;


	//Record the values
	int WinWidth  = DeviceManager::DisplayMode.Width;
	int WinHeight = DeviceManager::DisplayMode.Height;
	int Windowed  = DeviceManager::WindowedMode;

	

	//Create the main window
	if( Windowed )
	{
		int screenwidth  = GetSystemMetrics( SM_CXSCREEN );
		int screenheight = GetSystemMetrics( SM_CYSCREEN );

			
		g_hWnd = CreateWindow (szAppName, "Game - Alpha 7",
							 WS_OVERLAPPEDWINDOW,
							 screenwidth/2-WinWidth/2, screenheight/2-WinHeight/2,
							 WinWidth, WinHeight,
							 NULL, NULL, hInstance, NULL) ;

		DeviceManager::DisplayMode.Width = -1;
		DeviceManager::DisplayMode.Height= -1;

		

		ShowWindow (g_hWnd, SW_SHOW) ;
		ShowCursor(FALSE);


		
	}
	else
	{
		g_hWnd = CreateWindow (szAppName, "Game Alpha 7",
							 WS_POPUP,
							 0, 0,
							 WinWidth, WinHeight,
							 NULL, NULL, hInstance, NULL) ;
		
		ShowWindow (g_hWnd, SW_SHOW) ;
		ShowCursor(FALSE);
	}	
    UpdateWindow (g_hWnd) ;
	

	DeviceManager::InitSystemObjects( g_hWnd, hInstance );	
	
	//Initialize the game! wo0t wo0t!
	
	if(!Game->Init(g_hWnd, g_hInstance))
		return false;
	g_ResizeEnabled = true; 
	bool FirstRun = true;

	InitializeCriticalSection(&InputCS);
	InitializeCriticalSection(&MainCS);

	//_beginthread( InputThread, 0, 0 );

	//SetForegroundWindow(g_hWnd);
	BringWindowToTop(g_hWnd);
	//Start running the message loop
	while(1)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 
		
			if (msg.message == WM_QUIT)
			{
				break;
			}
			
			
			// translate any accelerator keys
			TranslateMessage(&msg);
			// send the message to the window proc
			DispatchMessage(&msg);
			
		}
		else
		{
			//Main loop here
			
			EnterCriticalSection( &MainCS);
			Game->HandleNetworkMessages();
			LeaveCriticalSection(&MainCS);	

			Game->BeginFrame();

			if(AppActive || Game->IsAlwaysActive())
			{


				//Calculate the framerate and such
				EnterCriticalSection( &MainCS);
				
				//We don't run these two functions on the first frame,
				//because the frametime is completly and utterly wacky,
				//so you're launched across half the map if you don't
				//give it a frame to stabalize.
				if(!FirstRun)
				{
					int timesToUpdate = Game->FrameTime/(1.0f/60.0f) + .5f;

					if(timesToUpdate < 1 )
						timesToUpdate = 1;
					else if( timesToUpdate > 4 )
						timesToUpdate = 4;
					timesToUpdate = 1;
				

					
					Game->FrameTime /= timesToUpdate;

					for( int i=0; i<timesToUpdate; i++)
					{
						Game->CheckInput();
						Game->MoveObjects();
					}
					
				}
				LeaveCriticalSection(&MainCS);	
				
				//Game->WM.DilationPercent = 0.01;
				//Game->RedrawScreen();
				//Game->WM.DilationPercent = 0.00;
				Game->RedrawScreen(); 
				
				FirstRun = false;
			}		

			if(Game->GetShareCPU())
				Sleep(1);

		}
	} 

	
	DeleteCriticalSection (&InputCS);

	if(Game)
	{
		Game->Shutdown();
		delete Game;
	}

	//m_dumpMemoryReport();


	return(1);
}


