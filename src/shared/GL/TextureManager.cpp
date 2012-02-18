#include "TextureManager.h"
#include <string.h>
#include "screen.h"

void TextureManager::Clear()
{
	ScreenPtr = NULL;

	for(int i=0; i<MAX_TEXTURES; i++)
	{
		if(TextureList[i] != NULL)
			delete TextureList[i];

		TextureList[i] = NULL;
	}
}

void TextureManager::Destroy()
{
	Clear();
}

TextureHandle TextureManager::AddTexture(char* Filename, char* AlphaMask)
{

	//create the new texture
	Texture* NewTexture = new Texture;
	if(NewTexture->LoadTexture(  *ScreenPtr, Filename, AlphaMask ) != S3DLIB_OK)
	{
		delete NewTexture;
		return -1;
	}
	

	//loop through and find an empty spot to place it
	for(int i=0; i<MAX_TEXTURES; i++)
	{
		//blank spot?
		if(TextureList[i] == NULL)
		{
			//assign the texture a spot
			TextureList[i] = NewTexture;
			
			//done, return the handle
			return i;
		}

	}

	return -1;
}


void TextureManager::RemoveTexture(TextureHandle ID)
{
	//destroy the texture
	if(TextureList[ID] != NULL)
		delete TextureList[ID];

	TextureList[ID] = NULL;

}

TextureHandle TextureManager::TextureExists(char* TextureName)
{
	//loop through all the textures to see if one
	//with the same name exists
	for(int i=0; i<MAX_TEXTURES; i++)
	{
		//if the texture exists in this spot..
		if(TextureList[i])
		{
			int RVal = strcmpi(TextureName, TextureList[i]->GetFilename());
			
			//did we find a texture with the same name?
			if(RVal == 0)
				return i;
		}
	}


	//nope, didn't find the texture
	return -1;

}
