//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef EULERROTATION_H__
#define EULERROTATION_H__

#include <math.h>

#include <Math/Vector3.h>




class GS_DllExport EulerRotation
{
public:
	union
	{
		struct
		{
			double x, y, z;
		};

		double v[3];
	};



	inline EulerRotation() : x( 0.0 ), y( 0.0 ), z( 0.0 )
	{
	}

	inline EulerRotation(double ix, double iy, double iz)
								: x( ix ), y( iy ), z( iz )
	{
	}

	inline EulerRotation(const Vector3 &v)
								: x( v.x ), y( v.y ), z( v.z )
	{
	}



	inline double & operator[](int i)
	{
		return v[i];
	}

	inline const double & operator[](int i) const
	{
		return v[i];
	}



	inline Vector3 toVector3() const
	{
		return Vector3( x, y, z );
	}
};


#endif
