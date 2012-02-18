// I N C L U D E S ////////////////////////////////////////////
#pragma once

#include <list>
#include <windows.h>


// D E F I N I T I O N S //////////////////////////////////////

//Use this typedef instead of "int" to make it clear
//that it's a handle to a texture.

#ifndef TextureHandle
#define TextureHandle int
#endif

#define MAX_TEXTURES 255

enum TextureType { TNormal,   //just a normal texturemap
				   TUserType, //don't set any texture stage state -- user is going to do so.
				   TDarkMap,  //plain old darkmap (aka lightmap, although darkmap is a more accurate name)
				   TEnvironmentMap, //environment map. Don't use this for multi texturing unless it's the base stage
				   TSpecularMap, //Map for specular lighting
				   TDetailMap,   //Detail map, in other words, a map that just adds detail to another map by making it rougher
				   TGlowMap,	 //adds ambient light to a texture
				   TGlossMap	 //describes how much an environment map should show through, basically
				};


//forward declaration of Texture
class Texture;
class Screen3D;



// C L A S S E S /////////////////////////////////////////////

//////// TextureManager //////////////////////
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

	//List of pointers to textures. I was considering
	//using a vector or list, but this seemed like the
	//most efficient way to do it with handles and such.
	Texture* TextureList[MAX_TEXTURES];

	//pointer to the screen (stored so that every call
	//doesn't require a pointer to the screen).
	Screen3D* ScreenPtr;


	//Pointers to the textures at each texture stage
	//.. you shouldn't need any more than four stages
	Texture* ActiveTexture[4]; 
	
	
public:

	//constructor/destructor
	TextureManager () { ZeroMemory(TextureList, MAX_TEXTURES * sizeof(Texture*)); Clear();    }
	~TextureManager() { Destroy(); }


	//Initializes the class for use
	void Init ( Screen3D* Screen ) { ScreenPtr = Screen; }

	//Functions for cleanup
	void Clear ( ) ;
	void Destroy ( );


	//Add a texture to the list
	TextureHandle  AddTexture ( char* Filename, char* AlphaMask = NULL ); //returns an ID for the texture
	void RemoveTexture ( TextureHandle ID ) ; //remove the texture with specified ID

	//returns a pointer to the texture with the specified handle, if
	//texture doesn't exist, returns NULL.
	Texture* GetTexture ( TextureHandle ID ) {
		if(ID >= 0 && ID < MAX_TEXTURES)
			return TextureList[ID]; 
		else
			return NULL;
	}

	//texture change functions
	void SetTexture( TextureHandle T, int stage );

	//checks to see if a texture exists
	TextureHandle TextureExists(char* TextureName);

};