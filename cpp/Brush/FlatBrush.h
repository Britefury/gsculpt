//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef FLATBRUSH_H__
#define FLATBRUSH_H__

#include <Brush/SphericalBoundaryBrush.h>



class GS_DllExport FlatBrush : public SphericalBoundaryBrush
{
RT_TYPE;
public:
	FlatBrush();
	FlatBrush(double radius);
	FlatBrush(const FlatBrush &b);
	virtual ~FlatBrush();


	virtual double computePointWeight(const Point3 &brushPosition, const Point3 &point) const;
};



#endif // FLATBRUSH_H__
