//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYDRAWING_CPP__
#define PYDRAWING_CPP__

#include <boost/python.hpp>
using namespace boost::python;

#include <Graphics/Drawing.h>


inline void py_glVertex3p3(const Point3 &p)
{
	glVertex3d( p.x, p.y, p.z );
}

inline void py_glVertex3p2z(const Point2 &p, double z = 0.0)
{
	glVertex3d( p.x, p.y, z );
}

inline void py_glVertex3p2fz(const Point2f &p, float z = 0.0f)
{
	glVertex3f( p.x, p.y, 0.0 );
}

inline void py_glVertex2p2(const Point2 &p)
{
	glVertex2d( p.x, p.y );
}

inline void py_glVertex2p2f(const Point2f &p)
{
	glVertex2f( p.x, p.y );
}

inline void py_glRasterPos3p3(const Point3 &p)
{
	glRasterPos3d( p.x, p.y, p.z );
}

inline void py_glRasterPos2p2(const Point2 &p)
{
	glRasterPos2d( p.x, p.y );
}

inline void py_glColor3c3(const Colour3f &c)
{
	glColor3f( c.r, c.g, c.b );
}

inline void py_glColor4c3a(const Colour3f &c, float alpha)
{
	glColor4f( c.r, c.g, c.b, alpha );
}


void export_Drawing()
{
	def( "setDrawingFont", setDrawingFont );
	def( "disableDrawingFont", disableDrawingFont );
	def( "glDrawString3", glDrawString3 );
	def( "glDrawString2", glDrawString2 );
	def( "glVertex3Point3", py_glVertex3p3 );
	def( "glVertex3Point2Z", py_glVertex3p2z );
	def( "glVertex3Point2fZ", py_glVertex3p2fz );
	def( "glVertex2Point2", py_glVertex2p2 );
	def( "glVertex2Point2f", py_glVertex2p2f );
	def( "glRasterPos3Point3", py_glRasterPos3p3 );
	def( "glRasterPos2Point2", py_glRasterPos2p2 );
	def( "glColor3Colour3f", py_glColor3c3 );
	def( "glColor4Colour3fA", py_glColor4c3a );
}


#endif
