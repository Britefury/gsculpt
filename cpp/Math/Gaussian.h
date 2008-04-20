//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GAUSSIAN_H__
#define GAUSSIAN_H__

#include <math.h>

#include <Brush/SphericalBrush.h>



// Gaussian function
// x: parameter
// a: height at peak
// b: position of the centre of the peak
// c: c determines the width. It is related to the FWHM (full width at half maximum).

GS_DllExport inline double gaussian(double x, double a, double b, double c)
{
	double relX = x - b;
	double exponent = ( relX * relX )  /  ( c * c * 2.0 );
	return a * exp( -exponent );
}

// Inverse Gaussian function
// y: parameter
// a: height at peak
// b: position of the centre of the peak
// c: c determines the width. It is related to the FWHM (full width at half maximum).

GS_DllExport inline double invGaussian(double y, double a, double b, double c)
{
	return sqrt( -2.0 * c * c * log( y / a ) )  +  b;
}



#endif // GAUSSIAN_H__

