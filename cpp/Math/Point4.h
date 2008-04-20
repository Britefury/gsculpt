//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POINT4_H__
#define POINT4_H__

#include <Math/Point3.h>



class GS_DllExport Point4
{
public:
	double x, y, z, w;


	inline Point4() : x( 0.0 ), y( 0.0 ), z( 0.0 ), w( 1.0 )
	{
	}

	inline Point4(double ix, double iy, double iz)
					  : x( ix ), y( iy ), z( iz ), w( 1.0 )
	{
	}

	inline Point4(double ix, double iy, double iz, double iw)
					  : x( ix ), y( iy ), z( iz ), w( iw )
	{
	}

	inline Point4(const Point3 &p3)
					  : x( p3.x ), y( p3.y ), z( p3.z ), w( 1.0 )
	{
	}

	inline Point4(const Point3 &p3, double iw)
					  : x( p3.x ), y( p3.y ), z( p3.z ), w( iw )
	{
	}



	inline Point3 convertToPoint3() const
	{
		double oneOverW = 1.0 / w;
		return Point3( x * oneOverW, y * oneOverW, z * oneOverW );
	}

	inline Point3 inverseConvertToPoint3() const
	{
		return Point3( x * w, y * w, z * w );
	}

	inline Point3 toPoint3() const
	{
		return Point3( x, y, z );
	}
};



#endif

