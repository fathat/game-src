#include "GuiHandler.h"
#include "App.h"
#include "..\shared\network\messageparser.h"

#include "..\shared\defs\defs.h"

//This handles all the interface messages generated
//by the game's gui. We could specialize it into multiple
//small functions instead of one large one (for instance,
//a procedure for each window), but this is easier since it's
//so simple at the moment.
UINT RootMessageProcedure( Event* m )
{
	
	Game->HandleGuiMessage( m );

	return 0;
}



bool GameGUI::InitGUI( Screen3D& Screen, WorldManager& WM )
{
	GM.Init(Screen, &Screen.FM, &WM);
	GM.SetState(true);

	ScreenPtr = &Screen;


	InitMenu();

	//InitConnect();
	//InitConversationDlg();

	State = GM_OPEN;

	GuiWindow* Root = GM.GetRootWindow();
	Root->SetMessageHandler( RootMessageProcedure );

	return true;

}

void GameGUI::InitMenu()
{
	
//Create the bottom console thing and add it to the root window.
//Also, set up the RootMessageProcedure to handle all the basic
//root messages
	GameConsole = new InputBox;
	GameConsole->Init( "IDCONSOLE" );
	GM.AddWindow(GameConsole, 0, true);
	GameConsole->OnMove(25, 50, ScreenPtr->GetWidth()-50, ScreenPtr->GetHeight() - 100);
			    	
// Create everything for the menu to the side
	MenuDlg.Init( "IDMENUDIALOG" );
	WindowHandle MenuDialogH = GM.AddWindow(&MenuDlg, 0, false);
	MenuDlg.OnMove(0, 0, ScreenPtr->GetWidth(), 40);
	MenuDlg.SetText( std::string( "O_O") );

	//create the resume button in the connect dialog
	Button* ResumeButton = new Button;
	ResumeButton->Init( "IDRESUME" );
	GM.AddWindow(ResumeButton, MenuDialogH , true);
	ResumeButton->OnMove(10, 15, 90, 20);
	ResumeButton->SetText( "Resume" );

	//create the settings button
	Button* SettingsButton = new Button;
	SettingsButton->Init( "IDSETTINGS" );
	GM.AddWindow(SettingsButton, MenuDialogH , true);
	SettingsButton->OnMove(110, 15, 90, 20);
	SettingsButton->SetText( "Settings" );
	
	//create the mission button
	/*Button* MissionButton = new Button;
	MissionButton->Init( "IDMISSION" );
	GM.AddWindow(MissionButton, MenuDialogH , true);
	MissionButton->OnMove(210, 15, 90, 20);
	MissionButton->SetText( "Mission" );*/

	//create the quit button
	Button* QuitButton = new Button;
	QuitButton->Init( "IDQUIT" );
	GM.AddWindow(QuitButton, MenuDialogH , true);
	QuitButton->OnMove(210, 15, 90, 20);
	QuitButton->SetText( "Quit" );
}

void GameGUI::InitSettings()
{
}

void GameGUI::InitMission()
{	
}

void GameGUI::InitConnect()
{
	ConnectDlg.Init( "IDCONNECTDIALOG" );
	WindowHandle ConnectDlgH = GM.AddWindow(&ConnectDlg, 0, false);
	ConnectDlg.OnMove(10, ScreenPtr->GetHeight()/4+190, 280, 70);

	//create the connect button
	Button* ConnectButton = new Button;
	ConnectButton->Init( "IDCONNECT" );
	GM.AddWindow(ConnectButton, ConnectDlgH , true);
	ConnectButton->OnMove(210, 5, 70, 20);
	ConnectButton->SetText( "Connect" );

	//create the connect button
	Button* DisconnectButton = new Button;
	DisconnectButton->Init( "IDDISCONNECT" );
	GM.AddWindow(DisconnectButton, ConnectDlgH , true);
	DisconnectButton->OnMove(210, 27, 70, 20);
	DisconnectButton->SetText( "Disconnect" );


	//create the username box
	InputBox* UserName = new InputBox;
	UserName->Init( "IDUSERNAME" );
	GM.AddWindow(UserName, ConnectDlgH, true);
	UserName->OnMove(10, 10, 180, 20);
	UserName->SetScrollOnEnter(false);

	//create the server box
	InputBox* ServerName = new InputBox;
	ServerName->Init( "IDSERVERNAME" );
	GM.AddWindow(ServerName, ConnectDlgH, true);
	ServerName->OnMove(10, 40, 180, 20);
	ServerName->SetScrollOnEnter(false);

	
}

void GameGUI::InitConversationDlg()
{
	// Create the conversation response dialog
	ConversationDlg.Init( "IDCONVERSATIONDIALOG" );
	WindowHandle ConversationDlgH = GM.AddWindow(&ConversationDlg, 0, false);
	ConversationDlg.OnMove(ScreenPtr->GetWidth() - 200, (ScreenPtr->GetHeight()-ScreenPtr->GetHeight()/10)-60, 180, 60);

	//create the first choice button
	Button* FirstButton = new Button;
	FirstButton->Init( "IDFIRSTCHOICE" );
	GM.AddWindow(FirstButton, ConversationDlgH , true);
	FirstButton->OnMove(0, 0, 180, 20);
	FirstButton->SetText( "(null)" );

	//create the second choice button
	Button* SecondButton = new Button;
	SecondButton->Init( "IDSECONDCHOICE" );
	GM.AddWindow(SecondButton, ConversationDlgH , true);
	SecondButton->OnMove(0, 21, 180, 20);
	SecondButton->SetText( "(null)" );
	
	//create the third choice button (!)
	Button* ThirdButton = new Button;
	ThirdButton->Init( "IDTHIRDCHOICE" );
	GM.AddWindow(ThirdButton, ConversationDlgH , true);
	ThirdButton->OnMove(0, 42, 180, 20);
	ThirdButton->SetText( "(null)" );

	ConversationDlg.Show( false );
}

void GameGUI::InitTelnetConnect()
{
}

void GameGUI::ToggleMenu( ) {
	if(State != GM_OPEN )
	{
		State = GM_OPEN;
		MenuDlg.Show(true);
		MissionDlg.Show(true);
		ConnectDlg.Show(true);
		SettingsDlg.Show(true);
		TelnetConnectDlg.Show(true);
		GameConsole->Show(true);
		GM.SetState(true);
		
	}
	else
	{
		State = GM_NONE;
		MenuDlg.Show(false);
		MissionDlg.Show(false);
		ConnectDlg.Show(false);
		SettingsDlg.Show(false);
		TelnetConnectDlg.Show(false);

		
		//GameConsole->OnMove( 30, ScreenPtr->GetHeight()-100, ScreenPtr->GetWidth()/2-100, 80);
		GameConsole->Show(false);
		GM.SetState(false);
	}
}

void GameGUI::ToggleChat( ) {
	if(State != GM_CHAT )
	{
		State = GM_CHAT;
		GM.SetState(true);
	}
	else
	{
		State = GM_NONE;
		GM.SetState(false);

	}		
		
	MenuDlg.Show(false);
	MissionDlg.Show(false);
	ConnectDlg.Show(false);
	SettingsDlg.Show(false);
	TelnetConnectDlg.Show(false);
	GameConsole->OnMove( 30, ScreenPtr->GetHeight()-100, ScreenPtr->GetWidth()/2-30, 80);

}
