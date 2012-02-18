#include "CommonDialog.h"
#include <string.h>

bool FileDialog::OpenLoad(HWND hParent, char* TitleBar, char* TypeString)
{

	OPENFILENAME ofn;
	char pstrFileName [255];
	strcpy(pstrFileName, "");  //Blank out filename (or else the the default filename will be gibberish)
	
	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.hwndOwner         = hParent ;
	ofn.hInstance         = NULL ;
	ofn.lpstrFilter       = TypeString ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter    = 0 ;
	ofn.nFilterIndex      = 0 ;
	ofn.nMaxFile          = _MAX_PATH ;
	ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
	ofn.lpstrInitialDir   = NULL ;
	ofn.lpstrTitle        = TitleBar ;
	ofn.nFileOffset       = 0 ;
	ofn.nFileExtension    = 0 ;
	ofn.lpstrDefExt       = "bmp" ;
	ofn.lCustData         = 0L ;
	ofn.lpfnHook          = NULL ;
	ofn.lpTemplateName    = NULL ;
	ofn.lpstrFile         = pstrFileName;
	ofn.lpstrFileTitle    = NULL;
	ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT | OFN_EXPLORER ;

	//load file here 
	if( GetOpenFileName (&ofn) )
	{
		strcpy( Filename, ofn.lpstrFile);
		return true;
	}

	return false;
}


bool FileDialog::OpenSave(HWND hParent, char* TitleBar, char* TypeString)
{

	OPENFILENAME ofn;
	char pstrFileName [255];
	strcpy(pstrFileName, "");  //Blank out filename (or else the the default filename will be gibberish)
	
	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.hwndOwner         = hParent ;
	ofn.hInstance         = NULL ;
	ofn.lpstrFilter       = TypeString ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter    = 0 ;
	ofn.nFilterIndex      = 0 ;
	ofn.nMaxFile          = _MAX_PATH ;
	ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
	ofn.lpstrInitialDir   = NULL ;
	ofn.lpstrTitle        = TitleBar ;
	ofn.nFileOffset       = 0 ;
	ofn.nFileExtension    = 0 ;
	ofn.lpstrDefExt       = "bmp" ;
	ofn.lCustData         = 0L ;
	ofn.lpfnHook          = NULL ;
	ofn.lpTemplateName    = NULL ;
	ofn.lpstrFile         = pstrFileName;
	ofn.lpstrFileTitle    = NULL;
	ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;

	//load file here 
	if( GetSaveFileName (&ofn) )
	{
		strcpy( Filename, ofn.lpstrFile);
		return true;
	}

	return false;
	
}
