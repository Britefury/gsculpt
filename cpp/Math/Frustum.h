//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef FRUSTUM_H__
#define FRUSTUM_H__

#include <boost/python.hpp>



#include <stdio.h>

#include <Util/Array.h>

#include <Math/Point3.h>
#include <Math/Plane.h>
#include <Math/BBox3.h>
#include <Math/Matrix4.h>
#include <Math/Segment3.h>

/*
FRUSTRUM:
a list of planes which mark out a region of 3D space
*/




class GS_DllExport Frustum
{
protected:
	//the planes
	Array<Plane> planes;


public:
	inline Frustum()
	{
	}

	inline Frustum(const Array<Plane> &p)
	{
		planes = p;
	}

	inline Frustum(boost::python::list pl)
	{
		boost::python::object lenObj = pl.attr( "__len__" )();
		boost::python::extract<int> lenExtract( lenObj );
		if ( lenExtract.check() )
		{
			int numPlanes = lenExtract;
			planes.reserve( numPlanes );
			for (int i = 0; i < numPlanes; i++)
			{
				boost::python::object planeObj = pl[i];
				boost::python::extract<Plane&> planeExtract( planeObj );
				if ( planeExtract.check() )
				{
					planes.push_back( planeExtract );
				}
			}
		}
	}



	//read and write methods
	inline void read(FILE *f)
	{
		int numberOfPlanes;
		fread( &numberOfPlanes, sizeof(int), 1, f );

		planes.resize( numberOfPlanes );
		for (int i = 0; i < numberOfPlanes; i++)
		{
			planes[i].read( f );
		}
	}

	inline void write(FILE *f) const
	{
		int numberOfPlanes = planes.size();
		fwrite( &numberOfPlanes, sizeof(int), 1, f );

		for (int i = 0; i < planes.size(); i++)
		{
			planes[i].write( f );
		}
	}


	//return true if this contains p
	inline bool contains(const Point3 &p) const
	{
		for (int i = 0; i < planes.size(); i++)
		{
			if ( p < planes[i] )
			{
				return false;
			}
		}

		return true;
	}

	inline bool py_containsPoint(const Point3 &p) const
	{
		return contains( p );
	}

	//See if box is inside this
	//returns:
	//		SIDE_POSITIVE: box is wholly inside this
	//		SIDE_NEGATIVE: box is wholly outside this
	//		SIDE_BOTH: box is partially inside and partially outside this
	inline Side contains(const BBox3 &box) const
	{
		Side result, current;

		if ( planes.size() == 0 )
		{
			return SIDE_POSITIVE;
		}

		result = box.side( planes[0] );
		if ( result == SIDE_NEGATIVE )
		{
			return SIDE_NEGATIVE;
		}

		for (int i = 1; i < planes.size(); i++)
		{
			current = box.side( planes[i] );
			if ( current == SIDE_NEGATIVE )
			{
				return SIDE_NEGATIVE;
			}
			if ( current == SIDE_BOTH )
			{
				result = SIDE_BOTH;
			}
		}

		return result;
	}

	inline bool py_containsBox(const BBox3 &box) const
	{
		return contains( box );
	}


	//clips line so that it is wholly inside this
	inline bool clip(Segment3 &seg) const
	{
		for (int i = 0; i < planes.size(); i++)
		{
			if ( !seg.clip( planes[i] ) )
			{
				return false;
			}
		}

		return true;
	}


	//matrix transform functions need access to private members
	friend void operator*=(Frustum &f, const Matrix4 &m);
	friend Frustum operator*(const Frustum &f, const Matrix4 &m);
};


inline void operator*=(Frustum &f, const Matrix4 &m)
{
	for (int i = 0; i < f.planes.size(); i++)
	{
		f.planes[i] *= m;
	}
}

inline Frustum operator*(const Frustum &f, const Matrix4 &m)
{
	Frustum result = f;
	result *= m;
	return result;
}









class PerspectiveFrustum : public Frustum
{
public:
	//viewing fustrum (perspective projection):
	//fovY: field of view in Y direction
	//aspectRatio: aspect ratio of screen; width/height
	//nearClip: distance of near clipping plane
	//farClip: distance of far clipping plane
	//nearClipFlag: create a near clipping plane?
	inline PerspectiveFrustum(double fovY, double aspectRatio, double nearClip, double farClip, bool nearClipFlag = true)
	{
		double x, y;
		Point3 p1, p2, p3, p4;

		y = tan( fovY * 0.5 );
		x = y * aspectRatio;

		//generate four corner points of screen at a distance of 1.0 from the camera
		p1 = Point3( -x, y,  -1.0 );
		p2 = Point3( x,  y,  -1.0 );
		p3 = Point3( x,  -y, -1.0 );
		p4 = Point3( -x, -y, -1.0 );

		//generate each plane
		planes.reserve( nearClipFlag  ?  6 : 5 );
		planes.push_back( Plane( p1, p2, Point3() ) );
		planes.push_back( Plane( p2, p3, Point3() ) );
		planes.push_back( Plane( p3, p4, Point3() ) );
		planes.push_back( Plane( p4, p1, Point3() ) );
		if ( nearClipFlag )
		{
			planes.push_back( Plane( Vector3( 0.0, 0.0, -1.0 ), Point3( 0.0, 0.0, -nearClip ) ) );
		}
		planes.push_back( Plane( Vector3( 0.0, 0.0, 1.0 ), Point3( 0.0, 0.0, -farClip ) ) );
	}
};






class OrthographicFrustum : public Frustum
{
public:
	//viewing frustum (orthographic projection):
	//height: height of fustrum
	//aspectRatio: aspect ratio of screen; width/height
	//nearClip: distance of near clipping plane
	//farClip: distance of far clipping plane
	//nearClipFlag: create a near clipping plane?
	inline OrthographicFrustum(double height, double aspectRatio, double nearClip, double farClip, bool nearClipFlag = true)
	{
		double width = height * aspectRatio;

		planes.reserve( nearClipFlag ? 6 : 5 );

		//left
		planes.push_back( Plane( Vector3( 1.0, 0.0, 0.0 ), Point3( -width * 0.5, 0.0, 0.0 ) ) );

		//bottom
		planes.push_back( Plane( Vector3( 0.0, 1.0, 0.0 ), Point3( 0.0, -height * 0.5, 0.0 ) ) );


		//right
		planes.push_back( Plane( Vector3( -1.0, 0.0, 0.0 ), Point3( width * 0.5, 0.0, 0.0 ) ) );

		//top
		planes.push_back( Plane( Vector3( 0.0, -1.0, 0.0 ), Point3( 0.0,  height * 0.5, 0.0 ) ) );


		//near
		if ( nearClipFlag )
		{
			planes.push_back( Plane( Vector3( 0.0, 0.0, -1.0 ), Point3( 0.0, 0.0, -nearClip ) ) );
		}

		//far
		planes.push_back( Plane( Vector3( 0.0, 0.0, 1.0 ), Point3( 0.0, 0.0, -farClip ) ) );
	}
};




#endif
