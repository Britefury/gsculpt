//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef _GL_CPP
#define _GL_CPP


//
//
// GENERATED CODE; DO NOT EDIT
//
//


#include <boost/python.hpp>
using namespace boost::python;


#include <algorithm>

#include <PlatformSpecific/IncludeGL.h>
#include <Util/Array.h>


void _wrap_glCallLists(const std::string &lists)
{
	glCallLists( lists.size(), GL_UNSIGNED_BYTE, lists.c_str() );
}



boost::python::list _wrap_glGenTextures(int n)
{
	boost::python::list py_textures;
	Array<GLuint> textures;
	textures.resize( n );
	glGenTextures( n, textures.begin() );
	for (int i = 0; i < n; i++)
	{
		py_textures.append( textures[i] );
	}
	return py_textures;
}



void _wrap_glDeleteTextures(boost::python::list py_textures)
{
	boost::python::object lenObj = py_textures.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numTextures = lenExtract;
		Array<GLuint> textures;
		textures.resize( numTextures );
		for (int i = 0; i < numTextures; i++)
		{
			boost::python::object texObj = py_textures[i];     // list_name
			boost::python::extract<GLuint> texExtract( texObj );     // type
			if ( texExtract.check() )
			{
				textures[i] = texExtract;
			}
		}
		glDeleteTextures( numTextures, textures.begin() );
	}
}


void _wrap_glAccum(GLenum op, GLfloat value)
{
	glAccum( op, value );
}



void _wrap_glAlphaFunc(GLenum func, GLclampf ref)
{
	glAlphaFunc( func, ref );
}



void _wrap_glArrayElement(GLint i)
{
	glArrayElement( i );
}



void _wrap_glBegin(GLenum mode)
{
	glBegin( mode );
}



void _wrap_glBindTexture(GLenum target, GLuint texture)
{
	glBindTexture( target, texture );
}



void _wrap_glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	glBlendFunc( sfactor, dfactor );
}



void _wrap_glCallList(GLuint list)
{
	glCallList( list );
}



void _wrap_glClear(GLbitfield mask)
{
	glClear( mask );
}



void _wrap_glClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	glClearAccum( red, green, blue, alpha );
}



void _wrap_glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	glClearColor( red, green, blue, alpha );
}



void _wrap_glClearDepth(GLclampd depth)
{
	glClearDepth( depth );
}



void _wrap_glClearIndex(GLfloat c)
{
	glClearIndex( c );
}



void _wrap_glClearStencil(GLint s)
{
	glClearStencil( s );
}



void _wrap_glClipPlane(GLenum plane, boost::python::list py_equation)
{
	GLdouble equation[4];   // type   name   size
	boost::python::object equation_lenObj = py_equation.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> equation_lenExtract( equation_lenObj );   // name   name
	if ( equation_lenExtract.check() )    // name
	{
		int len = equation_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_equation[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				equation[i] = itemExtract;    // name
			}
		}
	}
	
	glClipPlane( plane, equation );
}



void _wrap_glColor3b(GLbyte red, GLbyte green, GLbyte blue)
{
	glColor3b( red, green, blue );
}



void _wrap_glColor3bv(boost::python::list py_v)
{
	GLbyte v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLbyte> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor3bv( v );
}



void _wrap_glColor3d(GLdouble red, GLdouble green, GLdouble blue)
{
	glColor3d( red, green, blue );
}



void _wrap_glColor3dv(boost::python::list py_v)
{
	GLdouble v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor3dv( v );
}



void _wrap_glColor3f(GLfloat red, GLfloat green, GLfloat blue)
{
	glColor3f( red, green, blue );
}



void _wrap_glColor3fv(boost::python::list py_v)
{
	GLfloat v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor3fv( v );
}



void _wrap_glColor3i(GLint red, GLint green, GLint blue)
{
	glColor3i( red, green, blue );
}



void _wrap_glColor3iv(boost::python::list py_v)
{
	GLint v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor3iv( v );
}



void _wrap_glColor3s(GLshort red, GLshort green, GLshort blue)
{
	glColor3s( red, green, blue );
}



void _wrap_glColor3sv(boost::python::list py_v)
{
	GLshort v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor3sv( v );
}



void _wrap_glColor3ub(GLubyte red, GLubyte green, GLubyte blue)
{
	glColor3ub( red, green, blue );
}



void _wrap_glColor3ubv(boost::python::list py_v)
{
	GLubyte v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLubyte> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor3ubv( v );
}



void _wrap_glColor3ui(GLuint red, GLuint green, GLuint blue)
{
	glColor3ui( red, green, blue );
}



void _wrap_glColor3uiv(boost::python::list py_v)
{
	GLuint v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLuint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor3uiv( v );
}



void _wrap_glColor3us(GLushort red, GLushort green, GLushort blue)
{
	glColor3us( red, green, blue );
}



void _wrap_glColor3usv(boost::python::list py_v)
{
	GLushort v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLushort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor3usv( v );
}



void _wrap_glColor4b(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
	glColor4b( red, green, blue, alpha );
}



void _wrap_glColor4bv(boost::python::list py_v)
{
	GLbyte v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLbyte> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor4bv( v );
}



void _wrap_glColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
	glColor4d( red, green, blue, alpha );
}



void _wrap_glColor4dv(boost::python::list py_v)
{
	GLdouble v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor4dv( v );
}



void _wrap_glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	glColor4f( red, green, blue, alpha );
}



void _wrap_glColor4fv(boost::python::list py_v)
{
	GLfloat v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor4fv( v );
}



void _wrap_glColor4i(GLint red, GLint green, GLint blue, GLint alpha)
{
	glColor4i( red, green, blue, alpha );
}



void _wrap_glColor4iv(boost::python::list py_v)
{
	GLint v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor4iv( v );
}



void _wrap_glColor4s(GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
	glColor4s( red, green, blue, alpha );
}



void _wrap_glColor4sv(boost::python::list py_v)
{
	GLshort v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor4sv( v );
}



void _wrap_glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	glColor4ub( red, green, blue, alpha );
}



void _wrap_glColor4ubv(boost::python::list py_v)
{
	GLubyte v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLubyte> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor4ubv( v );
}



void _wrap_glColor4ui(GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
	glColor4ui( red, green, blue, alpha );
}



void _wrap_glColor4uiv(boost::python::list py_v)
{
	GLuint v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLuint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor4uiv( v );
}



void _wrap_glColor4us(GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
	glColor4us( red, green, blue, alpha );
}



void _wrap_glColor4usv(boost::python::list py_v)
{
	GLushort v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLushort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glColor4usv( v );
}



void _wrap_glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	glColorMask( red, green, blue, alpha );
}



void _wrap_glColorMaterial(GLenum face, GLenum mode)
{
	glColorMaterial( face, mode );
}



void _wrap_glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
	glCopyPixels( x, y, width, height, type );
}



void _wrap_glCopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border)
{
	glCopyTexImage1D( target, level, internalFormat, x, y, width, border );
}



void _wrap_glCopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
	glCopyTexImage2D( target, level, internalFormat, x, y, width, height, border );
}



void _wrap_glCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
	glCopyTexSubImage1D( target, level, xoffset, x, y, width );
}



void _wrap_glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	glCopyTexSubImage2D( target, level, xoffset, yoffset, x, y, width, height );
}



void _wrap_glCullFace(GLenum mode)
{
	glCullFace( mode );
}



void _wrap_glDeleteLists(GLuint list, GLsizei range)
{
	glDeleteLists( list, range );
}



void _wrap_glDepthFunc(GLenum func)
{
	glDepthFunc( func );
}



void _wrap_glDepthMask(GLboolean flag)
{
	glDepthMask( flag );
}



void _wrap_glDepthRange(GLclampd zNear, GLclampd zFar)
{
	glDepthRange( zNear, zFar );
}



void _wrap_glDisable(GLenum cap)
{
	glDisable( cap );
}



void _wrap_glDisableClientState(GLenum array)
{
	glDisableClientState( array );
}



void _wrap_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	glDrawArrays( mode, first, count );
}



void _wrap_glDrawBuffer(GLenum mode)
{
	glDrawBuffer( mode );
}



void _wrap_glEdgeFlag(GLboolean flag)
{
	glEdgeFlag( flag );
}



void _wrap_glEdgeFlagv(boost::python::list py_flag)
{
	Array<GLboolean> flag;   // type   name
	boost::python::object flag_lenObj = py_flag.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> flag_lenExtract( flag_lenObj );   // name   name
	if ( flag_lenExtract.check() )    // name
	{
		int len = flag_lenExtract;   // name
		flag.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_flag[i];     // list_name
			boost::python::extract<GLboolean> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				flag[i] = targetExtract;    // name
			}
		}
	}
	
	glEdgeFlagv( flag.begin() );
}



void _wrap_glEnable(GLenum cap)
{
	glEnable( cap );
}



void _wrap_glEnableClientState(GLenum array)
{
	glEnableClientState( array );
}



void _wrap_glEnd()
{
	glEnd(  );
}



void _wrap_glEndList()
{
	glEndList(  );
}



void _wrap_glEvalCoord1d(GLdouble u)
{
	glEvalCoord1d( u );
}



void _wrap_glEvalCoord1dv(boost::python::list py_u)
{
	GLdouble u[1];   // type   name   size
	boost::python::object u_lenObj = py_u.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> u_lenExtract( u_lenObj );   // name   name
	if ( u_lenExtract.check() )    // name
	{
		int len = u_lenExtract;   // name
		len = std::min( len, 1 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_u[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				u[i] = itemExtract;    // name
			}
		}
	}
	
	glEvalCoord1dv( u );
}



void _wrap_glEvalCoord1f(GLfloat u)
{
	glEvalCoord1f( u );
}



void _wrap_glEvalCoord1fv(boost::python::list py_u)
{
	GLfloat u[1];   // type   name   size
	boost::python::object u_lenObj = py_u.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> u_lenExtract( u_lenObj );   // name   name
	if ( u_lenExtract.check() )    // name
	{
		int len = u_lenExtract;   // name
		len = std::min( len, 1 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_u[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				u[i] = itemExtract;    // name
			}
		}
	}
	
	glEvalCoord1fv( u );
}



void _wrap_glEvalCoord2d(GLdouble u, GLdouble v)
{
	glEvalCoord2d( u, v );
}



void _wrap_glEvalCoord2dv(boost::python::list py_u)
{
	GLdouble u[2];   // type   name   size
	boost::python::object u_lenObj = py_u.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> u_lenExtract( u_lenObj );   // name   name
	if ( u_lenExtract.check() )    // name
	{
		int len = u_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_u[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				u[i] = itemExtract;    // name
			}
		}
	}
	
	glEvalCoord2dv( u );
}



void _wrap_glEvalCoord2f(GLfloat u, GLfloat v)
{
	glEvalCoord2f( u, v );
}



void _wrap_glEvalCoord2fv(boost::python::list py_u)
{
	GLfloat u[2];   // type   name   size
	boost::python::object u_lenObj = py_u.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> u_lenExtract( u_lenObj );   // name   name
	if ( u_lenExtract.check() )    // name
	{
		int len = u_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_u[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				u[i] = itemExtract;    // name
			}
		}
	}
	
	glEvalCoord2fv( u );
}



void _wrap_glEvalMesh1(GLenum mode, GLint i1, GLint i2)
{
	glEvalMesh1( mode, i1, i2 );
}



void _wrap_glEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
	glEvalMesh2( mode, i1, i2, j1, j2 );
}



void _wrap_glEvalPoint1(GLint i)
{
	glEvalPoint1( i );
}



void _wrap_glEvalPoint2(GLint i, GLint j)
{
	glEvalPoint2( i, j );
}



void _wrap_glFinish()
{
	glFinish(  );
}



void _wrap_glFlush()
{
	glFlush(  );
}



void _wrap_glFogf(GLenum pname, GLfloat param)
{
	glFogf( pname, param );
}



void _wrap_glFogfv(GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glFogfv( pname, params.begin() );
}



void _wrap_glFogi(GLenum pname, GLint param)
{
	glFogi( pname, param );
}



void _wrap_glFogiv(GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glFogiv( pname, params.begin() );
}



void _wrap_glFrontFace(GLenum mode)
{
	glFrontFace( mode );
}



void _wrap_glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	glFrustum( left, right, bottom, top, zNear, zFar );
}



GLuint _wrap_glGenLists(GLsizei range)
{
	return glGenLists( range );
}



GLenum _wrap_glGetError()
{
	return glGetError(  );
}



void _wrap_glHint(GLenum target, GLenum mode)
{
	glHint( target, mode );
}



void _wrap_glIndexMask(GLuint mask)
{
	glIndexMask( mask );
}



void _wrap_glIndexd(GLdouble c)
{
	glIndexd( c );
}



void _wrap_glIndexdv(boost::python::list py_c)
{
	Array<GLdouble> c;   // type   name
	boost::python::object c_lenObj = py_c.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> c_lenExtract( c_lenObj );   // name   name
	if ( c_lenExtract.check() )    // name
	{
		int len = c_lenExtract;   // name
		c.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_c[i];     // list_name
			boost::python::extract<GLdouble> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				c[i] = targetExtract;    // name
			}
		}
	}
	
	glIndexdv( c.begin() );
}



void _wrap_glIndexf(GLfloat c)
{
	glIndexf( c );
}



void _wrap_glIndexfv(boost::python::list py_c)
{
	Array<GLfloat> c;   // type   name
	boost::python::object c_lenObj = py_c.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> c_lenExtract( c_lenObj );   // name   name
	if ( c_lenExtract.check() )    // name
	{
		int len = c_lenExtract;   // name
		c.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_c[i];     // list_name
			boost::python::extract<GLfloat> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				c[i] = targetExtract;    // name
			}
		}
	}
	
	glIndexfv( c.begin() );
}



void _wrap_glIndexi(GLint c)
{
	glIndexi( c );
}



void _wrap_glIndexiv(boost::python::list py_c)
{
	Array<GLint> c;   // type   name
	boost::python::object c_lenObj = py_c.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> c_lenExtract( c_lenObj );   // name   name
	if ( c_lenExtract.check() )    // name
	{
		int len = c_lenExtract;   // name
		c.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_c[i];     // list_name
			boost::python::extract<GLint> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				c[i] = targetExtract;    // name
			}
		}
	}
	
	glIndexiv( c.begin() );
}



void _wrap_glIndexs(GLshort c)
{
	glIndexs( c );
}



void _wrap_glIndexsv(boost::python::list py_c)
{
	Array<GLshort> c;   // type   name
	boost::python::object c_lenObj = py_c.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> c_lenExtract( c_lenObj );   // name   name
	if ( c_lenExtract.check() )    // name
	{
		int len = c_lenExtract;   // name
		c.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_c[i];     // list_name
			boost::python::extract<GLshort> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				c[i] = targetExtract;    // name
			}
		}
	}
	
	glIndexsv( c.begin() );
}



void _wrap_glIndexub(GLubyte c)
{
	glIndexub( c );
}



void _wrap_glIndexubv(boost::python::list py_c)
{
	Array<GLubyte> c;   // type   name
	boost::python::object c_lenObj = py_c.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> c_lenExtract( c_lenObj );   // name   name
	if ( c_lenExtract.check() )    // name
	{
		int len = c_lenExtract;   // name
		c.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_c[i];     // list_name
			boost::python::extract<GLubyte> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				c[i] = targetExtract;    // name
			}
		}
	}
	
	glIndexubv( c.begin() );
}



void _wrap_glInitNames()
{
	glInitNames(  );
}



GLboolean _wrap_glIsEnabled(GLenum cap)
{
	return glIsEnabled( cap );
}



GLboolean _wrap_glIsList(GLuint list)
{
	return glIsList( list );
}



GLboolean _wrap_glIsTexture(GLuint texture)
{
	return glIsTexture( texture );
}



void _wrap_glLightModelf(GLenum pname, GLfloat param)
{
	glLightModelf( pname, param );
}



void _wrap_glLightModelfv(GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glLightModelfv( pname, params.begin() );
}



void _wrap_glLightModeli(GLenum pname, GLint param)
{
	glLightModeli( pname, param );
}



void _wrap_glLightModeliv(GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glLightModeliv( pname, params.begin() );
}



void _wrap_glLightf(GLenum light, GLenum pname, GLfloat param)
{
	glLightf( light, pname, param );
}



void _wrap_glLightfv(GLenum light, GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glLightfv( light, pname, params.begin() );
}



void _wrap_glLighti(GLenum light, GLenum pname, GLint param)
{
	glLighti( light, pname, param );
}



void _wrap_glLightiv(GLenum light, GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glLightiv( light, pname, params.begin() );
}



void _wrap_glLineStipple(GLint factor, GLushort pattern)
{
	glLineStipple( factor, pattern );
}



void _wrap_glLineWidth(GLfloat width)
{
	glLineWidth( width );
}



void _wrap_glListBase(GLuint base)
{
	glListBase( base );
}



void _wrap_glLoadIdentity()
{
	glLoadIdentity(  );
}



void _wrap_glLoadMatrixd(boost::python::list py_m)
{
	GLdouble m[16];   // type   name   size
	boost::python::object m_lenObj = py_m.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> m_lenExtract( m_lenObj );   // name   name
	if ( m_lenExtract.check() )    // name
	{
		int len = m_lenExtract;   // name
		len = std::min( len, 16 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_m[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				m[i] = itemExtract;    // name
			}
		}
	}
	
	glLoadMatrixd( m );
}



void _wrap_glLoadMatrixf(boost::python::list py_m)
{
	GLfloat m[16];   // type   name   size
	boost::python::object m_lenObj = py_m.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> m_lenExtract( m_lenObj );   // name   name
	if ( m_lenExtract.check() )    // name
	{
		int len = m_lenExtract;   // name
		len = std::min( len, 16 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_m[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				m[i] = itemExtract;    // name
			}
		}
	}
	
	glLoadMatrixf( m );
}



void _wrap_glLoadName(GLuint name)
{
	glLoadName( name );
}



void _wrap_glLogicOp(GLenum opcode)
{
	glLogicOp( opcode );
}



void _wrap_glMapGrid1d(GLint un, GLdouble u1, GLdouble u2)
{
	glMapGrid1d( un, u1, u2 );
}



void _wrap_glMapGrid1f(GLint un, GLfloat u1, GLfloat u2)
{
	glMapGrid1f( un, u1, u2 );
}



void _wrap_glMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{
	glMapGrid2d( un, u1, u2, vn, v1, v2 );
}



void _wrap_glMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{
	glMapGrid2f( un, u1, u2, vn, v1, v2 );
}



void _wrap_glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
	glMaterialf( face, pname, param );
}



void _wrap_glMaterialfv(GLenum face, GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glMaterialfv( face, pname, params.begin() );
}



void _wrap_glMateriali(GLenum face, GLenum pname, GLint param)
{
	glMateriali( face, pname, param );
}



void _wrap_glMaterialiv(GLenum face, GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glMaterialiv( face, pname, params.begin() );
}



void _wrap_glMatrixMode(GLenum mode)
{
	glMatrixMode( mode );
}



void _wrap_glMultMatrixd(boost::python::list py_m)
{
	GLdouble m[16];   // type   name   size
	boost::python::object m_lenObj = py_m.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> m_lenExtract( m_lenObj );   // name   name
	if ( m_lenExtract.check() )    // name
	{
		int len = m_lenExtract;   // name
		len = std::min( len, 16 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_m[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				m[i] = itemExtract;    // name
			}
		}
	}
	
	glMultMatrixd( m );
}



void _wrap_glMultMatrixf(boost::python::list py_m)
{
	GLfloat m[16];   // type   name   size
	boost::python::object m_lenObj = py_m.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> m_lenExtract( m_lenObj );   // name   name
	if ( m_lenExtract.check() )    // name
	{
		int len = m_lenExtract;   // name
		len = std::min( len, 16 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_m[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				m[i] = itemExtract;    // name
			}
		}
	}
	
	glMultMatrixf( m );
}



void _wrap_glNewList(GLuint list, GLenum mode)
{
	glNewList( list, mode );
}



void _wrap_glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz)
{
	glNormal3b( nx, ny, nz );
}



void _wrap_glNormal3bv(boost::python::list py_v)
{
	GLbyte v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLbyte> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glNormal3bv( v );
}



void _wrap_glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz)
{
	glNormal3d( nx, ny, nz );
}



void _wrap_glNormal3dv(boost::python::list py_v)
{
	GLdouble v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glNormal3dv( v );
}



void _wrap_glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
	glNormal3f( nx, ny, nz );
}



void _wrap_glNormal3fv(boost::python::list py_v)
{
	GLfloat v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glNormal3fv( v );
}



void _wrap_glNormal3i(GLint nx, GLint ny, GLint nz)
{
	glNormal3i( nx, ny, nz );
}



void _wrap_glNormal3iv(boost::python::list py_v)
{
	GLint v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glNormal3iv( v );
}



void _wrap_glNormal3s(GLshort nx, GLshort ny, GLshort nz)
{
	glNormal3s( nx, ny, nz );
}



void _wrap_glNormal3sv(boost::python::list py_v)
{
	GLshort v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glNormal3sv( v );
}



void _wrap_glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	glOrtho( left, right, bottom, top, zNear, zFar );
}



void _wrap_glPassThrough(GLfloat token)
{
	glPassThrough( token );
}



void _wrap_glPixelMapfv(GLenum map, GLsizei mapsize, boost::python::list py_values)
{
	Array<GLfloat> values;   // type   name
	boost::python::object values_lenObj = py_values.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> values_lenExtract( values_lenObj );   // name   name
	if ( values_lenExtract.check() )    // name
	{
		int len = values_lenExtract;   // name
		values.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_values[i];     // list_name
			boost::python::extract<GLfloat> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				values[i] = targetExtract;    // name
			}
		}
	}
	
	glPixelMapfv( map, mapsize, values.begin() );
}



void _wrap_glPixelMapuiv(GLenum map, GLsizei mapsize, boost::python::list py_values)
{
	Array<GLuint> values;   // type   name
	boost::python::object values_lenObj = py_values.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> values_lenExtract( values_lenObj );   // name   name
	if ( values_lenExtract.check() )    // name
	{
		int len = values_lenExtract;   // name
		values.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_values[i];     // list_name
			boost::python::extract<GLuint> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				values[i] = targetExtract;    // name
			}
		}
	}
	
	glPixelMapuiv( map, mapsize, values.begin() );
}



void _wrap_glPixelMapusv(GLenum map, GLsizei mapsize, boost::python::list py_values)
{
	Array<GLushort> values;   // type   name
	boost::python::object values_lenObj = py_values.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> values_lenExtract( values_lenObj );   // name   name
	if ( values_lenExtract.check() )    // name
	{
		int len = values_lenExtract;   // name
		values.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_values[i];     // list_name
			boost::python::extract<GLushort> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				values[i] = targetExtract;    // name
			}
		}
	}
	
	glPixelMapusv( map, mapsize, values.begin() );
}



void _wrap_glPixelStoref(GLenum pname, GLfloat param)
{
	glPixelStoref( pname, param );
}



void _wrap_glPixelStorei(GLenum pname, GLint param)
{
	glPixelStorei( pname, param );
}



void _wrap_glPixelTransferf(GLenum pname, GLfloat param)
{
	glPixelTransferf( pname, param );
}



void _wrap_glPixelTransferi(GLenum pname, GLint param)
{
	glPixelTransferi( pname, param );
}



void _wrap_glPixelZoom(GLfloat xfactor, GLfloat yfactor)
{
	glPixelZoom( xfactor, yfactor );
}



void _wrap_glPointSize(GLfloat size)
{
	glPointSize( size );
}



void _wrap_glPolygonMode(GLenum face, GLenum mode)
{
	glPolygonMode( face, mode );
}



void _wrap_glPolygonOffset(GLfloat factor, GLfloat units)
{
	glPolygonOffset( factor, units );
}



void _wrap_glPopAttrib()
{
	glPopAttrib(  );
}



void _wrap_glPopClientAttrib()
{
	glPopClientAttrib(  );
}



void _wrap_glPopMatrix()
{
	glPopMatrix(  );
}



void _wrap_glPopName()
{
	glPopName(  );
}



void _wrap_glPushAttrib(GLbitfield mask)
{
	glPushAttrib( mask );
}



void _wrap_glPushClientAttrib(GLbitfield mask)
{
	glPushClientAttrib( mask );
}



void _wrap_glPushMatrix()
{
	glPushMatrix(  );
}



void _wrap_glPushName(GLuint name)
{
	glPushName( name );
}



void _wrap_glRasterPos2d(GLdouble x, GLdouble y)
{
	glRasterPos2d( x, y );
}



void _wrap_glRasterPos2dv(boost::python::list py_v)
{
	GLdouble v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos2dv( v );
}



void _wrap_glRasterPos2f(GLfloat x, GLfloat y)
{
	glRasterPos2f( x, y );
}



void _wrap_glRasterPos2fv(boost::python::list py_v)
{
	GLfloat v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos2fv( v );
}



void _wrap_glRasterPos2i(GLint x, GLint y)
{
	glRasterPos2i( x, y );
}



void _wrap_glRasterPos2iv(boost::python::list py_v)
{
	GLint v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos2iv( v );
}



void _wrap_glRasterPos2s(GLshort x, GLshort y)
{
	glRasterPos2s( x, y );
}



void _wrap_glRasterPos2sv(boost::python::list py_v)
{
	GLshort v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos2sv( v );
}



void _wrap_glRasterPos3d(GLdouble x, GLdouble y, GLdouble z)
{
	glRasterPos3d( x, y, z );
}



void _wrap_glRasterPos3dv(boost::python::list py_v)
{
	GLdouble v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos3dv( v );
}



void _wrap_glRasterPos3f(GLfloat x, GLfloat y, GLfloat z)
{
	glRasterPos3f( x, y, z );
}



void _wrap_glRasterPos3fv(boost::python::list py_v)
{
	GLfloat v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos3fv( v );
}



void _wrap_glRasterPos3i(GLint x, GLint y, GLint z)
{
	glRasterPos3i( x, y, z );
}



void _wrap_glRasterPos3iv(boost::python::list py_v)
{
	GLint v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos3iv( v );
}



void _wrap_glRasterPos3s(GLshort x, GLshort y, GLshort z)
{
	glRasterPos3s( x, y, z );
}



void _wrap_glRasterPos3sv(boost::python::list py_v)
{
	GLshort v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos3sv( v );
}



void _wrap_glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	glRasterPos4d( x, y, z, w );
}



void _wrap_glRasterPos4dv(boost::python::list py_v)
{
	GLdouble v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos4dv( v );
}



void _wrap_glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	glRasterPos4f( x, y, z, w );
}



void _wrap_glRasterPos4fv(boost::python::list py_v)
{
	GLfloat v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos4fv( v );
}



void _wrap_glRasterPos4i(GLint x, GLint y, GLint z, GLint w)
{
	glRasterPos4i( x, y, z, w );
}



void _wrap_glRasterPos4iv(boost::python::list py_v)
{
	GLint v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos4iv( v );
}



void _wrap_glRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
	glRasterPos4s( x, y, z, w );
}



void _wrap_glRasterPos4sv(boost::python::list py_v)
{
	GLshort v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glRasterPos4sv( v );
}



void _wrap_glReadBuffer(GLenum mode)
{
	glReadBuffer( mode );
}



void _wrap_glRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
	glRectd( x1, y1, x2, y2 );
}



void _wrap_glRectdv(boost::python::list py_v1, boost::python::list py_v2)
{
	GLdouble v1[2];   // type   name   size
	boost::python::object v1_lenObj = py_v1.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v1_lenExtract( v1_lenObj );   // name   name
	if ( v1_lenExtract.check() )    // name
	{
		int len = v1_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v1[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v1[i] = itemExtract;    // name
			}
		}
	}
	
	GLdouble v2[2];   // type   name   size
	boost::python::object v2_lenObj = py_v2.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v2_lenExtract( v2_lenObj );   // name   name
	if ( v2_lenExtract.check() )    // name
	{
		int len = v2_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v2[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v2[i] = itemExtract;    // name
			}
		}
	}
	
	glRectdv( v1, v2 );
}



void _wrap_glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	glRectf( x1, y1, x2, y2 );
}



void _wrap_glRectfv(boost::python::list py_v1, boost::python::list py_v2)
{
	GLfloat v1[2];   // type   name   size
	boost::python::object v1_lenObj = py_v1.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v1_lenExtract( v1_lenObj );   // name   name
	if ( v1_lenExtract.check() )    // name
	{
		int len = v1_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v1[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v1[i] = itemExtract;    // name
			}
		}
	}
	
	GLfloat v2[2];   // type   name   size
	boost::python::object v2_lenObj = py_v2.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v2_lenExtract( v2_lenObj );   // name   name
	if ( v2_lenExtract.check() )    // name
	{
		int len = v2_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v2[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v2[i] = itemExtract;    // name
			}
		}
	}
	
	glRectfv( v1, v2 );
}



void _wrap_glRecti(GLint x1, GLint y1, GLint x2, GLint y2)
{
	glRecti( x1, y1, x2, y2 );
}



void _wrap_glRectiv(boost::python::list py_v1, boost::python::list py_v2)
{
	GLint v1[2];   // type   name   size
	boost::python::object v1_lenObj = py_v1.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v1_lenExtract( v1_lenObj );   // name   name
	if ( v1_lenExtract.check() )    // name
	{
		int len = v1_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v1[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v1[i] = itemExtract;    // name
			}
		}
	}
	
	GLint v2[2];   // type   name   size
	boost::python::object v2_lenObj = py_v2.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v2_lenExtract( v2_lenObj );   // name   name
	if ( v2_lenExtract.check() )    // name
	{
		int len = v2_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v2[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v2[i] = itemExtract;    // name
			}
		}
	}
	
	glRectiv( v1, v2 );
}



void _wrap_glRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
	glRects( x1, y1, x2, y2 );
}



void _wrap_glRectsv(boost::python::list py_v1, boost::python::list py_v2)
{
	GLshort v1[2];   // type   name   size
	boost::python::object v1_lenObj = py_v1.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v1_lenExtract( v1_lenObj );   // name   name
	if ( v1_lenExtract.check() )    // name
	{
		int len = v1_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v1[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v1[i] = itemExtract;    // name
			}
		}
	}
	
	GLshort v2[2];   // type   name   size
	boost::python::object v2_lenObj = py_v2.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v2_lenExtract( v2_lenObj );   // name   name
	if ( v2_lenExtract.check() )    // name
	{
		int len = v2_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v2[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v2[i] = itemExtract;    // name
			}
		}
	}
	
	glRectsv( v1, v2 );
}



GLint _wrap_glRenderMode(GLenum mode)
{
	return glRenderMode( mode );
}



void _wrap_glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
	glRotated( angle, x, y, z );
}



void _wrap_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	glRotatef( angle, x, y, z );
}



void _wrap_glScaled(GLdouble x, GLdouble y, GLdouble z)
{
	glScaled( x, y, z );
}



void _wrap_glScalef(GLfloat x, GLfloat y, GLfloat z)
{
	glScalef( x, y, z );
}



void _wrap_glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	glScissor( x, y, width, height );
}



void _wrap_glShadeModel(GLenum mode)
{
	glShadeModel( mode );
}



void _wrap_glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
	glStencilFunc( func, ref, mask );
}



void _wrap_glStencilMask(GLuint mask)
{
	glStencilMask( mask );
}



void _wrap_glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
	glStencilOp( fail, zfail, zpass );
}



void _wrap_glTexCoord1d(GLdouble s)
{
	glTexCoord1d( s );
}



void _wrap_glTexCoord1dv(boost::python::list py_v)
{
	GLdouble v[1];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 1 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord1dv( v );
}



void _wrap_glTexCoord1f(GLfloat s)
{
	glTexCoord1f( s );
}



void _wrap_glTexCoord1fv(boost::python::list py_v)
{
	GLfloat v[1];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 1 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord1fv( v );
}



void _wrap_glTexCoord1i(GLint s)
{
	glTexCoord1i( s );
}



void _wrap_glTexCoord1iv(boost::python::list py_v)
{
	GLint v[1];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 1 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord1iv( v );
}



void _wrap_glTexCoord1s(GLshort s)
{
	glTexCoord1s( s );
}



void _wrap_glTexCoord1sv(boost::python::list py_v)
{
	GLshort v[1];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 1 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord1sv( v );
}



void _wrap_glTexCoord2d(GLdouble s, GLdouble t)
{
	glTexCoord2d( s, t );
}



void _wrap_glTexCoord2dv(boost::python::list py_v)
{
	GLdouble v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord2dv( v );
}



void _wrap_glTexCoord2f(GLfloat s, GLfloat t)
{
	glTexCoord2f( s, t );
}



void _wrap_glTexCoord2fv(boost::python::list py_v)
{
	GLfloat v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord2fv( v );
}



void _wrap_glTexCoord2i(GLint s, GLint t)
{
	glTexCoord2i( s, t );
}



void _wrap_glTexCoord2iv(boost::python::list py_v)
{
	GLint v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord2iv( v );
}



void _wrap_glTexCoord2s(GLshort s, GLshort t)
{
	glTexCoord2s( s, t );
}



void _wrap_glTexCoord2sv(boost::python::list py_v)
{
	GLshort v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord2sv( v );
}



void _wrap_glTexCoord3d(GLdouble s, GLdouble t, GLdouble r)
{
	glTexCoord3d( s, t, r );
}



void _wrap_glTexCoord3dv(boost::python::list py_v)
{
	GLdouble v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord3dv( v );
}



void _wrap_glTexCoord3f(GLfloat s, GLfloat t, GLfloat r)
{
	glTexCoord3f( s, t, r );
}



void _wrap_glTexCoord3fv(boost::python::list py_v)
{
	GLfloat v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord3fv( v );
}



void _wrap_glTexCoord3i(GLint s, GLint t, GLint r)
{
	glTexCoord3i( s, t, r );
}



void _wrap_glTexCoord3iv(boost::python::list py_v)
{
	GLint v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord3iv( v );
}



void _wrap_glTexCoord3s(GLshort s, GLshort t, GLshort r)
{
	glTexCoord3s( s, t, r );
}



void _wrap_glTexCoord3sv(boost::python::list py_v)
{
	GLshort v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord3sv( v );
}



void _wrap_glTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
	glTexCoord4d( s, t, r, q );
}



void _wrap_glTexCoord4dv(boost::python::list py_v)
{
	GLdouble v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord4dv( v );
}



void _wrap_glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	glTexCoord4f( s, t, r, q );
}



void _wrap_glTexCoord4fv(boost::python::list py_v)
{
	GLfloat v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord4fv( v );
}



void _wrap_glTexCoord4i(GLint s, GLint t, GLint r, GLint q)
{
	glTexCoord4i( s, t, r, q );
}



void _wrap_glTexCoord4iv(boost::python::list py_v)
{
	GLint v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord4iv( v );
}



void _wrap_glTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q)
{
	glTexCoord4s( s, t, r, q );
}



void _wrap_glTexCoord4sv(boost::python::list py_v)
{
	GLshort v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glTexCoord4sv( v );
}



void _wrap_glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
	glTexEnvf( target, pname, param );
}



void _wrap_glTexEnvfv(GLenum target, GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glTexEnvfv( target, pname, params.begin() );
}



void _wrap_glTexEnvi(GLenum target, GLenum pname, GLint param)
{
	glTexEnvi( target, pname, param );
}



void _wrap_glTexEnviv(GLenum target, GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glTexEnviv( target, pname, params.begin() );
}



void _wrap_glTexGend(GLenum coord, GLenum pname, GLdouble param)
{
	glTexGend( coord, pname, param );
}



void _wrap_glTexGendv(GLenum coord, GLenum pname, boost::python::list py_params)
{
	Array<GLdouble> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLdouble> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glTexGendv( coord, pname, params.begin() );
}



void _wrap_glTexGenf(GLenum coord, GLenum pname, GLfloat param)
{
	glTexGenf( coord, pname, param );
}



void _wrap_glTexGenfv(GLenum coord, GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glTexGenfv( coord, pname, params.begin() );
}



void _wrap_glTexGeni(GLenum coord, GLenum pname, GLint param)
{
	glTexGeni( coord, pname, param );
}



void _wrap_glTexGeniv(GLenum coord, GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glTexGeniv( coord, pname, params.begin() );
}



void _wrap_glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	glTexParameterf( target, pname, param );
}



void _wrap_glTexParameterfv(GLenum target, GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glTexParameterfv( target, pname, params.begin() );
}



void _wrap_glTexParameteri(GLenum target, GLenum pname, GLint param)
{
	glTexParameteri( target, pname, param );
}



void _wrap_glTexParameteriv(GLenum target, GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int len = params_lenExtract;   // name
		params.resize( len );   // name
		for (int i = 0; i < len; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params[i] = targetExtract;    // name
			}
		}
	}
	
	glTexParameteriv( target, pname, params.begin() );
}



void _wrap_glTranslated(GLdouble x, GLdouble y, GLdouble z)
{
	glTranslated( x, y, z );
}



void _wrap_glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	glTranslatef( x, y, z );
}



void _wrap_glVertex2d(GLdouble x, GLdouble y)
{
	glVertex2d( x, y );
}



void _wrap_glVertex2dv(boost::python::list py_v)
{
	GLdouble v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex2dv( v );
}



void _wrap_glVertex2f(GLfloat x, GLfloat y)
{
	glVertex2f( x, y );
}



void _wrap_glVertex2fv(boost::python::list py_v)
{
	GLfloat v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex2fv( v );
}



void _wrap_glVertex2i(GLint x, GLint y)
{
	glVertex2i( x, y );
}



void _wrap_glVertex2iv(boost::python::list py_v)
{
	GLint v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex2iv( v );
}



void _wrap_glVertex2s(GLshort x, GLshort y)
{
	glVertex2s( x, y );
}



void _wrap_glVertex2sv(boost::python::list py_v)
{
	GLshort v[2];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 2 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex2sv( v );
}



void _wrap_glVertex3d(GLdouble x, GLdouble y, GLdouble z)
{
	glVertex3d( x, y, z );
}



void _wrap_glVertex3dv(boost::python::list py_v)
{
	GLdouble v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex3dv( v );
}



void _wrap_glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
	glVertex3f( x, y, z );
}



void _wrap_glVertex3fv(boost::python::list py_v)
{
	GLfloat v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex3fv( v );
}



void _wrap_glVertex3i(GLint x, GLint y, GLint z)
{
	glVertex3i( x, y, z );
}



void _wrap_glVertex3iv(boost::python::list py_v)
{
	GLint v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex3iv( v );
}



void _wrap_glVertex3s(GLshort x, GLshort y, GLshort z)
{
	glVertex3s( x, y, z );
}



void _wrap_glVertex3sv(boost::python::list py_v)
{
	GLshort v[3];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 3 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex3sv( v );
}



void _wrap_glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	glVertex4d( x, y, z, w );
}



void _wrap_glVertex4dv(boost::python::list py_v)
{
	GLdouble v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLdouble> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex4dv( v );
}



void _wrap_glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	glVertex4f( x, y, z, w );
}



void _wrap_glVertex4fv(boost::python::list py_v)
{
	GLfloat v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLfloat> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex4fv( v );
}



void _wrap_glVertex4i(GLint x, GLint y, GLint z, GLint w)
{
	glVertex4i( x, y, z, w );
}



void _wrap_glVertex4iv(boost::python::list py_v)
{
	GLint v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLint> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex4iv( v );
}



void _wrap_glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
	glVertex4s( x, y, z, w );
}



void _wrap_glVertex4sv(boost::python::list py_v)
{
	GLshort v[4];   // type   name   size
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int len = v_lenExtract;   // name
		len = std::min( len, 4 );
		for (int i = 0; i < len; i++)   // size
		{
			boost::python::object itemObj = py_v[i];     // list_name
			boost::python::extract<GLshort> itemExtract( itemObj );     // type
			if ( itemExtract.check() )
			{
				v[i] = itemExtract;    // name
			}
		}
	}
	
	glVertex4sv( v );
}



void _wrap_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	glViewport( x, y, width, height );
}




BOOST_PYTHON_MODULE(_GL)
{
	def( "glCallLists", _wrap_glCallLists );
	def( "glGenTextures", _wrap_glGenTextures );
	def( "glDeleteTextures", _wrap_glDeleteTextures );
	def( "glAccum", _wrap_glAccum );
	def( "glAlphaFunc", _wrap_glAlphaFunc );
	def( "glArrayElement", _wrap_glArrayElement );
	def( "glBegin", _wrap_glBegin );
	def( "glBindTexture", _wrap_glBindTexture );
	def( "glBlendFunc", _wrap_glBlendFunc );
	def( "glCallList", _wrap_glCallList );
	def( "glClear", _wrap_glClear );
	def( "glClearAccum", _wrap_glClearAccum );
	def( "glClearColor", _wrap_glClearColor );
	def( "glClearDepth", _wrap_glClearDepth );
	def( "glClearIndex", _wrap_glClearIndex );
	def( "glClearStencil", _wrap_glClearStencil );
	def( "glClipPlane", _wrap_glClipPlane );
	def( "glColor3b", _wrap_glColor3b );
	def( "glColor3bv", _wrap_glColor3bv );
	def( "glColor3d", _wrap_glColor3d );
	def( "glColor3dv", _wrap_glColor3dv );
	def( "glColor3f", _wrap_glColor3f );
	def( "glColor3fv", _wrap_glColor3fv );
	def( "glColor3i", _wrap_glColor3i );
	def( "glColor3iv", _wrap_glColor3iv );
	def( "glColor3s", _wrap_glColor3s );
	def( "glColor3sv", _wrap_glColor3sv );
	def( "glColor3ub", _wrap_glColor3ub );
	def( "glColor3ubv", _wrap_glColor3ubv );
	def( "glColor3ui", _wrap_glColor3ui );
	def( "glColor3uiv", _wrap_glColor3uiv );
	def( "glColor3us", _wrap_glColor3us );
	def( "glColor3usv", _wrap_glColor3usv );
	def( "glColor4b", _wrap_glColor4b );
	def( "glColor4bv", _wrap_glColor4bv );
	def( "glColor4d", _wrap_glColor4d );
	def( "glColor4dv", _wrap_glColor4dv );
	def( "glColor4f", _wrap_glColor4f );
	def( "glColor4fv", _wrap_glColor4fv );
	def( "glColor4i", _wrap_glColor4i );
	def( "glColor4iv", _wrap_glColor4iv );
	def( "glColor4s", _wrap_glColor4s );
	def( "glColor4sv", _wrap_glColor4sv );
	def( "glColor4ub", _wrap_glColor4ub );
	def( "glColor4ubv", _wrap_glColor4ubv );
	def( "glColor4ui", _wrap_glColor4ui );
	def( "glColor4uiv", _wrap_glColor4uiv );
	def( "glColor4us", _wrap_glColor4us );
	def( "glColor4usv", _wrap_glColor4usv );
	def( "glColorMask", _wrap_glColorMask );
	def( "glColorMaterial", _wrap_glColorMaterial );
	def( "glCopyPixels", _wrap_glCopyPixels );
	def( "glCopyTexImage1D", _wrap_glCopyTexImage1D );
	def( "glCopyTexImage2D", _wrap_glCopyTexImage2D );
	def( "glCopyTexSubImage1D", _wrap_glCopyTexSubImage1D );
	def( "glCopyTexSubImage2D", _wrap_glCopyTexSubImage2D );
	def( "glCullFace", _wrap_glCullFace );
	def( "glDeleteLists", _wrap_glDeleteLists );
	def( "glDepthFunc", _wrap_glDepthFunc );
	def( "glDepthMask", _wrap_glDepthMask );
	def( "glDepthRange", _wrap_glDepthRange );
	def( "glDisable", _wrap_glDisable );
	def( "glDisableClientState", _wrap_glDisableClientState );
	def( "glDrawArrays", _wrap_glDrawArrays );
	def( "glDrawBuffer", _wrap_glDrawBuffer );
	def( "glEdgeFlag", _wrap_glEdgeFlag );
	def( "glEdgeFlagv", _wrap_glEdgeFlagv );
	def( "glEnable", _wrap_glEnable );
	def( "glEnableClientState", _wrap_glEnableClientState );
	def( "glEnd", _wrap_glEnd );
	def( "glEndList", _wrap_glEndList );
	def( "glEvalCoord1d", _wrap_glEvalCoord1d );
	def( "glEvalCoord1dv", _wrap_glEvalCoord1dv );
	def( "glEvalCoord1f", _wrap_glEvalCoord1f );
	def( "glEvalCoord1fv", _wrap_glEvalCoord1fv );
	def( "glEvalCoord2d", _wrap_glEvalCoord2d );
	def( "glEvalCoord2dv", _wrap_glEvalCoord2dv );
	def( "glEvalCoord2f", _wrap_glEvalCoord2f );
	def( "glEvalCoord2fv", _wrap_glEvalCoord2fv );
	def( "glEvalMesh1", _wrap_glEvalMesh1 );
	def( "glEvalMesh2", _wrap_glEvalMesh2 );
	def( "glEvalPoint1", _wrap_glEvalPoint1 );
	def( "glEvalPoint2", _wrap_glEvalPoint2 );
	def( "glFinish", _wrap_glFinish );
	def( "glFlush", _wrap_glFlush );
	def( "glFogf", _wrap_glFogf );
	def( "glFogfv", _wrap_glFogfv );
	def( "glFogi", _wrap_glFogi );
	def( "glFogiv", _wrap_glFogiv );
	def( "glFrontFace", _wrap_glFrontFace );
	def( "glFrustum", _wrap_glFrustum );
	def( "glGenLists", _wrap_glGenLists );
	def( "glGetError", _wrap_glGetError );
	def( "glHint", _wrap_glHint );
	def( "glIndexMask", _wrap_glIndexMask );
	def( "glIndexd", _wrap_glIndexd );
	def( "glIndexdv", _wrap_glIndexdv );
	def( "glIndexf", _wrap_glIndexf );
	def( "glIndexfv", _wrap_glIndexfv );
	def( "glIndexi", _wrap_glIndexi );
	def( "glIndexiv", _wrap_glIndexiv );
	def( "glIndexs", _wrap_glIndexs );
	def( "glIndexsv", _wrap_glIndexsv );
	def( "glIndexub", _wrap_glIndexub );
	def( "glIndexubv", _wrap_glIndexubv );
	def( "glInitNames", _wrap_glInitNames );
	def( "glIsEnabled", _wrap_glIsEnabled );
	def( "glIsList", _wrap_glIsList );
	def( "glIsTexture", _wrap_glIsTexture );
	def( "glLightModelf", _wrap_glLightModelf );
	def( "glLightModelfv", _wrap_glLightModelfv );
	def( "glLightModeli", _wrap_glLightModeli );
	def( "glLightModeliv", _wrap_glLightModeliv );
	def( "glLightf", _wrap_glLightf );
	def( "glLightfv", _wrap_glLightfv );
	def( "glLighti", _wrap_glLighti );
	def( "glLightiv", _wrap_glLightiv );
	def( "glLineStipple", _wrap_glLineStipple );
	def( "glLineWidth", _wrap_glLineWidth );
	def( "glListBase", _wrap_glListBase );
	def( "glLoadIdentity", _wrap_glLoadIdentity );
	def( "glLoadMatrixd", _wrap_glLoadMatrixd );
	def( "glLoadMatrixf", _wrap_glLoadMatrixf );
	def( "glLoadName", _wrap_glLoadName );
	def( "glLogicOp", _wrap_glLogicOp );
	def( "glMapGrid1d", _wrap_glMapGrid1d );
	def( "glMapGrid1f", _wrap_glMapGrid1f );
	def( "glMapGrid2d", _wrap_glMapGrid2d );
	def( "glMapGrid2f", _wrap_glMapGrid2f );
	def( "glMaterialf", _wrap_glMaterialf );
	def( "glMaterialfv", _wrap_glMaterialfv );
	def( "glMateriali", _wrap_glMateriali );
	def( "glMaterialiv", _wrap_glMaterialiv );
	def( "glMatrixMode", _wrap_glMatrixMode );
	def( "glMultMatrixd", _wrap_glMultMatrixd );
	def( "glMultMatrixf", _wrap_glMultMatrixf );
	def( "glNewList", _wrap_glNewList );
	def( "glNormal3b", _wrap_glNormal3b );
	def( "glNormal3bv", _wrap_glNormal3bv );
	def( "glNormal3d", _wrap_glNormal3d );
	def( "glNormal3dv", _wrap_glNormal3dv );
	def( "glNormal3f", _wrap_glNormal3f );
	def( "glNormal3fv", _wrap_glNormal3fv );
	def( "glNormal3i", _wrap_glNormal3i );
	def( "glNormal3iv", _wrap_glNormal3iv );
	def( "glNormal3s", _wrap_glNormal3s );
	def( "glNormal3sv", _wrap_glNormal3sv );
	def( "glOrtho", _wrap_glOrtho );
	def( "glPassThrough", _wrap_glPassThrough );
	def( "glPixelMapfv", _wrap_glPixelMapfv );
	def( "glPixelMapuiv", _wrap_glPixelMapuiv );
	def( "glPixelMapusv", _wrap_glPixelMapusv );
	def( "glPixelStoref", _wrap_glPixelStoref );
	def( "glPixelStorei", _wrap_glPixelStorei );
	def( "glPixelTransferf", _wrap_glPixelTransferf );
	def( "glPixelTransferi", _wrap_glPixelTransferi );
	def( "glPixelZoom", _wrap_glPixelZoom );
	def( "glPointSize", _wrap_glPointSize );
	def( "glPolygonMode", _wrap_glPolygonMode );
	def( "glPolygonOffset", _wrap_glPolygonOffset );
	def( "glPopAttrib", _wrap_glPopAttrib );
	def( "glPopClientAttrib", _wrap_glPopClientAttrib );
	def( "glPopMatrix", _wrap_glPopMatrix );
	def( "glPopName", _wrap_glPopName );
	def( "glPushAttrib", _wrap_glPushAttrib );
	def( "glPushClientAttrib", _wrap_glPushClientAttrib );
	def( "glPushMatrix", _wrap_glPushMatrix );
	def( "glPushName", _wrap_glPushName );
	def( "glRasterPos2d", _wrap_glRasterPos2d );
	def( "glRasterPos2dv", _wrap_glRasterPos2dv );
	def( "glRasterPos2f", _wrap_glRasterPos2f );
	def( "glRasterPos2fv", _wrap_glRasterPos2fv );
	def( "glRasterPos2i", _wrap_glRasterPos2i );
	def( "glRasterPos2iv", _wrap_glRasterPos2iv );
	def( "glRasterPos2s", _wrap_glRasterPos2s );
	def( "glRasterPos2sv", _wrap_glRasterPos2sv );
	def( "glRasterPos3d", _wrap_glRasterPos3d );
	def( "glRasterPos3dv", _wrap_glRasterPos3dv );
	def( "glRasterPos3f", _wrap_glRasterPos3f );
	def( "glRasterPos3fv", _wrap_glRasterPos3fv );
	def( "glRasterPos3i", _wrap_glRasterPos3i );
	def( "glRasterPos3iv", _wrap_glRasterPos3iv );
	def( "glRasterPos3s", _wrap_glRasterPos3s );
	def( "glRasterPos3sv", _wrap_glRasterPos3sv );
	def( "glRasterPos4d", _wrap_glRasterPos4d );
	def( "glRasterPos4dv", _wrap_glRasterPos4dv );
	def( "glRasterPos4f", _wrap_glRasterPos4f );
	def( "glRasterPos4fv", _wrap_glRasterPos4fv );
	def( "glRasterPos4i", _wrap_glRasterPos4i );
	def( "glRasterPos4iv", _wrap_glRasterPos4iv );
	def( "glRasterPos4s", _wrap_glRasterPos4s );
	def( "glRasterPos4sv", _wrap_glRasterPos4sv );
	def( "glReadBuffer", _wrap_glReadBuffer );
	def( "glRectd", _wrap_glRectd );
	def( "glRectdv", _wrap_glRectdv );
	def( "glRectf", _wrap_glRectf );
	def( "glRectfv", _wrap_glRectfv );
	def( "glRecti", _wrap_glRecti );
	def( "glRectiv", _wrap_glRectiv );
	def( "glRects", _wrap_glRects );
	def( "glRectsv", _wrap_glRectsv );
	def( "glRenderMode", _wrap_glRenderMode );
	def( "glRotated", _wrap_glRotated );
	def( "glRotatef", _wrap_glRotatef );
	def( "glScaled", _wrap_glScaled );
	def( "glScalef", _wrap_glScalef );
	def( "glScissor", _wrap_glScissor );
	def( "glShadeModel", _wrap_glShadeModel );
	def( "glStencilFunc", _wrap_glStencilFunc );
	def( "glStencilMask", _wrap_glStencilMask );
	def( "glStencilOp", _wrap_glStencilOp );
	def( "glTexCoord1d", _wrap_glTexCoord1d );
	def( "glTexCoord1dv", _wrap_glTexCoord1dv );
	def( "glTexCoord1f", _wrap_glTexCoord1f );
	def( "glTexCoord1fv", _wrap_glTexCoord1fv );
	def( "glTexCoord1i", _wrap_glTexCoord1i );
	def( "glTexCoord1iv", _wrap_glTexCoord1iv );
	def( "glTexCoord1s", _wrap_glTexCoord1s );
	def( "glTexCoord1sv", _wrap_glTexCoord1sv );
	def( "glTexCoord2d", _wrap_glTexCoord2d );
	def( "glTexCoord2dv", _wrap_glTexCoord2dv );
	def( "glTexCoord2f", _wrap_glTexCoord2f );
	def( "glTexCoord2fv", _wrap_glTexCoord2fv );
	def( "glTexCoord2i", _wrap_glTexCoord2i );
	def( "glTexCoord2iv", _wrap_glTexCoord2iv );
	def( "glTexCoord2s", _wrap_glTexCoord2s );
	def( "glTexCoord2sv", _wrap_glTexCoord2sv );
	def( "glTexCoord3d", _wrap_glTexCoord3d );
	def( "glTexCoord3dv", _wrap_glTexCoord3dv );
	def( "glTexCoord3f", _wrap_glTexCoord3f );
	def( "glTexCoord3fv", _wrap_glTexCoord3fv );
	def( "glTexCoord3i", _wrap_glTexCoord3i );
	def( "glTexCoord3iv", _wrap_glTexCoord3iv );
	def( "glTexCoord3s", _wrap_glTexCoord3s );
	def( "glTexCoord3sv", _wrap_glTexCoord3sv );
	def( "glTexCoord4d", _wrap_glTexCoord4d );
	def( "glTexCoord4dv", _wrap_glTexCoord4dv );
	def( "glTexCoord4f", _wrap_glTexCoord4f );
	def( "glTexCoord4fv", _wrap_glTexCoord4fv );
	def( "glTexCoord4i", _wrap_glTexCoord4i );
	def( "glTexCoord4iv", _wrap_glTexCoord4iv );
	def( "glTexCoord4s", _wrap_glTexCoord4s );
	def( "glTexCoord4sv", _wrap_glTexCoord4sv );
	def( "glTexEnvf", _wrap_glTexEnvf );
	def( "glTexEnvfv", _wrap_glTexEnvfv );
	def( "glTexEnvi", _wrap_glTexEnvi );
	def( "glTexEnviv", _wrap_glTexEnviv );
	def( "glTexGend", _wrap_glTexGend );
	def( "glTexGendv", _wrap_glTexGendv );
	def( "glTexGenf", _wrap_glTexGenf );
	def( "glTexGenfv", _wrap_glTexGenfv );
	def( "glTexGeni", _wrap_glTexGeni );
	def( "glTexGeniv", _wrap_glTexGeniv );
	def( "glTexParameterf", _wrap_glTexParameterf );
	def( "glTexParameterfv", _wrap_glTexParameterfv );
	def( "glTexParameteri", _wrap_glTexParameteri );
	def( "glTexParameteriv", _wrap_glTexParameteriv );
	def( "glTranslated", _wrap_glTranslated );
	def( "glTranslatef", _wrap_glTranslatef );
	def( "glVertex2d", _wrap_glVertex2d );
	def( "glVertex2dv", _wrap_glVertex2dv );
	def( "glVertex2f", _wrap_glVertex2f );
	def( "glVertex2fv", _wrap_glVertex2fv );
	def( "glVertex2i", _wrap_glVertex2i );
	def( "glVertex2iv", _wrap_glVertex2iv );
	def( "glVertex2s", _wrap_glVertex2s );
	def( "glVertex2sv", _wrap_glVertex2sv );
	def( "glVertex3d", _wrap_glVertex3d );
	def( "glVertex3dv", _wrap_glVertex3dv );
	def( "glVertex3f", _wrap_glVertex3f );
	def( "glVertex3fv", _wrap_glVertex3fv );
	def( "glVertex3i", _wrap_glVertex3i );
	def( "glVertex3iv", _wrap_glVertex3iv );
	def( "glVertex3s", _wrap_glVertex3s );
	def( "glVertex3sv", _wrap_glVertex3sv );
	def( "glVertex4d", _wrap_glVertex4d );
	def( "glVertex4dv", _wrap_glVertex4dv );
	def( "glVertex4f", _wrap_glVertex4f );
	def( "glVertex4fv", _wrap_glVertex4fv );
	def( "glVertex4i", _wrap_glVertex4i );
	def( "glVertex4iv", _wrap_glVertex4iv );
	def( "glVertex4s", _wrap_glVertex4s );
	def( "glVertex4sv", _wrap_glVertex4sv );
	def( "glViewport", _wrap_glViewport );
}



//
//
// GENERATED CODE; DO NOT EDIT
//
//


#endif
