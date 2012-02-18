#include "ImposterManager.h"
#include "..\mesh\mesh.h"
#include "..\s3dlib\camera.h"

extern CRITICAL_SECTION LoadingCS;





ImposterManager::ImposterManager()
{
	for(int i=0; i<MAX_IMPOSTER_TYPES; i++)
	{
        ImposterTypeList[i] = NULL;
	}
}

ImposterManager::~ImposterManager()
{
	DestroyAll();
}

ImposterHandle ImposterManager::AddImposter( MeshHandle hBaseMesh )
{
	int imposterSize = 64;

	Mesh* m = MMPtr->Get( hBaseMesh );
	if( m->Box.max.y - m->Box.min.y > 100  ) 
		imposterSize = 128;
	if( m->Box.max.y - m->Box.min.y > 200  ) 
		imposterSize = 256;
	for(int i=0; i<MAX_IMPOSTER_TYPES; i++)
	{
        if(ImposterTypeList[i] == NULL) 
		{
			ImposterTypeList[i] = new ImposterType();
			ImposterTypeList[i]->GenerateTextures ( *ScreenPtr, *MMPtr, AngleIncrement, imposterSize, imposterSize, hBaseMesh );
			return i;
		}
	}
	return -1;

}

void ImposterManager::RemoveImposter ( ImposterHandle hImposter )
{
	if( ImposterTypeList[hImposter] != NULL )
		delete ImposterTypeList[hImposter];
	ImposterTypeList[hImposter] = NULL;
}

int ImposterManager::Exists( MeshHandle hImposterMesh )
{
	for(int i=0; i<MAX_IMPOSTER_TYPES; i++)
	{
		if(ImposterTypeList[i])
		{
			if(ImposterTypeList[i]->hBaseMesh == hImposterMesh)
			{
				return i;
			}
		}
	}

	return -1;
}


bool ImposterManager::Init( Screen3D& Screen, MeshManager& MM,  Real angleIncrement )
{
	AngleIncrement = angleIncrement;
	ScreenPtr = &Screen;
	MMPtr     = &MM;
	return true;
}

void ImposterManager::DestroyAll()
{
	for(int i=0; i<MAX_IMPOSTER_TYPES; i++)
	{
		RemoveImposter( i );
	}
}

ImposterHandle ImposterManager::GetImposterHandle( MeshHandle hBaseMesh )
{
	int index = Exists(hBaseMesh );

	if(index > -1 )
		return index;

	return AddImposter( hBaseMesh );
}

ImposterType* ImposterManager::GetImposter( ImposterHandle h )
{
	if( h >=0 && h< MAX_IMPOSTER_TYPES )
		return ImposterTypeList[h];
	
	return NULL;
}

void ImposterManager::DrawAll(Screen3D& Screen, MeshManager& MM)
{
    for(int i=0; i<MAX_IMPOSTER_TYPES; i++)
	{
		if(ImposterTypeList[i])
		{
			ImposterTypeList[i]->DrawStoredVertices( Screen, MM );
		}
	}
}