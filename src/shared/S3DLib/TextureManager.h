///////////////// TextureManager.h ////////////////////////////
#pragma once

// I N C L U D E S ////////////////////////////////////////////
#include <list>
#include <vector>
#include <windows.h>
#include <d3d9.h>



// D E F I N I T I O N S //////////////////////////////////////

//Use this typedef instead of "int" to make it clear
//that it's a handle to a texture.

#ifndef TextureHandle
#define TextureHandle int
#endif

class ScriptAPI;
class RenderTarget;

#define MAX_TEXTURES 512

enum TextureMode { TMNormal,   //just a normal texturemap
				   TMAlphaMask,//Modulates and alpha texture across
				   TMDarkMap,  
				   TMAdd, //adds the texture's values
				   TMModulate2x, //Modulates the colors, then multiplies by 2
				   TMSecondMap, //Just applies this map on top
				   TMDetailMap,   //Detail map, in other words, a map that just adds detail to another map by making it rougher
				   TMGlowMap,	 //adds ambient light to a texture
				   TMGlossMap,	 //describes how much an environment map should show through, basically
				   TMBlendMap,   //blends one texture onto another
				   TMSphereMap
				};


//forward declaration of Texture
class Texture;
class Screen3D;



// C L A S S E S /////////////////////////////////////////////

//////// cTextureManager /////////////////////
//
// Description:
//
// Stores all the textures in a program, and
// is responsible for all cleanup.
//
// Inherits From:
//
// - Nothing
//
// TODO:
//
// - Nothing
//
// Known Bugs:
//
// - none
//
///////////////////////////////////////////////

class TextureManager
{
protected:
	friend class ScriptAPI;
	//List of pointers to textures. I was considering
	//using a vector or list, but this seemed like the
	//most efficient way to do it with handles and such.
	Texture* TextureList[MAX_TEXTURES];

	std::list< RenderTarget* > RenderTargets;

	//pointer to the screen (stored so that every call
	//doesn't require a pointer to the screen).
	Screen3D* ScreenPtr;

	// what the current texture stage is like (used to
	// avoid redundent state changes.
	TextureMode CurrentTextureMode;

	
public:

	//constructor/destructor
	TextureManager () { ZeroMemory(TextureList, MAX_TEXTURES * sizeof(Texture*)); Clear();    }
	~TextureManager() { Destroy(); }

	//Render target management
	void RegisterRenderTarget( RenderTarget* rt ) ;
	void UnregisterRenderTarget( RenderTarget* rt ) ;
	void InvalidateRenderTargets();
	void RestoreRenderTargets();
	void ClearRTList();

	//Initializes the class for use
	void Init ( Screen3D* Screen ) { ScreenPtr = Screen; }

	//Functions for cleanup
	void Clear ( ) ;
	void Destroy ( );


	void RestoreTextures();

	void LimitSize( int texSize );
	void UnlimitTextures();

	//Add a texture to the list
	TextureHandle  AddTexture ( const char* Filename, const char* AlphaMask = NULL, int width=256, int height=256, bool notFile=false, D3DFORMAT Format=D3DFMT_A8R8G8B8 ); //returns an ID for the texture
	void RemoveTexture ( TextureHandle ID ) ; //remove the texture with specified ID

	bool HasAlpha( TextureHandle ID );

	//returns a pointer to the texture with the specified handle, if
	//texture doesn't exist, returns NULL.
	Texture* GetTexture ( TextureHandle ID ) {
		if(ID >= 0 && ID < MAX_TEXTURES)
			return TextureList[ID]; 
		else
			return NULL;
	}

	//texture change functions
	void SetTextureMode( TextureMode TM, int BaseStage=0, bool ForceSet=false);


	void PrintLoadedTexturesToDebug();
	//checks to see if a texture exists
	TextureHandle TextureExists( const char* TextureName);

};