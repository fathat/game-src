import Mesh

mesh_list = {}

def LoadMeshFromFile( filename ):
	global mesh_list
	if not filename: return None
	#assert not '\\' in filename
	if mesh_list.has_key( filename ):
		return mesh_list[filename]
	m = Mesh.Mesh( filename )
	mesh_list[filename] = m
	return m
