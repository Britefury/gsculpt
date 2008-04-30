//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef _GLU_CPP
#define _GLU_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <algorithm>

#include <PlatformSpecific/IncludeGL.h>
#include <Util/Array.h>



template <typename ItemType> void _pyListToFixedArray(ItemType *dst, boost::python::list src, int size)
{
	boost::python::object lenObj = src.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )    // name
	{
		int len = lenExtract;   // name
		len = std::min( len, size );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = src[i];     // list_name
			boost::python::extract<ItemType> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				dst[i] = itemExtract;    // name
			}
		}
	}
}


void _wrap_gluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
	gluOrtho2D( left, right, bottom, top );
}


void _wrap_gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	gluPerspective( fovy, aspect, zNear, zFar );
}


void _wrap_gluPickMatrix(GLdouble x, GLdouble y, GLdouble width, GLdouble height, boost::python::list py_viewport)
{
	GLint viewport[4];
	_pyListToFixedArray( viewport, py_viewport, 4 );
	gluPickMatrix( x, y, width, height, viewport );
}



void _wrap_gluLookat(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centrex, GLdouble centrey, GLdouble centrez, GLdouble upx, GLdouble upy, GLdouble upz)
{
	gluLookAt( eyex, eyey, eyez, centrex, centrey, centrez, upx, upy, upz );
}



boost::python::object _wrap_gluProject(GLdouble objx, GLdouble objy, GLdouble objz, boost::python::list py_modelMatrix, boost::python::list py_projMatrix, boost::python::list py_viewport)
{
	GLdouble modelMatrix[16], projMatrix[16];
	GLint viewport[4];
	GLdouble winx, winy, winz;
	_pyListToFixedArray( modelMatrix, py_modelMatrix, 16 );
	_pyListToFixedArray( projMatrix, py_projMatrix, 16 );
	_pyListToFixedArray( viewport, py_viewport, 4 );
	GLint result = gluProject( objx, objy, objz, modelMatrix, projMatrix, viewport, &winx, &winy, &winz );
	if ( result == GL_TRUE )
	{
		return boost::python::make_tuple( winx, winy, winz );
	}
	else
	{
		return boost::python::object();
	}
}




boost::python::object _wrap_gluUnProject(GLdouble winx, GLdouble winy, GLdouble winz, boost::python::list py_modelMatrix, boost::python::list py_projMatrix, boost::python::list py_viewport)
{
	GLdouble modelMatrix[16], projMatrix[16];
	GLint viewport[4];
	GLdouble objx, objy, objz;
	_pyListToFixedArray( modelMatrix, py_modelMatrix, 16 );
	_pyListToFixedArray( projMatrix, py_projMatrix, 16 );
	_pyListToFixedArray( viewport, py_viewport, 4 );
	GLint result = gluUnProject( winx, winy, winz, modelMatrix, projMatrix, viewport, &objx, &objy, &objz );
	if ( result == GL_TRUE )
	{
		return boost::python::make_tuple( objx, objy, objz );
	}
	else
	{
		return boost::python::object();
	}
}


GLint _wrap_gluBuild1DMipmaps(GLenum target, GLint components, GLint width, GLenum format, GLenum type, const char *data)
{
	return gluBuild1DMipmaps( target, components, width, format, type, (const GLvoid*)data );
}

GLint _wrap_gluBuild2DMipmaps(GLenum target, GLint components, GLint width, GLint height, GLenum format, GLenum type, const char *data)
{
	return gluBuild2DMipmaps( target, components, width, height, format, type, (const GLvoid*)data );
}




class GS_DllExport W_GLUQuadric
{
public:
	GLUquadric *q;
	

	inline W_GLUQuadric()
	{
		q = gluNewQuadric();
	}
	
	inline ~W_GLUQuadric()
	{
		gluDeleteQuadric( q );
	}
};




void _wrap_gluQuadricNormals(W_GLUQuadric *q, GLenum normals)
{
	gluQuadricNormals( q->q, normals );
}

void _wrap_gluQuadricTexture(W_GLUQuadric *q, GLenum textureCoords)
{
	gluQuadricTexture( q->q, textureCoords );
}

void _wrap_gluQuadricOrientation(W_GLUQuadric *q, GLenum orientation)
{
	gluQuadricOrientation( q->q, orientation );
}

void _wrap_gluQuadricDrawStyle(W_GLUQuadric *q, GLenum drawStyle)
{
	gluQuadricDrawStyle( q->q, drawStyle );
}

void _wrap_gluCylinder(W_GLUQuadric *q, GLdouble baseRadius, GLdouble topRadius, GLdouble height, GLint slices, GLint stacks)
{
	gluCylinder( q->q, baseRadius, topRadius, height, slices, stacks );
}

void _wrap_gluDisk(W_GLUQuadric *q, GLdouble innerRadius, GLdouble outerRadius, GLint slices, GLint loops)
{
	gluDisk( q->q, innerRadius, outerRadius, slices, loops );
}

void _wrap_gluPartialDisk(W_GLUQuadric *q, GLdouble innerRadius, GLdouble outerRadius, GLint slices, GLint loops, GLdouble startAngle, GLdouble sweepAngle)
{
	gluPartialDisk( q->q, innerRadius, outerRadius, slices, loops, startAngle, sweepAngle );
}

void _wrap_gluSphere(W_GLUQuadric *q, GLdouble radius, GLint slices, GLint stacks)
{
	gluSphere( q->q, radius, slices, stacks );
}






BOOST_PYTHON_MODULE(_GLU)
{
	def( "gluOrtho2D", _wrap_gluOrtho2D );
	def( "gluPerspective", _wrap_gluPerspective );
	def( "gluPickMatrix", _wrap_gluPickMatrix );
	def( "gluLookat", _wrap_gluLookat );
	def( "gluProject", _wrap_gluProject );
	def( "gluUnProject", _wrap_gluUnProject );
	def( "gluBuild1DMipmaps", _wrap_gluBuild1DMipmaps );
	def( "gluBuild2DMipmaps", _wrap_gluBuild2DMipmaps );
	
	
	class_<W_GLUQuadric>( "GLUquadric", init<>() );
	
	def( "gluQuadricNormals", _wrap_gluQuadricNormals );
	def( "gluQuadricTexture", _wrap_gluQuadricTexture );
	def( "gluQuadricOrientation", _wrap_gluQuadricOrientation );
	def( "gluQuadricDrawStyle", _wrap_gluQuadricDrawStyle );
	def( "gluCylinder", _wrap_gluCylinder );
	def( "gluDisk", _wrap_gluDisk );
	def( "gluPartialDisk", _wrap_gluPartialDisk );
	def( "gluSphere", _wrap_gluSphere );
}


#endif
