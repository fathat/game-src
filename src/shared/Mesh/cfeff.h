#pragma once

#define MS_MAX_PATH             256


#pragma pack(1)

struct cf_header_t
{
	unsigned long ID; //should be "S3D1"
	long MeshCount;
	long MaterialCount;
	long AnimationTime;
	
};

struct cf_mesh
{
	long nVertices;
	long nNormals;
	long nTriangles;
	long material;
};

struct cf_vertex
{
	Real x, y, z, u, v;
};

struct cf_normal
{
	Real x, y, z;
};

struct cf_triangle
{
	long v1, v2, v3; //vertex indexes
	long n1, n2, n3; //normal indexes

};

struct cf_material
{
	Real ambient_rgba[4];
	Real diffuse_rgba[4];
	Real specular_rgba[4];
	Real emissive_rgba[4];
	Real shininess;
	Real transparency;

	char Texture[MS_MAX_PATH];
	char AlphaMap[MS_MAX_PATH];
};

#pragma pack(8)