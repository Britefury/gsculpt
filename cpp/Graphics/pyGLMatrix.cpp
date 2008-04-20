//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYGLMATRIX_CPP__
#define PYGLMATRIX_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Graphics/GLMatrix.h>


Matrix4 py_glGetMatrix()
{
	Matrix4 m;
	glGetMatrix( m );
	return m;
}


void export_GLMatrix()
{
	def( "glCurrentMatrixStack", glCurrentMatrixStack );
	def( "glGetMatrix", py_glGetMatrix );
	def( "glMultMatrix", glMultMatrix );
}


#endif
