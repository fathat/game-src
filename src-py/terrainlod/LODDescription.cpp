#include "TerrainMap.h"
#include "LODDescription.h"
#include "TerrainPatch.h"

void LODDescription::SetDefaults()
{
	westLOD=-1;
	eastLOD=-1;
	southLOD=-1;
	northLOD=-1;
	LOD = -1;
}
LODDescription::LODDescription( TerrainPatch* p )
{
	LoadValuesFromPatch(p );
}

int LODDescription::LODOfPatch( TerrainPatch* p )
{
	if( p != 0) 
		return (int)p->LOD() ;

	return -1;
}

void LODDescription::LoadValuesFromPatch( TerrainPatch* p )
{
	TerrainPatch* e,*w,*n,*s;
	e = p->ParentTerrain()->GetEastNeighborOf( p->Column() , p->Row() );
	w = p->ParentTerrain()->GetWestNeighborOf( p->Column() , p->Row() );
	n = p->ParentTerrain()->GetNorthNeighborOf( p->Column(), p->Row() );
	s = p->ParentTerrain()->GetSouthNeighborOf( p->Column(), p->Row() );


	westLOD = LODOfPatch( w );
	eastLOD = LODOfPatch( e );
	northLOD = LODOfPatch( n );
	southLOD = LODOfPatch( s );

	LOD = (int)p->LOD();
}

bool LODDescription::IsConsistentWithPatch( TerrainPatch* p )
{


	return (westLOD  == (int)LODOfPatch(p->ParentTerrain()->GetWestNeighborOf( p->Column(), p->Row() ))  && 
			eastLOD  == (int)LODOfPatch(p->ParentTerrain()->GetEastNeighborOf( p->Column(), p->Row() ))  &&
			southLOD == (int)LODOfPatch(p->ParentTerrain()->GetSouthNeighborOf( p->Column(), p->Row() )) &&
			northLOD == (int)LODOfPatch(p->ParentTerrain()->GetNorthNeighborOf( p->Column(), p->Row() )) &&
			LOD == (int)p->LOD());

}

bool LODDescription::Equals(LODDescription& obj)
{		
	
	LODDescription desc = (LODDescription)obj;
	if( westLOD != desc.westLOD ) return false;
	if( eastLOD != desc.eastLOD ) return false;
	if( southLOD != desc.southLOD ) return false;
	if( northLOD != desc.northLOD ) return false;
	if( LOD != desc.LOD ) return false;

	return true;
}