//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef DRAWING_H__
#define DRAWING_H__

#include <PlatformSpecific/IncludeGL.h>

#include <Math/Point3.h>
#include <Math/Point2.h>
#include <Math/Point2f.h>
#include <Math/Vector3.h>
#include <Math/Colour3f.h>



GS_DllExport void setDrawingFont(GLuint glFontBase, int height);
GS_DllExport void disableDrawingFont();

GS_DllExport int getDrawingFontHeight();
//int drawingComputeStringWidth(const char *str);

GS_DllExport void glDrawCircle2(const Point2 &centre, double radius);
GS_DllExport void glDrawCircle3(const Point3 &centre, const Vector3 &u, const Vector3 &v, double radius);
GS_DllExport void glDrawArc3(const Point3 &centre, const Vector3 &u, const Vector3 &v, double radius, double theta0, double theta1);

GS_DllExport void glDrawString3(const Point3 &p, const char *str);
GS_DllExport void glDrawString2(const Point2 &p, const char *str);


GS_DllExport inline void glVertex3(const Point3 &p)
{
	glVertex3d( p.x, p.y, p.z );
}

GS_DllExport inline void glVertex3(const Point2 &p, double z = 0.0)
{
	glVertex3d( p.x, p.y, z );
}

GS_DllExport inline void glVertex3(const Point2f &p, float z = 0.0f)
{
	glVertex3f( p.x, p.y, 0.0 );
}

GS_DllExport inline void glVertex2(const Point2 &p)
{
	glVertex2d( p.x, p.y );
}

GS_DllExport inline void glVertex2(const Point2f &p)
{
	glVertex2f( p.x, p.y );
}

GS_DllExport inline void glRasterPos3(const Point3 &p)
{
	glRasterPos3d( p.x, p.y, p.z );
}

GS_DllExport inline void glRasterPos2(const Point2 &p)
{
	glRasterPos2d( p.x, p.y );
}

GS_DllExport inline void glColor3(const Colour3f &c)
{
	glColor3f( c.r, c.g, c.b );
}

GS_DllExport inline void glColor4(const Colour3f &c, float alpha)
{
	glColor4f( c.r, c.g, c.b, alpha );
}



GS_DllExport void pushGraphicsMarkOverride(bool override);
GS_DllExport void popGraphicsMarkOverride();
GS_DllExport bool graphicsOverrideMarkState(bool state);


#endif
