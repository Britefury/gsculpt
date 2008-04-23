//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMESH_CPP__
#define PYMESH_CPP__

#include <boost/python.hpp>
using namespace boost::python;



void export_GSProductMesh();
void export_MImportMesh();
void export_MeshPainter();
void export_MeshPrimitives();



BOOST_PYTHON_MODULE(Mesh)
{
	export_GSProductMesh();
	export_MImportMesh();
	export_MeshPainter();
	export_MeshPrimitives();
}



#endif