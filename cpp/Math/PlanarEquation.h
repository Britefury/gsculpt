//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PLANAREQUATION_H__
#define PLANAREQUATION_H__

#include <Util/gsassert.h>
#include <Util/Array.h>

#include <Math/Point3.h>
#include <Math/Vector3.h>
#include <Math/Matrix3.h>




class GS_DllExport PlanarEquation
{
private:
	double a, b, c;			//Z = a + bX + cY


public:
	inline PlanarEquation() : a( 0.0 ), b( 0.0 ), c( 0.0 )
	{
	}

	inline PlanarEquation(double a, double b, double c)
						: a( a ), b( b ), c( c )
	{
	}

	inline PlanarEquation(const Array<Point3> &points)
	{
		gs_assert( points.size() >= 3, "PlanarEquation::PlanarEquation(): not enough data points\n" );

		//P =		[ 1	1	1	1 ]
		//		[ x0	x1	..	xn ]
		//		[ y0	y1	..	yn ]
		//
		//G =	[ a b c ]
		//
		//D =	[ z0	z1	..	zn ]
		//
		// G . P  =  D
		// G . P . Pt  =  D . Pt		(where Pt = transpose of P)
		// G = (D . Pt)  .  (P . Pt) ^ -1

		Vector3 dPt;
		Matrix3 pPt;

		// Matrices are initialised to identity; set the diagonal elements to 0
		pPt.at(0,0) = pPt.at(1,1) = pPt.at(2,2) = 0.0;

		for (int i = 0; i < points.size(); i++)
		{
			double x = points[i].x;
			double y = points[i].y;
			double z = points[i].z;
			double xy = x * y;
			dPt.x += z;
			dPt.y += z * x;
			dPt.z += z * y;

			pPt.at(0,0) += 1.0;
			pPt.at(0,1) += x;
			pPt.at(0,2) += y;

			pPt.at(1,0) += x;
			pPt.at(1,1) += x * x;
			pPt.at(1,2) += xy;

			pPt.at(2,0) += y;
			pPt.at(2,1) += xy;
			pPt.at(2,2) += y * y;
		}

		pPt.invert();
		Vector3 g = dPt * pPt;

		a = g.x;
		b = g.y;
		c = g.z;
	}



	inline double evaluate(double x, double y) const
	{
		return a  +  b * x  +  c * y;
	}
};



#endif
