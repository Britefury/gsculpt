//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef EPSILON_H__
#define EPSILON_H__

#include <algorithm>


//define EPSILON for use in mathematics functions
#define EPSILON 1.0e-10
#define SQR_EPSILON (EPSILON * EPSILON)
#define ONEMINUSEPSILON (1.0 - EPSILON)
#define ONEPLUSEPSILON (1.0 + EPSILON)


template <typename Real> bool almostEqualAbsolute(Real a, Real b)
{
	return fabs( a - b )  <  Real( EPSILON );
}

template <typename Real> bool almostEqualRelative(Real a, Real b)
{
	Real epsilon = std::max( fabs( a ), fabs( b ) )  *  EPSILON;
	return fabs( a - b )  <  epsilon;
}


#endif

