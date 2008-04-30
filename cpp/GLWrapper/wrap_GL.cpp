//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GL_CPP
#define GL_CPP

#include <boost/python.hpp>
using namespace boost::python;


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
			boost::python::extract<GLuint&> texExtract( texObj );     // type
			if ( texExtract.check() )
			{
				textures[i] = texExtract;
			}
		}
		glDeleteTextures( numTextures, textures.begin() );
	}
}


void _wrap_glClipPlane(GLenum plane, boost::python::list py_equation)
{
	GLdouble equation[4];   // type   name   size
	boost::python::object equation_lenObj = py_equation.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> equation_lenExtract( equation_lenObj );   // name   name
	if ( equation_lenExtract.check() )    // name
	{
		int numTargets = equation_lenExtract;   // name
		if ( numTargets == 4 )   // size
		{
			for (int i = 0; i < 4; i++)   // size
			{
				boost::python::object targetObj = py_equation[i];     // list_name
				boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
				if ( targetExtract.check() )
				{
					equation[i] = targetExtract;    // name
				}
			}
		}
	}
	
	glClipPlane( plane, equation );
}



void _wrap_glColor3bv(boost::python::list py_v)
{
	Array<GLbyte> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLbyte&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor3bv( v.begin() );
}



void _wrap_glColor3dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor3dv( v.begin() );
}



void _wrap_glColor3fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor3fv( v.begin() );
}



void _wrap_glColor3iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor3iv( v.begin() );
}



void _wrap_glColor3sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor3sv( v.begin() );
}



void _wrap_glColor3ubv(boost::python::list py_v)
{
	Array<GLubyte> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLubyte&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor3ubv( v.begin() );
}



void _wrap_glColor3uiv(boost::python::list py_v)
{
	Array<GLuint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLuint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor3uiv( v.begin() );
}



void _wrap_glColor3usv(boost::python::list py_v)
{
	Array<GLushort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLushort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor3usv( v.begin() );
}



void _wrap_glColor4bv(boost::python::list py_v)
{
	Array<GLbyte> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLbyte&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor4bv( v.begin() );
}



void _wrap_glColor4dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor4dv( v.begin() );
}



void _wrap_glColor4fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor4fv( v.begin() );
}



void _wrap_glColor4iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor4iv( v.begin() );
}



void _wrap_glColor4sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor4sv( v.begin() );
}



void _wrap_glColor4ubv(boost::python::list py_v)
{
	Array<GLubyte> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLubyte&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor4ubv( v.begin() );
}



void _wrap_glColor4uiv(boost::python::list py_v)
{
	Array<GLuint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLuint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor4uiv( v.begin() );
}



void _wrap_glColor4usv(boost::python::list py_v)
{
	Array<GLushort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLushort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glColor4usv( v.begin() );
}



void _wrap_glEdgeFlagv(boost::python::list py_flag)
{
	Array<GLboolean> flag;   // type   name
	boost::python::object flag_lenObj = py_flag.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> flag_lenExtract( flag_lenObj );   // name   name
	if ( flag_lenExtract.check() )    // name
	{
		int numTargets = flag_lenExtract;   // name
		flag.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_flag[i];     // list_name
			boost::python::extract<GLboolean&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				flag.push_back( targetExtract );    // name
			}
		}
	}
	
	glEdgeFlagv( flag.begin() );
}



void _wrap_glEvalCoord1dv(boost::python::list py_u)
{
	Array<GLdouble> u;   // type   name
	boost::python::object u_lenObj = py_u.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> u_lenExtract( u_lenObj );   // name   name
	if ( u_lenExtract.check() )    // name
	{
		int numTargets = u_lenExtract;   // name
		u.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_u[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				u.push_back( targetExtract );    // name
			}
		}
	}
	
	glEvalCoord1dv( u.begin() );
}



void _wrap_glEvalCoord1fv(boost::python::list py_u)
{
	Array<GLfloat> u;   // type   name
	boost::python::object u_lenObj = py_u.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> u_lenExtract( u_lenObj );   // name   name
	if ( u_lenExtract.check() )    // name
	{
		int numTargets = u_lenExtract;   // name
		u.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_u[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				u.push_back( targetExtract );    // name
			}
		}
	}
	
	glEvalCoord1fv( u.begin() );
}



void _wrap_glEvalCoord2dv(boost::python::list py_u)
{
	Array<GLdouble> u;   // type   name
	boost::python::object u_lenObj = py_u.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> u_lenExtract( u_lenObj );   // name   name
	if ( u_lenExtract.check() )    // name
	{
		int numTargets = u_lenExtract;   // name
		u.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_u[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				u.push_back( targetExtract );    // name
			}
		}
	}
	
	glEvalCoord2dv( u.begin() );
}



void _wrap_glEvalCoord2fv(boost::python::list py_u)
{
	Array<GLfloat> u;   // type   name
	boost::python::object u_lenObj = py_u.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> u_lenExtract( u_lenObj );   // name   name
	if ( u_lenExtract.check() )    // name
	{
		int numTargets = u_lenExtract;   // name
		u.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_u[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				u.push_back( targetExtract );    // name
			}
		}
	}
	
	glEvalCoord2fv( u.begin() );
}



void _wrap_glFogfv(GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glFogfv( pname, params.begin() );
}



void _wrap_glFogiv(GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glFogiv( pname, params.begin() );
}



void _wrap_glIndexdv(boost::python::list py_c)
{
	Array<GLdouble> c;   // type   name
	boost::python::object c_lenObj = py_c.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> c_lenExtract( c_lenObj );   // name   name
	if ( c_lenExtract.check() )    // name
	{
		int numTargets = c_lenExtract;   // name
		c.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_c[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				c.push_back( targetExtract );    // name
			}
		}
	}
	
	glIndexdv( c.begin() );
}



void _wrap_glIndexfv(boost::python::list py_c)
{
	Array<GLfloat> c;   // type   name
	boost::python::object c_lenObj = py_c.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> c_lenExtract( c_lenObj );   // name   name
	if ( c_lenExtract.check() )    // name
	{
		int numTargets = c_lenExtract;   // name
		c.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_c[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				c.push_back( targetExtract );    // name
			}
		}
	}
	
	glIndexfv( c.begin() );
}



void _wrap_glIndexiv(boost::python::list py_c)
{
	Array<GLint> c;   // type   name
	boost::python::object c_lenObj = py_c.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> c_lenExtract( c_lenObj );   // name   name
	if ( c_lenExtract.check() )    // name
	{
		int numTargets = c_lenExtract;   // name
		c.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_c[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				c.push_back( targetExtract );    // name
			}
		}
	}
	
	glIndexiv( c.begin() );
}



void _wrap_glIndexsv(boost::python::list py_c)
{
	Array<GLshort> c;   // type   name
	boost::python::object c_lenObj = py_c.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> c_lenExtract( c_lenObj );   // name   name
	if ( c_lenExtract.check() )    // name
	{
		int numTargets = c_lenExtract;   // name
		c.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_c[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				c.push_back( targetExtract );    // name
			}
		}
	}
	
	glIndexsv( c.begin() );
}



void _wrap_glIndexubv(boost::python::list py_c)
{
	Array<GLubyte> c;   // type   name
	boost::python::object c_lenObj = py_c.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> c_lenExtract( c_lenObj );   // name   name
	if ( c_lenExtract.check() )    // name
	{
		int numTargets = c_lenExtract;   // name
		c.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_c[i];     // list_name
			boost::python::extract<GLubyte&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				c.push_back( targetExtract );    // name
			}
		}
	}
	
	glIndexubv( c.begin() );
}



void _wrap_glLightModelfv(GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glLightModelfv( pname, params.begin() );
}



void _wrap_glLightModeliv(GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glLightModeliv( pname, params.begin() );
}



void _wrap_glLightfv(GLenum light, GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glLightfv( light, pname, params.begin() );
}



void _wrap_glLightiv(GLenum light, GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glLightiv( light, pname, params.begin() );
}



void _wrap_glMaterialfv(GLenum face, GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glMaterialfv( face, pname, params.begin() );
}



void _wrap_glMaterialiv(GLenum face, GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glMaterialiv( face, pname, params.begin() );
}



void _wrap_glMultMatrixd(boost::python::list py_m)
{
	GLdouble m[16];   // type   name   size
	boost::python::object m_lenObj = py_m.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> m_lenExtract( m_lenObj );   // name   name
	if ( m_lenExtract.check() )    // name
	{
		int numTargets = m_lenExtract;   // name
		if ( numTargets == 16 )   // size
		{
			for (int i = 0; i < 16; i++)   // size
			{
				boost::python::object targetObj = py_m[i];     // list_name
				boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
				if ( targetExtract.check() )
				{
					m[i] = targetExtract;    // name
				}
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
		int numTargets = m_lenExtract;   // name
		if ( numTargets == 16 )   // size
		{
			for (int i = 0; i < 16; i++)   // size
			{
				boost::python::object targetObj = py_m[i];     // list_name
				boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
				if ( targetExtract.check() )
				{
					m[i] = targetExtract;    // name
				}
			}
		}
	}
	
	glMultMatrixf( m );
}



void _wrap_glNormal3bv(boost::python::list py_v)
{
	Array<GLbyte> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLbyte&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glNormal3bv( v.begin() );
}



void _wrap_glNormal3dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glNormal3dv( v.begin() );
}



void _wrap_glNormal3fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glNormal3fv( v.begin() );
}



void _wrap_glNormal3iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glNormal3iv( v.begin() );
}



void _wrap_glNormal3sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glNormal3sv( v.begin() );
}



void _wrap_glPixelMapfv(GLenum map, GLsizei mapsize, boost::python::list py_values)
{
	Array<GLfloat> values;   // type   name
	boost::python::object values_lenObj = py_values.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> values_lenExtract( values_lenObj );   // name   name
	if ( values_lenExtract.check() )    // name
	{
		int numTargets = values_lenExtract;   // name
		values.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_values[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				values.push_back( targetExtract );    // name
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
		int numTargets = values_lenExtract;   // name
		values.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_values[i];     // list_name
			boost::python::extract<GLuint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				values.push_back( targetExtract );    // name
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
		int numTargets = values_lenExtract;   // name
		values.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_values[i];     // list_name
			boost::python::extract<GLushort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				values.push_back( targetExtract );    // name
			}
		}
	}
	
	glPixelMapusv( map, mapsize, values.begin() );
}



void _wrap_glRasterPos2dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos2dv( v.begin() );
}



void _wrap_glRasterPos2fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos2fv( v.begin() );
}



void _wrap_glRasterPos2iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos2iv( v.begin() );
}



void _wrap_glRasterPos2sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos2sv( v.begin() );
}



void _wrap_glRasterPos3dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos3dv( v.begin() );
}



void _wrap_glRasterPos3fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos3fv( v.begin() );
}



void _wrap_glRasterPos3iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos3iv( v.begin() );
}



void _wrap_glRasterPos3sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos3sv( v.begin() );
}



void _wrap_glRasterPos4dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos4dv( v.begin() );
}



void _wrap_glRasterPos4fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos4fv( v.begin() );
}



void _wrap_glRasterPos4iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos4iv( v.begin() );
}



void _wrap_glRasterPos4sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glRasterPos4sv( v.begin() );
}



void _wrap_glRectdv(boost::python::list py_v1, boost::python::list py_v2)
{
	GLdouble v1[2];   // type   name   size
	boost::python::object v1_lenObj = py_v1.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v1_lenExtract( v1_lenObj );   // name   name
	if ( v1_lenExtract.check() )    // name
	{
		int numTargets = v1_lenExtract;   // name
		if ( numTargets == 2 )   // size
		{
			for (int i = 0; i < 2; i++)   // size
			{
				boost::python::object targetObj = py_v1[i];     // list_name
				boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
				if ( targetExtract.check() )
				{
					v1[i] = targetExtract;    // name
				}
			}
		}
	}
	
	GLdouble v2[2];   // type   name   size
	boost::python::object v2_lenObj = py_v2.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v2_lenExtract( v2_lenObj );   // name   name
	if ( v2_lenExtract.check() )    // name
	{
		int numTargets = v2_lenExtract;   // name
		if ( numTargets == 2 )   // size
		{
			for (int i = 0; i < 2; i++)   // size
			{
				boost::python::object targetObj = py_v2[i];     // list_name
				boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
				if ( targetExtract.check() )
				{
					v2[i] = targetExtract;    // name
				}
			}
		}
	}
	
	glRectdv( v1, v2 );
}



void _wrap_glRectfv(boost::python::list py_v1, boost::python::list py_v2)
{
	GLfloat v1[2];   // type   name   size
	boost::python::object v1_lenObj = py_v1.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v1_lenExtract( v1_lenObj );   // name   name
	if ( v1_lenExtract.check() )    // name
	{
		int numTargets = v1_lenExtract;   // name
		if ( numTargets == 2 )   // size
		{
			for (int i = 0; i < 2; i++)   // size
			{
				boost::python::object targetObj = py_v1[i];     // list_name
				boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
				if ( targetExtract.check() )
				{
					v1[i] = targetExtract;    // name
				}
			}
		}
	}
	
	GLfloat v2[2];   // type   name   size
	boost::python::object v2_lenObj = py_v2.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v2_lenExtract( v2_lenObj );   // name   name
	if ( v2_lenExtract.check() )    // name
	{
		int numTargets = v2_lenExtract;   // name
		if ( numTargets == 2 )   // size
		{
			for (int i = 0; i < 2; i++)   // size
			{
				boost::python::object targetObj = py_v2[i];     // list_name
				boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
				if ( targetExtract.check() )
				{
					v2[i] = targetExtract;    // name
				}
			}
		}
	}
	
	glRectfv( v1, v2 );
}



void _wrap_glRectiv(boost::python::list py_v1, boost::python::list py_v2)
{
	GLint v1[2];   // type   name   size
	boost::python::object v1_lenObj = py_v1.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v1_lenExtract( v1_lenObj );   // name   name
	if ( v1_lenExtract.check() )    // name
	{
		int numTargets = v1_lenExtract;   // name
		if ( numTargets == 2 )   // size
		{
			for (int i = 0; i < 2; i++)   // size
			{
				boost::python::object targetObj = py_v1[i];     // list_name
				boost::python::extract<GLint&> targetExtract( targetObj );     // type
				if ( targetExtract.check() )
				{
					v1[i] = targetExtract;    // name
				}
			}
		}
	}
	
	GLint v2[2];   // type   name   size
	boost::python::object v2_lenObj = py_v2.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v2_lenExtract( v2_lenObj );   // name   name
	if ( v2_lenExtract.check() )    // name
	{
		int numTargets = v2_lenExtract;   // name
		if ( numTargets == 2 )   // size
		{
			for (int i = 0; i < 2; i++)   // size
			{
				boost::python::object targetObj = py_v2[i];     // list_name
				boost::python::extract<GLint&> targetExtract( targetObj );     // type
				if ( targetExtract.check() )
				{
					v2[i] = targetExtract;    // name
				}
			}
		}
	}
	
	glRectiv( v1, v2 );
}



void _wrap_glRectsv(boost::python::list py_v1, boost::python::list py_v2)
{
	GLshort v1[2];   // type   name   size
	boost::python::object v1_lenObj = py_v1.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v1_lenExtract( v1_lenObj );   // name   name
	if ( v1_lenExtract.check() )    // name
	{
		int numTargets = v1_lenExtract;   // name
		if ( numTargets == 2 )   // size
		{
			for (int i = 0; i < 2; i++)   // size
			{
				boost::python::object targetObj = py_v1[i];     // list_name
				boost::python::extract<GLshort&> targetExtract( targetObj );     // type
				if ( targetExtract.check() )
				{
					v1[i] = targetExtract;    // name
				}
			}
		}
	}
	
	GLshort v2[2];   // type   name   size
	boost::python::object v2_lenObj = py_v2.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v2_lenExtract( v2_lenObj );   // name   name
	if ( v2_lenExtract.check() )    // name
	{
		int numTargets = v2_lenExtract;   // name
		if ( numTargets == 2 )   // size
		{
			for (int i = 0; i < 2; i++)   // size
			{
				boost::python::object targetObj = py_v2[i];     // list_name
				boost::python::extract<GLshort&> targetExtract( targetObj );     // type
				if ( targetExtract.check() )
				{
					v2[i] = targetExtract;    // name
				}
			}
		}
	}
	
	glRectsv( v1, v2 );
}



void _wrap_glTexCoord1dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord1dv( v.begin() );
}



void _wrap_glTexCoord1fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord1fv( v.begin() );
}



void _wrap_glTexCoord1iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord1iv( v.begin() );
}



void _wrap_glTexCoord1sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord1sv( v.begin() );
}



void _wrap_glTexCoord2dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord2dv( v.begin() );
}



void _wrap_glTexCoord2fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord2fv( v.begin() );
}



void _wrap_glTexCoord2iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord2iv( v.begin() );
}



void _wrap_glTexCoord2sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord2sv( v.begin() );
}



void _wrap_glTexCoord3dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord3dv( v.begin() );
}



void _wrap_glTexCoord3fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord3fv( v.begin() );
}



void _wrap_glTexCoord3iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord3iv( v.begin() );
}



void _wrap_glTexCoord3sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord3sv( v.begin() );
}



void _wrap_glTexCoord4dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord4dv( v.begin() );
}



void _wrap_glTexCoord4fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord4fv( v.begin() );
}



void _wrap_glTexCoord4iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord4iv( v.begin() );
}



void _wrap_glTexCoord4sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexCoord4sv( v.begin() );
}



void _wrap_glTexEnvfv(GLenum target, GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexEnvfv( target, pname, params.begin() );
}



void _wrap_glTexEnviv(GLenum target, GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexEnviv( target, pname, params.begin() );
}



void _wrap_glTexGendv(GLenum coord, GLenum pname, boost::python::list py_params)
{
	Array<GLdouble> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexGendv( coord, pname, params.begin() );
}



void _wrap_glTexGenfv(GLenum coord, GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexGenfv( coord, pname, params.begin() );
}



void _wrap_glTexGeniv(GLenum coord, GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexGeniv( coord, pname, params.begin() );
}



void _wrap_glTexParameterfv(GLenum target, GLenum pname, boost::python::list py_params)
{
	Array<GLfloat> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexParameterfv( target, pname, params.begin() );
}



void _wrap_glTexParameteriv(GLenum target, GLenum pname, boost::python::list py_params)
{
	Array<GLint> params;   // type   name
	boost::python::object params_lenObj = py_params.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> params_lenExtract( params_lenObj );   // name   name
	if ( params_lenExtract.check() )    // name
	{
		int numTargets = params_lenExtract;   // name
		params.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_params[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				params.push_back( targetExtract );    // name
			}
		}
	}
	
	glTexParameteriv( target, pname, params.begin() );
}



void _wrap_glVertex2dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex2dv( v.begin() );
}



void _wrap_glVertex2fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex2fv( v.begin() );
}



void _wrap_glVertex2iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex2iv( v.begin() );
}



void _wrap_glVertex2sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex2sv( v.begin() );
}



void _wrap_glVertex3dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex3dv( v.begin() );
}



void _wrap_glVertex3fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex3fv( v.begin() );
}



void _wrap_glVertex3iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex3iv( v.begin() );
}



void _wrap_glVertex3sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex3sv( v.begin() );
}



void _wrap_glVertex4dv(boost::python::list py_v)
{
	Array<GLdouble> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLdouble&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex4dv( v.begin() );
}



void _wrap_glVertex4fv(boost::python::list py_v)
{
	Array<GLfloat> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLfloat&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex4fv( v.begin() );
}



void _wrap_glVertex4iv(boost::python::list py_v)
{
	Array<GLint> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLint&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex4iv( v.begin() );
}



void _wrap_glVertex4sv(boost::python::list py_v)
{
	Array<GLshort> v;   // type   name
	boost::python::object v_lenObj = py_v.attr( "__len__" )();     // name   list_name
	boost::python::extract<int> v_lenExtract( v_lenObj );   // name   name
	if ( v_lenExtract.check() )    // name
	{
		int numTargets = v_lenExtract;   // name
		v.reserve( numTargets );   // name
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = py_v[i];     // list_name
			boost::python::extract<GLshort&> targetExtract( targetObj );     // type
			if ( targetExtract.check() )
			{
				v.push_back( targetExtract );    // name
			}
		}
	}
	
	glVertex4sv( v.begin() );
}




BOOST_PYTHON_MODULE(GL)
{
	def( "glCallLists", _wrap_glCallLists );
	def( "glGenTextures", _wrap_glGenTextures );
	def( "glDeleteTextures", _wrap_glDeleteTextures );
	def( "glAccum", &glAccum );
	//def( "glAlphaFunc", glAlphaFunc );
	//def( "glArrayElement", glArrayElement );
	//def( "glBegin", glBegin );
	//def( "glBindTexture", glBindTexture );
	//def( "glBlendFunc", glBlendFunc );
	//def( "glCallList", glCallList );
	//def( "glClear", glClear );
	//def( "glClearAccum", glClearAccum );
	//def( "glClearColor", glClearColor );
	//def( "glClearDepth", glClearDepth );
	//def( "glClearIndex", glClearIndex );
	//def( "glClearStencil", glClearStencil );
	//def( "glClipPlane", _wrap_glClipPlane );
	//def( "glColor3b", glColor3b );
	//def( "glColor3bv", _wrap_glColor3bv );
	//def( "glColor3d", glColor3d );
	//def( "glColor3dv", _wrap_glColor3dv );
	//def( "glColor3f", glColor3f );
	//def( "glColor3fv", _wrap_glColor3fv );
	//def( "glColor3i", glColor3i );
	//def( "glColor3iv", _wrap_glColor3iv );
	//def( "glColor3s", glColor3s );
	//def( "glColor3sv", _wrap_glColor3sv );
	//def( "glColor3ub", glColor3ub );
	//def( "glColor3ubv", _wrap_glColor3ubv );
	//def( "glColor3ui", glColor3ui );
	//def( "glColor3uiv", _wrap_glColor3uiv );
	//def( "glColor3us", glColor3us );
	//def( "glColor3usv", _wrap_glColor3usv );
	//def( "glColor4b", glColor4b );
	//def( "glColor4bv", _wrap_glColor4bv );
	//def( "glColor4d", glColor4d );
	//def( "glColor4dv", _wrap_glColor4dv );
	//def( "glColor4f", glColor4f );
	//def( "glColor4fv", _wrap_glColor4fv );
	//def( "glColor4i", glColor4i );
	//def( "glColor4iv", _wrap_glColor4iv );
	//def( "glColor4s", glColor4s );
	//def( "glColor4sv", _wrap_glColor4sv );
	//def( "glColor4ub", glColor4ub );
	//def( "glColor4ubv", _wrap_glColor4ubv );
	//def( "glColor4ui", glColor4ui );
	//def( "glColor4uiv", _wrap_glColor4uiv );
	//def( "glColor4us", glColor4us );
	//def( "glColor4usv", _wrap_glColor4usv );
	//def( "glColorMask", glColorMask );
	//def( "glColorMaterial", glColorMaterial );
	//def( "glCopyPixels", glCopyPixels );
	//def( "glCopyTexImage1D", glCopyTexImage1D );
	//def( "glCopyTexImage2D", glCopyTexImage2D );
	//def( "glCopyTexSubImage1D", glCopyTexSubImage1D );
	//def( "glCopyTexSubImage2D", glCopyTexSubImage2D );
	//def( "glCullFace", glCullFace );
	//def( "glDeleteLists", glDeleteLists );
	//def( "glDepthFunc", glDepthFunc );
	//def( "glDepthMask", glDepthMask );
	//def( "glDepthRange", glDepthRange );
	//def( "glDisable", glDisable );
	//def( "glDisableClientState", glDisableClientState );
	//def( "glDrawArrays", glDrawArrays );
	//def( "glDrawBuffer", glDrawBuffer );
	//def( "glEdgeFlag", glEdgeFlag );
	//def( "glEdgeFlagv", _wrap_glEdgeFlagv );
	//def( "glEnable", glEnable );
	//def( "glEnableClientState", glEnableClientState );
	//def( "glEvalCoord1d", glEvalCoord1d );
	//def( "glEvalCoord1dv", _wrap_glEvalCoord1dv );
	//def( "glEvalCoord1f", glEvalCoord1f );
	//def( "glEvalCoord1fv", _wrap_glEvalCoord1fv );
	//def( "glEvalCoord2d", glEvalCoord2d );
	//def( "glEvalCoord2dv", _wrap_glEvalCoord2dv );
	//def( "glEvalCoord2f", glEvalCoord2f );
	//def( "glEvalCoord2fv", _wrap_glEvalCoord2fv );
	//def( "glEvalMesh1", glEvalMesh1 );
	//def( "glEvalMesh2", glEvalMesh2 );
	//def( "glEvalPoint1", glEvalPoint1 );
	//def( "glEvalPoint2", glEvalPoint2 );
	//def( "glFogf", glFogf );
	//def( "glFogfv", _wrap_glFogfv );
	//def( "glFogi", glFogi );
	//def( "glFogiv", _wrap_glFogiv );
	//def( "glFrontFace", glFrontFace );
	//def( "glFrustum", glFrustum );
	//def( "glGenLists", glGenLists );
	//def( "glHint", glHint );
	//def( "glIndexMask", glIndexMask );
	//def( "glIndexd", glIndexd );
	//def( "glIndexdv", _wrap_glIndexdv );
	//def( "glIndexf", glIndexf );
	//def( "glIndexfv", _wrap_glIndexfv );
	//def( "glIndexi", glIndexi );
	//def( "glIndexiv", _wrap_glIndexiv );
	//def( "glIndexs", glIndexs );
	//def( "glIndexsv", _wrap_glIndexsv );
	//def( "glIndexub", glIndexub );
	//def( "glIndexubv", _wrap_glIndexubv );
	//def( "glIsEnabled", glIsEnabled );
	//def( "glIsList", glIsList );
	//def( "glIsTexture", glIsTexture );
	//def( "glLightModelf", glLightModelf );
	//def( "glLightModelfv", _wrap_glLightModelfv );
	//def( "glLightModeli", glLightModeli );
	//def( "glLightModeliv", _wrap_glLightModeliv );
	//def( "glLightf", glLightf );
	//def( "glLightfv", _wrap_glLightfv );
	//def( "glLighti", glLighti );
	//def( "glLightiv", _wrap_glLightiv );
	//def( "glLineStipple", glLineStipple );
	//def( "glLineWidth", glLineWidth );
	//def( "glListBase", glListBase );
	//def( "glLoadName", glLoadName );
	//def( "glLogicOp", glLogicOp );
	//def( "glMapGrid1d", glMapGrid1d );
	//def( "glMapGrid1f", glMapGrid1f );
	//def( "glMapGrid2d", glMapGrid2d );
	//def( "glMapGrid2f", glMapGrid2f );
	//def( "glMaterialf", glMaterialf );
	//def( "glMaterialfv", _wrap_glMaterialfv );
	//def( "glMateriali", glMateriali );
	//def( "glMaterialiv", _wrap_glMaterialiv );
	//def( "glMatrixMode", glMatrixMode );
	//def( "glMultMatrixd", _wrap_glMultMatrixd );
	//def( "glMultMatrixf", _wrap_glMultMatrixf );
	//def( "glNewList", glNewList );
	//def( "glNormal3b", glNormal3b );
	//def( "glNormal3bv", _wrap_glNormal3bv );
	//def( "glNormal3d", glNormal3d );
	//def( "glNormal3dv", _wrap_glNormal3dv );
	//def( "glNormal3f", glNormal3f );
	//def( "glNormal3fv", _wrap_glNormal3fv );
	//def( "glNormal3i", glNormal3i );
	//def( "glNormal3iv", _wrap_glNormal3iv );
	//def( "glNormal3s", glNormal3s );
	//def( "glNormal3sv", _wrap_glNormal3sv );
	//def( "glOrtho", glOrtho );
	//def( "glPassThrough", glPassThrough );
	//def( "glPixelMapfv", _wrap_glPixelMapfv );
	//def( "glPixelMapuiv", _wrap_glPixelMapuiv );
	//def( "glPixelMapusv", _wrap_glPixelMapusv );
	//def( "glPixelStoref", glPixelStoref );
	//def( "glPixelStorei", glPixelStorei );
	//def( "glPixelTransferf", glPixelTransferf );
	//def( "glPixelTransferi", glPixelTransferi );
	//def( "glPixelZoom", glPixelZoom );
	//def( "glPointSize", glPointSize );
	//def( "glPolygonMode", glPolygonMode );
	//def( "glPolygonOffset", glPolygonOffset );
	//def( "glPushAttrib", glPushAttrib );
	//def( "glPushClientAttrib", glPushClientAttrib );
	//def( "glPushName", glPushName );
	//def( "glRasterPos2d", glRasterPos2d );
	//def( "glRasterPos2dv", _wrap_glRasterPos2dv );
	//def( "glRasterPos2f", glRasterPos2f );
	//def( "glRasterPos2fv", _wrap_glRasterPos2fv );
	//def( "glRasterPos2i", glRasterPos2i );
	//def( "glRasterPos2iv", _wrap_glRasterPos2iv );
	//def( "glRasterPos2s", glRasterPos2s );
	//def( "glRasterPos2sv", _wrap_glRasterPos2sv );
	//def( "glRasterPos3d", glRasterPos3d );
	//def( "glRasterPos3dv", _wrap_glRasterPos3dv );
	//def( "glRasterPos3f", glRasterPos3f );
	//def( "glRasterPos3fv", _wrap_glRasterPos3fv );
	//def( "glRasterPos3i", glRasterPos3i );
	//def( "glRasterPos3iv", _wrap_glRasterPos3iv );
	//def( "glRasterPos3s", glRasterPos3s );
	//def( "glRasterPos3sv", _wrap_glRasterPos3sv );
	//def( "glRasterPos4d", glRasterPos4d );
	//def( "glRasterPos4dv", _wrap_glRasterPos4dv );
	//def( "glRasterPos4f", glRasterPos4f );
	//def( "glRasterPos4fv", _wrap_glRasterPos4fv );
	//def( "glRasterPos4i", glRasterPos4i );
	//def( "glRasterPos4iv", _wrap_glRasterPos4iv );
	//def( "glRasterPos4s", glRasterPos4s );
	//def( "glRasterPos4sv", _wrap_glRasterPos4sv );
	//def( "glReadBuffer", glReadBuffer );
	//def( "glRectd", glRectd );
	//def( "glRectdv", _wrap_glRectdv );
	//def( "glRectf", glRectf );
	//def( "glRectfv", _wrap_glRectfv );
	//def( "glRecti", glRecti );
	//def( "glRectiv", _wrap_glRectiv );
	//def( "glRects", glRects );
	//def( "glRectsv", _wrap_glRectsv );
	//def( "glRenderMode", glRenderMode );
	//def( "glRotated", glRotated );
	//def( "glRotatef", glRotatef );
	//def( "glScaled", glScaled );
	//def( "glScalef", glScalef );
	//def( "glScissor", glScissor );
	//def( "glShadeModel", glShadeModel );
	//def( "glStencilFunc", glStencilFunc );
	//def( "glStencilMask", glStencilMask );
	//def( "glStencilOp", glStencilOp );
	//def( "glTexCoord1d", glTexCoord1d );
	//def( "glTexCoord1dv", _wrap_glTexCoord1dv );
	//def( "glTexCoord1f", glTexCoord1f );
	//def( "glTexCoord1fv", _wrap_glTexCoord1fv );
	//def( "glTexCoord1i", glTexCoord1i );
	//def( "glTexCoord1iv", _wrap_glTexCoord1iv );
	//def( "glTexCoord1s", glTexCoord1s );
	//def( "glTexCoord1sv", _wrap_glTexCoord1sv );
	//def( "glTexCoord2d", glTexCoord2d );
	//def( "glTexCoord2dv", _wrap_glTexCoord2dv );
	//def( "glTexCoord2f", glTexCoord2f );
	//def( "glTexCoord2fv", _wrap_glTexCoord2fv );
	//def( "glTexCoord2i", glTexCoord2i );
	//def( "glTexCoord2iv", _wrap_glTexCoord2iv );
	//def( "glTexCoord2s", glTexCoord2s );
	//def( "glTexCoord2sv", _wrap_glTexCoord2sv );
	//def( "glTexCoord3d", glTexCoord3d );
	//def( "glTexCoord3dv", _wrap_glTexCoord3dv );
	//def( "glTexCoord3f", glTexCoord3f );
	//def( "glTexCoord3fv", _wrap_glTexCoord3fv );
	//def( "glTexCoord3i", glTexCoord3i );
	//def( "glTexCoord3iv", _wrap_glTexCoord3iv );
	//def( "glTexCoord3s", glTexCoord3s );
	//def( "glTexCoord3sv", _wrap_glTexCoord3sv );
	//def( "glTexCoord4d", glTexCoord4d );
	//def( "glTexCoord4dv", _wrap_glTexCoord4dv );
	//def( "glTexCoord4f", glTexCoord4f );
	//def( "glTexCoord4fv", _wrap_glTexCoord4fv );
	//def( "glTexCoord4i", glTexCoord4i );
	//def( "glTexCoord4iv", _wrap_glTexCoord4iv );
	//def( "glTexCoord4s", glTexCoord4s );
	//def( "glTexCoord4sv", _wrap_glTexCoord4sv );
	//def( "glTexEnvf", glTexEnvf );
	//def( "glTexEnvfv", _wrap_glTexEnvfv );
	//def( "glTexEnvi", glTexEnvi );
	//def( "glTexEnviv", _wrap_glTexEnviv );
	//def( "glTexGend", glTexGend );
	//def( "glTexGendv", _wrap_glTexGendv );
	//def( "glTexGenf", glTexGenf );
	//def( "glTexGenfv", _wrap_glTexGenfv );
	//def( "glTexGeni", glTexGeni );
	//def( "glTexGeniv", _wrap_glTexGeniv );
	//def( "glTexParameterf", glTexParameterf );
	//def( "glTexParameterfv", _wrap_glTexParameterfv );
	//def( "glTexParameteri", glTexParameteri );
	//def( "glTexParameteriv", _wrap_glTexParameteriv );
	//def( "glTranslated", glTranslated );
	//def( "glTranslatef", glTranslatef );
	//def( "glVertex2d", glVertex2d );
	//def( "glVertex2dv", _wrap_glVertex2dv );
	//def( "glVertex2f", glVertex2f );
	//def( "glVertex2fv", _wrap_glVertex2fv );
	//def( "glVertex2i", glVertex2i );
	//def( "glVertex2iv", _wrap_glVertex2iv );
	//def( "glVertex2s", glVertex2s );
	//def( "glVertex2sv", _wrap_glVertex2sv );
	//def( "glVertex3d", glVertex3d );
	//def( "glVertex3dv", _wrap_glVertex3dv );
	//def( "glVertex3f", glVertex3f );
	//def( "glVertex3fv", _wrap_glVertex3fv );
	//def( "glVertex3i", glVertex3i );
	//def( "glVertex3iv", _wrap_glVertex3iv );
	//def( "glVertex3s", glVertex3s );
	//def( "glVertex3sv", _wrap_glVertex3sv );
	//def( "glVertex4d", glVertex4d );
	//def( "glVertex4dv", _wrap_glVertex4dv );
	//def( "glVertex4f", glVertex4f );
	//def( "glVertex4fv", _wrap_glVertex4fv );
	//def( "glVertex4i", glVertex4i );
	//def( "glVertex4iv", _wrap_glVertex4iv );
	//def( "glVertex4s", glVertex4s );
	//def( "glVertex4sv", _wrap_glVertex4sv );
	//def( "glViewport", glViewport );
}


#endif
