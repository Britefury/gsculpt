//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYGLDELETETEXTURES_CPP__
#define PYGLDELETETEXTURES_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <PlatformSpecific/IncludeGL.h>

#include <Util/Array.h>



GS_DllExport void pyGLDeleteTextures(boost::python::object textureHandles)
{
	boost::python::object lenObj = textureHandles.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numHandles = lenExtract;
		Array<GLuint> handles;
		handles.reserve( numHandles );
		for (int i = 0; i < numHandles; i++)
		{
			boost::python::object handleObj = textureHandles[i];
			boost::python::extract<int> handleExtract( handleObj );
			if ( handleExtract.check() )
			{
				handles.push_back( handleExtract );
			}
		}

		glDeleteTextures( handles.size(), handles.begin() );
	}
}


void export_glDeleteTextures()
{
	def( "glDeleteTextures", &pyGLDeleteTextures );
}


#endif
