#pragma once

class TerrainPatch;

struct LODDescription
{
	int westLOD;
	int eastLOD;
	int southLOD;
	int northLOD;
	int LOD;


	void SetDefaults();
	LODDescription() { SetDefaults(); }
	LODDescription( TerrainPatch* p );
	int LODOfPatch( TerrainPatch* p );

	void LoadValuesFromPatch( TerrainPatch* p );

	bool IsConsistentWithPatch( TerrainPatch* p );

	bool Equals(LODDescription& obj);
};