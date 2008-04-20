//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LINEARBRUSH_H__
#define LINEARBRUSH_H__

#include <Brush/SphericalBoundaryBrush.h>



class GS_DllExport LinearBrush : public SphericalBoundaryBrush
{
RT_TYPE;
private:
	double xScale;

public:
	LinearBrush();
	LinearBrush(double radius);
	LinearBrush(const LinearBrush &b);
	virtual ~LinearBrush();


	virtual void setRadius(double radius);


	virtual double computePointWeight(const Point3 &brushPosition, const Point3 &point) const;


private:
	void refresh();
};



#endif // LINEARBRUSH_H__
