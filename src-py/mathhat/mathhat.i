%module mathhat
%{
#include "frustrum.h"
#include "sphere.h"
#include "astar.h"
%}

%typemap(in) const float* matrix {
	
	/* Check if is a list */
	if (PyList_Check($input)) 
	{
		int size = PyList_Size($input);
		int i = 0;
		$1 = (float*)malloc(size*sizeof(float));
		for (i = 0; i < size; i++) 
		{
			PyObject *o = PyList_GetItem($input,i);
			$1[i] = (float)PyFloat_AsDouble( o );
		}
		
	} 
	else if ( $input == Py_None )
	{
		$1 = NULL;
	}
	
	else 
	{
		PyErr_SetString(PyExc_TypeError,"not a list");
		return NULL;
	}
}

%typemap(freearg) const float* matrix {
	if($1) free($1);
}

%apply const float* matrix { const float* clip_matrix };

%include "frustrum.h"
%include "sphere.h"
%include "astar.h"