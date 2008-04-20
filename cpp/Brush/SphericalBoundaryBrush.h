//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef SPHERICALBOUNDARYBRUSH_H__
#define SPHERICALBOUNDARYBRUSH_H__

#include <Brush/Brush.h>



class GS_DllExport SphericalBoundaryBrush : public Brush
{
RT_ABSTRACT_TYPE;
protected:
	double radius, sqrRadius;


public:
	SphericalBoundaryBrush();
	SphericalBoundaryBrush(double radius);
	SphericalBoundaryBrush(const SphericalBoundaryBrush &b);
	virtual ~SphericalBoundaryBrush();


	double getRadius() const;
	virtual void setRadius(double radius);


	virtual bool intersectsBBox(const Point3 &brushPosition, const BBox3 &box) const;
};



#endif // SPHERICALBOUNDARYBRUSH_H__
