//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef REGION_CPP__
#define REGION_CPP__

#include <math.h>

#include <PlatformSpecific/IncludeGL.h>

#include <Math/Vector2.h>
#include <Math/Point2.h>
#include <Math/Segment2.h>
#include <Math/BBox2.h>
#include <Math/Polygon2.h>
#include <Math/Polygon3.h>

#include <Graphics/Region.h>

#include <Graphics/Drawing.h>



#define __CIRCLEREGION2D_NUMPOINTS 48






/***********************************************************************************
						Region2d
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( Region2d, RTObject )


Region2d::Region2d()
{
}

Region2d::Region2d(const Region2d &b)
{
}

Region2d::~Region2d()
{
}



bool Region2d::isEmpty() const
{
	return true;
}


bool Region2d::contains(const Point2 &p) const
{
	return false;
}

bool Region2d::containsPartOf(const Segment2 &seg) const
{
	return false;
}

bool Region2d::containsAllOf(const Segment2 &seg) const
{
	return false;
}

bool Region2d::containsPartOf(const Polygon2 &poly) const
{
	return false;
}

bool Region2d::containsAllOf(const Polygon2 &poly) const
{
	return false;
}


void Region2d::generatePoints(Array<Point2> &points) const
{
}






/***********************************************************************************
						RectangleRegion2d
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( RectangleRegion2d, Region2d )


RectangleRegion2d::RectangleRegion2d() : Region2d()
{
}

RectangleRegion2d::RectangleRegion2d(const RectangleRegion2d &b)
										: Region2d( b )
{
	box = b.box;
}

RectangleRegion2d::RectangleRegion2d(const BBox2 &b)
										: Region2d()
{
	box = b;
}

RectangleRegion2d::RectangleRegion2d(const Point2 &corner0, const Point2 &corner1)
										: Region2d()
{
	box = BBox2( corner0, corner1 );
}

RectangleRegion2d::~RectangleRegion2d()
{
}



bool RectangleRegion2d::isEmpty() const
{
	return false;
}


bool RectangleRegion2d::contains(const Point2 &p) const
{
	return box.contains( p );
}

bool RectangleRegion2d::containsPartOf(const Segment2 &seg) const
{
	return box.containsPartOf( seg );
}

bool RectangleRegion2d::containsAllOf(const Segment2 &seg) const
{
	return box.containsAllOf( seg );
}

bool RectangleRegion2d::containsPartOf(const Polygon2 &poly) const
{
	return box.containsPartOf( poly );
}

bool RectangleRegion2d::containsAllOf(const Polygon2 &poly) const
{
	return box.containsAllOf( poly );
}


void RectangleRegion2d::generatePoints(Array<Point2> &points) const
{
	points.reserve( 5 );
	points.push_back( box.getPoint( 0 ) );
	points.push_back( box.getPoint( 1 ) );
	points.push_back( box.getPoint( 3 ) );
	points.push_back( box.getPoint( 2 ) );
	points.push_back( box.getPoint( 0 ) );
}





/***********************************************************************************
						CircleRegion2d
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( CircleRegion2d, Region2d )


CircleRegion2d::CircleRegion2d() : Region2d()
{
}

CircleRegion2d::CircleRegion2d(const CircleRegion2d &b)
								: Region2d( b )
{
	circle = b.circle;
}

CircleRegion2d::CircleRegion2d(const Circle2 &circle)
								: Region2d(),
								circle( circle )
{
}

CircleRegion2d::~CircleRegion2d()
{
}



bool CircleRegion2d::isEmpty() const
{
	return false;
}


bool CircleRegion2d::contains(const Point2 &p) const
{
	return circle.contains( p );
}

bool CircleRegion2d::containsPartOf(const Segment2 &seg) const
{
	return circle.containsPartOf( seg );
}

bool CircleRegion2d::containsAllOf(const Segment2 &seg) const
{
	return circle.containsAllOf( seg );
}

bool CircleRegion2d::containsPartOf(const Polygon2 &poly) const
{
	return circle.containsPartOf( poly );
}

bool CircleRegion2d::containsAllOf(const Polygon2 &poly) const
{
	return circle.containsAllOf( poly );
}


void CircleRegion2d::generatePoints(Array<Point2> &points) const
{
	points.reserve( 36 );

	double theta = 0.0, thetaInc = M_PI * 2.0  /  36.0;
	for (int i = 0; i <= 36; i++)
	{
		points.push_back( circle.getCentre()  +  Vector2( cos( theta ), sin( theta ) ) * circle.getRadius() );

		theta += thetaInc;
	}
}



/***********************************************************************************
						PolylineRegion2d
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( PolylineRegion2d, Region2d )


PolylineRegion2d::PolylineRegion2d() : Region2d()
{
}

PolylineRegion2d::PolylineRegion2d(const PolylineRegion2d &b)
									: Region2d( b )
{
	vertices = b.vertices;
}

PolylineRegion2d::PolylineRegion2d(const Array<Point2> &vts)
									: Region2d()
{
	vertices = vts;
}

PolylineRegion2d::PolylineRegion2d(boost::python::list verts)
									: Region2d()
{
	py_setVertices( verts );
}

PolylineRegion2d::~PolylineRegion2d()
{
}



bool PolylineRegion2d::isEmpty() const
{
	return false;
}


bool PolylineRegion2d::contains(const Point2 &p) const
{
	return false;
}

bool PolylineRegion2d::containsPartOf(const Segment2 &seg) const
{
	for (int i = 0; i < vertices.size() - 1; i++)
	{
		//check each edge of the polyline against @l for intersection
		Segment2 edge( vertices[i], vertices[ i + 1 ] );

		if ( edge.intersects( seg ) )
		{
			//found an intersection; true
			return true;
		}
	}

	return false;
}

bool PolylineRegion2d::containsAllOf(const Segment2 &seg) const
{
	return false;
}

bool PolylineRegion2d::containsPartOf(const Polygon2 &poly) const
{
	// Check to see of @poly contains any of the vertices; this checks for the case
	// where the segments that make up @this are contained within poly
	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		if ( poly.contains( vertices[vertexI] ) )
		{
			return true;
		}
	}

	// Check to see if any of the edge segments of @this intersect with @poly
	for (int vertexI = 0; vertexI < vertices.size() - 1; vertexI++)
	{
		// Check each edge of the polyline against @poly for intersection
		Segment2 edge( vertices[vertexI], vertices[ vertexI + 1 ] );

		// For each edge of @poly
		int polyPrevI = poly.size() - 1;
		for (int polyI = 0; polyI < poly.size(); polyI++)
		{
			Segment2 polyEdge( poly[polyPrevI], poly[polyI] );

			if ( edge.intersects( polyEdge ) )
			{
				return true;
			}
		}
	}

	return false;
}

bool PolylineRegion2d::containsAllOf(const Polygon2 &poly) const
{
	return false;
}


void PolylineRegion2d::generatePoints(Array<Point2> &points) const
{
	points.replaceContentsWith( vertices );
}


boost::python::list PolylineRegion2d::py_getVertices() const
{
	boost::python::list result;
	for (int i = 0; i < vertices.size(); i++)
	{
		result.extend( vertices[i] );
	}
	return result;
}

void PolylineRegion2d::py_setVertices(boost::python::list verts)
{
	vertices.clear();
	boost::python::object lenObj = verts.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numVerts = lenExtract;
		vertices.reserve( numVerts );
		for (int i = 0; i < numVerts; i++)
		{
			boost::python::object pointObj = verts[i];
			boost::python::extract<Point2&> pointExtract( pointObj );
			if ( pointExtract.check() )
			{
				vertices.push_back( pointExtract );
			}
		}
	}
}







/***********************************************************************************
						LassoRegion2d
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( LassoRegion2d, Region2d )


LassoRegion2d::LassoRegion2d() : Region2d()
{
}

LassoRegion2d::LassoRegion2d(const LassoRegion2d &b)
									: Region2d( b )
{
	lasso = b.lasso;
}

LassoRegion2d::LassoRegion2d(const Polygon2 &p)
									: Region2d()
{
	lasso = p;
}

LassoRegion2d::LassoRegion2d(const Array<Point2> &vts)
									: Region2d()
{
	lasso = Polygon2( vts );
}

LassoRegion2d::~LassoRegion2d()
{
}



bool LassoRegion2d::isEmpty() const
{
	return false;
}


bool LassoRegion2d::contains(const Point2 &p) const
{
	return lasso.contains( p );
}

bool LassoRegion2d::containsPartOf(const Segment2 &seg) const
{
	return lasso.containsPartOf( seg );
}

bool LassoRegion2d::containsAllOf(const Segment2 &seg) const
{
	return lasso.containsAllOf( seg );
}

bool LassoRegion2d::containsPartOf(const Polygon2 &poly) const
{
	return lasso.containsPartOf( poly );
}

bool LassoRegion2d::containsAllOf(const Polygon2 &poly) const
{
	return lasso.containsAllOf( poly );
}


void LassoRegion2d::generatePoints(Array<Point2> &points) const
{
	//repeat first vertex at end
	points.reserve( lasso.size() + 1 );
	points.extend( lasso.getVertices() );
	points.push_back( lasso.firstVertex() );
}






/***********************************************************************************
						Region3d
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( Region3d, RTObject )


Region3d::Region3d()
{
}

Region3d::Region3d(const Region3d &b)
{
}

Region3d::~Region3d()
{
}



void Region3d::transform(const Matrix4 &m, const Matrix4 &mInv)
{
}


bool Region3d::isEmpty() const
{
	return true;
}


bool Region3d::contains(const Point3 &p) const
{
	return false;
}

bool Region3d::containsPartOf(const Segment3 &seg) const
{
	return false;
}

bool Region3d::containsAllOf(const Segment3 &seg) const
{
	return false;
}

bool Region3d::containsPartOf(const Polygon3 &poly) const
{
	return false;
}

bool Region3d::containsAllOf(const Polygon3 &poly) const
{
	return false;
}


void Region3d::paint() const
{
}





/********************************************************************************
						ProjectedRegion3d
 ********************************************************************************/

RT_IMPLEMENT_TYPE( ProjectedRegion3d, Region3d )


static Polygon2 projectedPoly;
static Polygon3 clipPoly0;
static Polygon3 clipPoly1;
static Array<Triangle3> clippedTriangles;


ProjectedRegion3d::ProjectedRegion3d() : Region3d()
{
	region2d = new Region2d();
}

ProjectedRegion3d::ProjectedRegion3d(const ProjectedRegion3d &b)
										: Region3d( b )
{
	region2d = b.region2d->cloneTyped<Region2d>();
	projection = b.projection;
}

ProjectedRegion3d::ProjectedRegion3d(const Region2d &r2d, const Projection &proj)
										: Region3d()
{
	region2d = r2d.cloneTyped<Region2d>();
	projection = proj;
}

ProjectedRegion3d::~ProjectedRegion3d()
{
	if ( region2d != NULL )
	{
		delete region2d;
	}
}



void ProjectedRegion3d::transform(const Matrix4 &m, const Matrix4 &mInv)
{
	projection = projection.preTransformed( mInv, m );
}


bool ProjectedRegion3d::isEmpty() const
{
	return false;
}


bool ProjectedRegion3d::contains(const Point3 &p) const
{
	if ( p < projection.getNearClipPlane()  ||  p < projection.getFarClipPlane() )
	{
		return false;
	}

	return region2d->contains( projection.getWorldToScreenMatrix().transformHomogeneous2d( p ) );
}

bool ProjectedRegion3d::containsPartOf(const Segment3 &seg) const
{
	Side sideOfNearPlane = seg.side( projection.getNearClipPlane() );
	if ( sideOfNearPlane == SIDE_NEGATIVE )
	{
		return false;
	}

	Side sideOfFarPlane = seg.side( projection.getFarClipPlane() );
	if ( sideOfFarPlane == SIDE_NEGATIVE )
	{
		return false;
	}

	if ( sideOfNearPlane == SIDE_BOTH  ||  sideOfFarPlane == SIDE_BOTH )
	{
		Segment3 tempSeg = seg;

		if ( sideOfNearPlane == SIDE_BOTH )
		{
			tempSeg.clip( projection.getNearClipPlane() );
		}
		if ( sideOfFarPlane == SIDE_BOTH )
		{
			tempSeg.clip( projection.getFarClipPlane() );
		}

		return region2d->containsPartOf( projection.getWorldToScreenMatrix().transformHomogeneous2d( tempSeg ) );
	}
	else
	{
		return region2d->containsPartOf( projection.getWorldToScreenMatrix().transformHomogeneous2d( seg ) );
	}
}

bool ProjectedRegion3d::containsAllOf(const Segment3 &seg) const
{
	Side sideOfNearPlane = seg.side( projection.getNearClipPlane() );
	if ( sideOfNearPlane == SIDE_NEGATIVE  ||  sideOfNearPlane == SIDE_BOTH )
	{
		return false;
	}

	Side sideOfFarPlane = seg.side( projection.getFarClipPlane() );
	if ( sideOfFarPlane == SIDE_NEGATIVE  ||  sideOfFarPlane == SIDE_BOTH )
	{
		return false;
	}

	return region2d->containsAllOf( projection.getWorldToScreenMatrix().transformHomogeneous2d( seg ) );
}

bool ProjectedRegion3d::containsPartOf(const Polygon3 &poly) const
{
	Side sideOfNearPlane = poly.side( projection.getNearClipPlane(), true );
	if ( sideOfNearPlane == SIDE_NEGATIVE )
	{
		return false;
	}

	Side sideOfFarPlane = poly.side( projection.getFarClipPlane(), true );
	if ( sideOfFarPlane == SIDE_NEGATIVE )
	{
		return false;
	}

	if ( sideOfNearPlane == SIDE_BOTH  ||  sideOfFarPlane == SIDE_BOTH )
	{
		Polygon3 *clipped = NULL;

		// Clip the polygon and generate a triangle list
		if ( sideOfNearPlane == SIDE_BOTH )
		{
			Polygon3::sutherlandHodgmanClip( clipPoly0, poly, projection.getNearClipPlane() );

			if ( sideOfFarPlane == SIDE_BOTH )
			{
				Polygon3::sutherlandHodgmanClip( clipPoly1, clipPoly0, projection.getFarClipPlane() );
				clipped = &clipPoly1;
			}
			else
			{
				clipped = &clipPoly0;
			}
		}
		else
		{
			Polygon3::sutherlandHodgmanClip( clipPoly0, poly, projection.getFarClipPlane() );
			clipped = &clipPoly0;
		}


		// Project the clipped polygon
		projectPolygon( projectedPoly, *clipped );
		return region2d->containsPartOf( projectedPoly );
	}
	else
	{
		projectPolygon( projectedPoly, poly );
		return region2d->containsPartOf( projectedPoly );
	}
}

bool ProjectedRegion3d::containsAllOf(const Polygon3 &poly) const
{
	for (int polyI = 0; polyI < poly.size(); polyI++)
	{
		const Point3 &p = poly[polyI];
		if ( projection.getNearClipPlane().side( p, true )  !=  SIDE_POSITIVE )
		{
			return false;
		}
		else if ( projection.getFarClipPlane().side( p, true )  !=  SIDE_POSITIVE )
		{
			return false;
		}
	}

	projectPolygon( projectedPoly, poly );
	return region2d->containsAllOf( projectedPoly );
}


void ProjectedRegion3d::paint() const
{
	Array<Point2> points2d;

	region2d->generatePoints( points2d );


	Array<Point3> points[2];

	points[0].reserve( points2d.size() );
	points[1].reserve( points2d.size() );
	for (int i = 0; i < points2d.size(); i++)
	{
		//use a screen depth co-ordinate of 0.0001; this ensures that the front
		//end of the selection region is SLIGHTLY in front of the near clip plane,
		//so that it doesn't get clipped
		points[0].push_back( projection.getScreenToWorldMatrix().transformHomogeneous( points2d[i], 0.0001 ) );
		points[1].push_back( projection.getScreenToWorldMatrix().transformHomogeneous( points2d[i], 1.0 ) );
	}


	//quads
	glBegin( GL_QUAD_STRIP );
	for (int i = 0; i < points2d.size(); i++)
	{
		glVertex3( points[0][i] );
		glVertex3( points[1][i] );
	}
	glEnd();

	glLineWidth( 2.0 );

	//lines along
	glBegin( GL_LINES );
	for (int i = 0; i < points2d.size(); i++)
	{
		glVertex3( points[0][i] );
		glVertex3( points[1][i] );
	}
	glEnd();

	//lines around
	glBegin( GL_LINE_STRIP );
	for (int i = 0; i < points2d.size(); i++)
	{
		glVertex3( points[0][i] );
	}
	glEnd();

	glBegin( GL_LINE_STRIP );
	for (int i = 0; i < points2d.size(); i++)
	{
		glVertex3( points[1][i] );
	}
	glEnd();

	glLineWidth( 1.0 );
}



Region2d & ProjectedRegion3d::getRegion2d()
{
	return *region2d;
}

void ProjectedRegion3d::setRegion2d(const Region2d &r2d)
{
	if ( region2d != NULL )
	{
		delete region2d;
	}
	region2d = r2d.cloneTyped<Region2d>();
}



void ProjectedRegion3d::projectPolygon(Polygon2 &projectedPoly, const Polygon3 &poly3) const
{
	projectedPoly.resize( poly3.size() );

	for (int polyI = 0; polyI < poly3.size(); polyI++)
	{
		projectedPoly[polyI] = projection.getWorldToScreenMatrix().transformHomogeneous2d( poly3[polyI] );
	}
}




#endif
