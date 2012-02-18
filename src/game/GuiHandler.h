// I N C L U D E S ////////////////////////////
#pragma once
#include "..\shared\gamegui\guimanager.h"
#include "..\shared\gamegui\controls.h"
#include "..\shared\gamegui\inputbox.h"


// D E F I N E S //////////////////////////////
UINT RootMessageProcedure( Event* m );

enum GuiState { GM_CHAT, GM_OPEN, GM_NONE };

// C L A S S E S /////////////////////////////
class GameGUI
{
public:
	//gui objects
	GUIManager GM;
	DialogBox ConnectDlg;
	DialogBox TelnetConnectDlg;
	DialogBox MenuDlg;
	DialogBox SettingsDlg;
	DialogBox MissionDlg;
	DialogBox ConversationDlg;
	InputBox* GameConsole;    

	GuiState State;

	Screen3D* ScreenPtr;
	
	
	void InitMenu();
	void InitSettings();
	void InitMission();
	void InitConnect();
	void InitTelnetConnect();
	void InitConversationDlg();
public:

	bool InitGUI( Screen3D& Screen, WorldManager& WM );
	

	void ShowMessage(char* message) { GameConsole->InputLine( message ); }

	void ShowConversationMenu( bool showmenu ) { ConversationDlg.Show( showmenu ); }

	void ToggleMenu( );
	void ToggleChat();

	void UpdateGUI();
    
};