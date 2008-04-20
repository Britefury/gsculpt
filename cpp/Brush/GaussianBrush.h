//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GAUSSIANBRUSH_H__
#define GAUSSIANBRUSH_H__

#include <Brush/SphericalBoundaryBrush.h>



class GS_DllExport GaussianBrush : public SphericalBoundaryBrush
{
RT_TYPE;
private:
	double gaussianMinimum;
	double yOffset, yScale, xScale;

public:
	GaussianBrush();
	GaussianBrush(double radius, double gaussianMinimum);
	GaussianBrush(const GaussianBrush &b);
	virtual ~GaussianBrush();


	virtual void setRadius(double radius);

	double getGaussianMinimum() const;
	void setGaussianMinimum(double gaussianMinimum);


	virtual double computePointWeight(const Point3 &brushPosition, const Point3 &point) const;


private:
	void refresh();
};



#endif // GAUSSIANBRUSH_H__
