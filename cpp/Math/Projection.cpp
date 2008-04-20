//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PROJECTION_CPP__
#define PROJECTION_CPP__

#include <Math/Projection.h>



Projection::Projection() : nearClipPlane( Vector3( 0.0, 0.0, 1.0 ), 0.0 ), farClipPlane( Vector3( 0.0, 0.0, -1.0 ), -1.0 )
{
}

Projection::Projection(const Matrix4 &worldToScreen, const Matrix4 &screenToWorld, const Plane &nearClip, const Plane &farClip)
				: worldToScreenMatrix( worldToScreen ), screenToWorldMatrix( screenToWorld ), nearClipPlane( nearClip ), farClipPlane( farClip )
{
}


const Matrix4 & Projection::getWorldToScreenMatrix() const
{
	return worldToScreenMatrix;
}

const Matrix4 & Projection::getScreenToWorldMatrix() const
{
	return screenToWorldMatrix;
}

const Plane & Projection::getNearClipPlane() const
{
	return nearClipPlane;
}

const Plane & Projection::getFarClipPlane() const
{
	return farClipPlane;
}


Point3 Projection::transformWorldToScreen(const Point3 &world) const
{
	return worldToScreenMatrix.transformHomogeneous( world );
}

Point3 Projection::transformScreenToWorldScreenZ(const Point2 &screen, double z) const
{
	return screenToWorldMatrix.transformHomogeneous( screen, z );
}


Segment3 Projection::computeRay(const Point2 &screen) const
{
	Point3 near = transformScreenToWorldScreenZ( screen, 0.0 );
	Point3 far = transformScreenToWorldScreenZ( screen, 1.0 );

	return Segment3( near, far );
}


bool Projection::containsPoint(const Point3 &point) const
{
	return point >= nearClipPlane  &&  point >= farClipPlane;
}

Point3 Projection::projectPoint(const Point3 &point) const
{
	return worldToScreenMatrix.transformHomogeneous( point );
}

Point2 Projection::projectPoint2d(const Point3 &point) const
{
	return worldToScreenMatrix.transformHomogeneous2d( point );
}

bool Projection::checkAndProjectPoint(Point3 &out, const Point3 &in) const
{
	if ( containsPoint( in ) )
	{
		out = projectPoint( in );
		return true;
	}
	else
	{
		return false;
	}
}

bool Projection::checkAndProjectPoint2d(Point2 &out, const Point3 &in) const
{
	if ( containsPoint( in ) )
	{
		out = projectPoint2d( in );
		return true;
	}
	else
	{
		return false;
	}
}



bool Projection::clipSegment(Segment3 &seg) const
{
	if ( seg.clip( nearClipPlane ) )
	{
		if ( seg.clip( farClipPlane ) )
		{
			return true;
		}
	}

	return false;
}

Segment3 Projection::projectSegment(const Segment3 &seg) const
{
	return worldToScreenMatrix.transformHomogeneous( seg );
}

Segment2 Projection::projectSegment2d(const Segment3 &seg) const
{
	return worldToScreenMatrix.transformHomogeneous2d( seg );
}

bool Projection::clipAndProjectSegment(Segment3 &out, Segment3 in) const
{
	if ( clipSegment( in ) )
	{
		out = projectSegment( in );
		return true;
	}
	else
	{
		return false;
	}
}

bool Projection::clipAndProjectSegment2d(Segment2 &out, Segment3 in) const
{
	if ( clipSegment( in ) )
	{
		out = projectSegment2d( in );
		return true;
	}
	else
	{
		return false;
	}
}


void Projection::clipPoly(Polygon3 &poly) const
{
	Polygon3 temp;
	Polygon3::sutherlandHodgmanClip( temp, poly, nearClipPlane);
	Polygon3::sutherlandHodgmanClip( poly, temp, farClipPlane );
}

void Projection::clipPoly(Polygon3 &poly, Array<int> &indices) const
{
	Polygon3 temp;
	Array<int> initialIndices, tempIndices;

	generateInitialIndices( initialIndices, poly.size() );

	Polygon3::sutherlandHodgmanClip( temp, tempIndices, poly, initialIndices, nearClipPlane );
	Polygon3::sutherlandHodgmanClip( poly, indices, temp, tempIndices, farClipPlane );
}

void Projection::clipAndProjectPoly(Polygon3 &poly) const
{
	clipPoly( poly );
	projectPoly( poly );
}

void Projection::clipAndProjectPoly(Polygon3 &poly, Array<int> &indices) const
{
	clipPoly( poly, indices );
	projectPoly( poly );
}

void Projection::clipAndProjectPoly(Polygon2 &out, const Polygon3 &in) const
{
	Polygon3 temp, temp2;

	Polygon3::sutherlandHodgmanClip( temp, in, nearClipPlane );
	Polygon3::sutherlandHodgmanClip( temp2, temp, farClipPlane );

	projectPoly( out, temp2 );
}

void Projection::clipAndProjectPoly(Polygon2 &out, Array<int> &indices,  const Polygon3 &in) const
{
	Polygon3 temp, temp2;
	Array<int> initialIndices, tempIndices;

	generateInitialIndices( initialIndices, in.size() );

	Polygon3::sutherlandHodgmanClip( temp, tempIndices, in, initialIndices, nearClipPlane );
	Polygon3::sutherlandHodgmanClip( temp2, indices, temp, tempIndices, farClipPlane );

	projectPoly( out, temp2 );
}


Projection Projection::preTransformed(const Matrix4 &m, const Matrix4 &mInv) const
{
	return Projection( m * worldToScreenMatrix, screenToWorldMatrix * mInv, nearClipPlane * mInv, farClipPlane * mInv );
}


void Projection::generateInitialIndices(Array<int> &indices, int size)
{
	indices.resize( size );
	for (int i = 0; i < size; i++)
	{
		indices[i] = i;
	}
}

void Projection::projectPoly(Polygon3 &poly) const
{
	for (int i = 0; i < poly.size(); i++)
	{
		poly[i] = worldToScreenMatrix.transformHomogeneous( poly[i] );
	}
}

void Projection::projectPoly(Polygon2 &out, const Polygon3 &in) const
{
	out.resize( in.size() );

	for (int i = 0; i < in.size(); i++)
	{
		out[i] = worldToScreenMatrix.transformHomogeneous( in[i] ).toPoint2();
	}
}



#endif
