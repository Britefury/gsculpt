//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef DRAWING_CPP__
#define DRAWING_CPP__

#include <math.h>

#include <string.h>

#include <PlatformSpecific/IncludeGL.h>

#include <Math/MathConstants.h>

#include <Util/gsassert.h>
#include <Util/Array.h>

#include <Graphics/Drawing.h>


#define CIRCLE_SEGMENTS 72



static GLuint drawing__gl_font_base;
static int drawing__font_height;
static bool drawing__font_enabled = false;

static bool drawing__mark_override;
static int drawing__mark_override_count = 0;


void setDrawingFont(GLuint glFontBase, int height)
{
	drawing__gl_font_base = glFontBase;
	drawing__font_height = height;
	drawing__font_enabled = true;
}

void disableDrawingFont()
{
	drawing__font_enabled = false;
}


int getDrawingFontHeight()
{
	gs_assert( drawing__font_enabled, "getDrawingFontHeight(): drawing font is disabled\n" );

	return drawing__font_enabled  ?  drawing__font_height  :  0;
}

/*int drawingComputeStringWidth(const char *str)
{
	return gdk_string_width( drawing__gdk_font, str );
}*/


void glDrawCircle2(const Point2 &centre, double radius)
{
	double a = 0.0;

	double circumference = radius * 2.0 * M_PI;
	int segments = (int)( circumference * 0.3 );

	double aInc = 2.0 * M_PI / (double)segments;

	glBegin( GL_LINE_LOOP );
	for (int i = 0; i < segments; i++)
	{
		double x = centre.x + sin(a) * radius;
		double y = centre.y + cos(a) * radius;

		glVertex2d( x, y );

		a += aInc;
	}
	glEnd();
}



static const Array<Point2> & getCirclePoints()
{
	static Array<Point2> p;
	static bool initialisedFlag = false;

	if ( !initialisedFlag )
	{
		p.resize( CIRCLE_SEGMENTS );

		double theta = 0.0;
		double thetaInc = 2.0 * M_PI  /  (double)CIRCLE_SEGMENTS;
		for (int vertexI = 0; vertexI < CIRCLE_SEGMENTS; vertexI++)
		{
			p[vertexI] = Point2( sin( theta ), cos( theta ) );

			theta += thetaInc;
		}

		initialisedFlag = true;
	}

	return p;
}

void glDrawCircle3(const Point3 &centre, const Vector3 &u, const Vector3 &v, double radius)
{
	const Array<Point2> &circlePoints = getCirclePoints();

	glBegin( GL_LINE_LOOP );
	for (int vertexI = 0; vertexI < circlePoints.size(); vertexI++)
	{
		const Point2 &p = circlePoints[vertexI];

		glVertex3( centre  +  u * p.x  +  v * p.y );
	}
	glEnd();
}

void glDrawArc3(const Point3 &centre, const Vector3 &u, const Vector3 &v, double radius, double theta0, double theta1)
{
	double theta = theta0;
	double thetaInc = ( theta1 - theta0 )  /  (double)CIRCLE_SEGMENTS;

	glBegin( GL_LINE_STRIP );
	for (int i = 0; i <= CIRCLE_SEGMENTS; i++)
	{
		double x = sin( theta )  *  radius;
		double y = cos( theta )  *  radius;

		glVertex3( centre  +  u * x  +  v * y );

		theta += thetaInc;
	}
	glEnd();
}



void glDrawString3(const Point3 &p, const char *str)
{
	gs_assert( drawing__font_enabled, "getDrawingFontHeight(): drawing font is disabled\n" );

	if ( drawing__font_enabled )
	{
		glRasterPos3d( p.x, p.y, p.z );
		glListBase( drawing__gl_font_base );
		glCallLists( strlen( str ), GL_UNSIGNED_BYTE, str );
	}
}

void glDrawString2(const Point2 &p, const char *str)
{
	gs_assert( drawing__font_enabled, "getDrawingFontHeight(): drawing font is disabled\n" );

	if ( drawing__font_enabled )
	{
		glRasterPos2d( p.x, p.y );
		glListBase( drawing__gl_font_base );
		glCallLists( strlen( str ), GL_UNSIGNED_BYTE, str );
	}
}



void pushGraphicsMarkOverride(bool override)
{
	if ( drawing__mark_override_count == 0 )
	{
		drawing__mark_override = override;
	}
	drawing__mark_override_count++;
}

void popGraphicsMarkOverride()
{
	drawing__mark_override_count--;
}

bool graphicsOverrideMarkState(bool state)
{
	if ( drawing__mark_override_count > 0 )
	{
		return drawing__mark_override;
	}
	else
	{
		return state;
	}
}


#endif
