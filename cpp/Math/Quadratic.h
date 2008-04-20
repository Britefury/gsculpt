//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef QUADRATIC_H__
#define QUADRATIC_H__

#include <algorithm>

#include <Math/Vector2.h>



#define QUADRATIC_EPSILON 1e-10


class GS_DllExport QuadraticRootList
{
private:
	double roots[2];
	int numRoots;

public:
	inline QuadraticRootList() : numRoots( 0 )
	{
	}

	inline QuadraticRootList(const QuadraticRootList &r)
	{
		roots[0] = r.roots[0];
		roots[1] = r.roots[1];
		numRoots = r.numRoots;
	}

	inline QuadraticRootList(double a) : numRoots( 1 )
	{
		roots[0] = a;
	}

	inline QuadraticRootList(double a, double b) : numRoots( 1 )
	{
		roots[0] = a;
		roots[1] = b;
	}



	inline void setRoots(double a)
	{
		roots[0] = a;
		numRoots = 1;
	}

	inline void setRoots(double a, double b)
	{
		roots[0] = a;
		roots[1] = b;
		numRoots = 2;
	}


	inline void clear()
	{
		numRoots = 0;
	}


	inline double getRoot(int i) const
	{
		return roots[i];
	}

	inline double operator[](int i) const
	{
		return roots[i];
	}

	inline double getMinimumRoot() const
	{
		switch ( numRoots )
		{
		case 0:
			return 0.0;
		case 1:
			return roots[0];
		case 2:
			return std::min( roots[0], roots[1] );
		default:
			return 0.0;
		}
	}

	inline double getMaximumRoot() const
	{
		switch ( numRoots )
		{
		case 0:
			return 0.0;
		case 1:
			return roots[0];
		case 2:
			return std::max( roots[0], roots[1] );
		default:
			return 0.0;
		}
	}

	inline int getNumRoots() const
	{
		return numRoots;
	}

	inline bool hasRoots() const
	{
		return numRoots > 0;
	}



	inline void removeRoot0()
	{
		roots[0] = roots[1];
		numRoots--;
	}

	inline void clampRootsToUnitInterval()
	{
		if ( roots[0] < 0.0  ||  roots[0] > 1.0 )
		{
			removeRoot0();
			if ( roots[0] < 0.0  ||  roots[0] > 1.0 )
			{
				removeRoot0();
			}
		}
		else
		{
			if ( roots[1] < 0.0  ||  roots[1] > 1.0 )
			{
				numRoots--;
			}
		}
	}

	inline void removeRootAt0()
	{
		if ( numRoots == 2 )
		{
			if ( roots[0] >= -QUADRATIC_EPSILON  &&  roots[0] <= QUADRATIC_EPSILON )
			{
				removeRoot0();
				return;
			}

			if ( roots[1] >= -QUADRATIC_EPSILON  &&  roots[1] <= QUADRATIC_EPSILON )
			{
				numRoots--;
				return;
			}
		}
	}
};



GS_DllExport inline Vector2 evaluateQuadBezierPoint2(const Vector2 c[3], double t)
{
	return c[0] * ( ( 1.0 - t ) * ( 1.0 - t ) ) +
			 c[1] * ( 2.0 * t * ( 1.0 - t ) ) +
			 c[2] * ( t * t );
}

GS_DllExport QuadraticRootList solveQuadratic(double a, double b, double c)
{
	if ( a == 0.0 )
	{
		//linear eqn: solve
		if ( b == 0.0 )
		{
			return QuadraticRootList();
		}
		else
		{
			return QuadraticRootList( -c / b );
		}
	}
	else
	{
		double bSquaredMinus4ac = b * b  -  4.0 * a * c;

		if ( bSquaredMinus4ac < 0.0 )
		{
			return QuadraticRootList();
		}
		else
		{
			double oneOverDenominator = 1.0 / ( 2.0 * a );
			double sq;

			if ( bSquaredMinus4ac == 0.0 )
			{
				return QuadraticRootList( -b * oneOverDenominator );
			}
			else
			{
				sq = sqrt( bSquaredMinus4ac );
				double r[2];
				r[0] = ( -b + sq ) * oneOverDenominator;
				r[1] = ( -b - sq ) * oneOverDenominator;
				if ( r[1] < r[0] )
				{
					double swap = r[0];
					r[0] = r[1];
					r[1] = swap;
				}

				return QuadraticRootList( r[0], r[1] );
			}
		}
	}
}


GS_DllExport inline QuadraticRootList solveQuadraticBezier(double control0, double control1, double control2)
{
	return solveQuadratic( control0  -  2.0 * control1  +  control2,
						 2.0 * control1  -  2.0 * control0,
						 control0 );
}


#endif
