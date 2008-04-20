//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BLENDINGPOLYLINE_CPP__
#define BLENDINGPOLYLINE_CPP__

#include <PolyBlend/BlendingPolyline.h>



BlendingPolyline::BlendingPolyline() : Array<Point3>()
{
}




void BlendingPolyline::operator*=(const Matrix4 &m)
{
	for (int i = 0; i < (int)size(); i++)
	{
		at( i ) *= m;
	}
}

const Point3 & BlendingPolyline::atWrapped(int i) const
{
	return (*this)[ i % size() ];
}

Point3 & BlendingPolyline::atWrapped(int i)
{
	return (*this)[ i % size() ];
}



void BlendingPolyline::close()
{
	closedFlag = true;
}

void BlendingPolyline::open()
{
	closedFlag = false;
}

void BlendingPolyline::setClosed(bool closed)
{
	closedFlag = closed;
}

bool BlendingPolyline::isClosed() const
{
	return closedFlag;
}



void BlendingPolyline::scaleToUnit()
{
	BBox3 box = BBox3( *this );

	double scale = 1.0 / box.getMajorSize();

	Matrix4 m = Matrix4::translate( -box.getCentre().toVector3() )  *  Matrix4::scale( scale );

	*this *= m;
}

void BlendingPolyline::scaleToUnit(BlendingPolyline &p1, BlendingPolyline &p2)
{
	BBox3 box = BBox3( p1 );
	for (int i = 0; i < p2.size(); i++)
	{
		box.addPoint( p2[i] );
	}

	double scale = 1.0 / box.getMajorSize();

	Matrix4 m = Matrix4::translate( -box.getCentre().toVector3() )  *  Matrix4::scale( scale );

	p1 *= m;
	p2 *= m;
}


#endif
