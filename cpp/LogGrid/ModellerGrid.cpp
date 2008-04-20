//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MODELLERGRID_CPP__
#define MODELLERGRID_CPP__

#include <math.h>

#include <algorithm>
#include <sstream>

#include <Graphics/Drawing.h>

#include <LogGrid/Grid.h>
#include <LogGrid/ModellerGrid.h>




ModellerGrid::ModellerGrid()
{
	g = CGrid();
	gridRequiresRefreshFlag = true;
	gridPlane = GRIDPLANE_Y;
	enabledFlag = true;
}

ModellerGrid::~ModellerGrid()
{
}



void ModellerGrid::setGrid(const CGrid &gr)
{
	g = gr;
	gridRequiresRefreshFlag = true;
}


void ModellerGrid::setGridPlane(GridPlane plane)
{
	gridPlane = plane;
	gridRequiresRefreshFlag = true;
}

ModellerGrid::GridPlane ModellerGrid::getGridPlane() const
{
	return gridPlane;
}



void ModellerGrid::setEnabled(bool enabled)
{
	enabledFlag = enabled;
}

bool ModellerGrid::isEnabled() const
{
	return enabledFlag;
}



const Array<Colour3f> & ModellerGrid::getColours() const
{
	return g.getColours();
}

void ModellerGrid::setColours(const Array<Colour3f> &c)
{
	g.setColours( c );
	gridRequiresRefreshFlag = true;
}



boost::python::list ModellerGrid::py_getColours() const
{
	const Array<Colour3f> &colours = getColours();
	boost::python::list result;
	for (int i = 0; i < colours.size(); i++)
	{
		result.append( colours[i] );
	}
	return result;
}

void ModellerGrid::py_setColours(boost::python::list c)
{
	Array<Colour3f> colours;
	boost::python::object lenObj = c.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numColours = lenExtract;
		colours.reserve( numColours );
		for (int i = 0; i < numColours; i++)
		{
			boost::python::object colourObj = c[i];
			boost::python::extract<Colour3f&> colourExtract( colourObj );
			if ( colourExtract.check() )
			{
				colours.push_back( colourExtract );
			}
		}
		setColours( colours );
	}
}




void ModellerGrid::paint3d(Camera *camera)
{
	if ( !enabledFlag )
	{
		return;
	}


	int depthFunction;

	if ( gridRequiresRefreshFlag )
	{
		computeGrid( camera );
	}


	//GRID OFFSET
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glTranslated( gridOffset.x, gridOffset.y, gridOffset.z );


	if ( camera->getCameraProjection()->getProjectionType() == CameraProjection::ORTHOGRAPHIC )
	{
		//orhtographic
		glDisable( GL_DEPTH_TEST );
	}
	else if ( camera->getCameraProjection()->getProjectionType() == CameraProjection::PERSPECTIVE )
	{
		glGetIntegerv( GL_DEPTH_FUNC, &depthFunction );
		glDepthFunc( GL_ALWAYS );
	}


	bool labelGrid = camera->getCameraPosition()->isAxisAligned()  &&  camera->getCameraProjection()->getProjectionType() == CameraProjection::ORTHOGRAPHIC;

	for (int i = 0; i < lineList.size(); i++)
	{
		LevelLineList &level = lineList[i];

		glColor3( level.col );

		glBegin( GL_LINES );
		for (int j = 0; j < level.hlines.size(); j++)
		{
			Segment3 line = level.hlines[j].line;

			glVertex3d( line.a.x, line.a.y, line.a.z );
			glVertex3d( line.b.x, line.b.y, line.b.z );
		}
		for (int j = 0; j < level.vlines.size(); j++)
		{
			Segment3 line = level.vlines[j].line;

			glVertex3d( line.a.x, line.a.y, line.a.z );
			glVertex3d( line.b.x, line.b.y, line.b.z );
		}
		glEnd();
	}


	if ( camera->getCameraProjection()->getProjectionType() == CameraProjection::ORTHOGRAPHIC )
	{
		glEnable( GL_DEPTH_TEST );
		glGetIntegerv( GL_DEPTH_FUNC, &depthFunction );
		glDepthFunc( GL_ALWAYS );
	}


	if ( labelGrid )
	{
		//i starts at 1  -  don't label finest grid; only label larger ones
		for (int i = 1; i < lineList.size(); i++)
		{
			LevelLineList &level = lineList[i];

			glColor3( level.col );
			for (int j = 0; j < level.hlines.size(); j++)
			{
				paintLabel( level.hlines[j], camera );
			}
			for (int j = 0; j < level.vlines.size(); j++)
			{
				paintLabel( level.vlines[j], camera );
			}
		}
	}


	glDepthFunc( depthFunction );


	//REMOVE GRID OFFSET TRANSFORMATION
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}


void ModellerGrid::worldToScreenChangedNotify()
{
	gridRequiresRefreshFlag = true;
}





Axis ModellerGrid::getBestAxis(Vector3 forwardVector)
{
	forwardVector.x = fabs( forwardVector.x );
	forwardVector.y = fabs( forwardVector.y );
	forwardVector.z = fabs( forwardVector.z );

	double bestComponent = forwardVector.x;
	Axis bestAxis = AXIS_NEGATIVE_X;


	if ( forwardVector.y > bestComponent )
	{
		bestComponent = forwardVector.y;
		bestAxis = AXIS_NEGATIVE_Y;
	}

	if ( forwardVector.z > bestComponent )
	{
		bestComponent = forwardVector.z;
		bestAxis = AXIS_Z;
	}


	return bestAxis;
}

Axis ModellerGrid::computeGridPlaneNormalAxis(Camera *cam)
{
	switch ( gridPlane )
	{
	case GRIDPLANE_X:
		return AXIS_NEGATIVE_X;
	case GRIDPLANE_Y:
		return AXIS_NEGATIVE_Y;
	case GRIDPLANE_Z:
		return AXIS_Z;
	case GRIDPLANE_AUTO:
		return getBestAxis( cam->getCameraPosition()->getForwardAxis() );
	default:
		return AXIS_NEGATIVE_Y;
	}
}

BBox3 ModellerGrid::computeVisibleArea(Camera *cam, const Plane &gridPlane)
{
	//to compute the area of the grid plane that is visible, shoot the four corner
	//rays from the camera at the grid plane
	Segment3 cornerRays[4];

	cornerRays[0] = cam->getTopLeftCornerRay();
	cornerRays[1] = cam->getTopRightCornerRay();
	cornerRays[2] = cam->getBottomLeftCornerRay();
	cornerRays[3] = cam->getBottomRightCornerRay();

	BBox3 visibleArea;


	//count the number of intersections between the corner rays and the grid
	//plane, and form a bounding box surrounding them
	int hitCount = 0;
	for (int i = 0; i < 4; i++)
	{
		Point3 intersection;
		double t;

		if ( cornerRays[i].intersect( gridPlane, t, intersection ) )
		{
			if ( hitCount == 0 )
			{
				visibleArea = BBox3( intersection );
			}
			else
			{
				visibleArea.addPoint( intersection );
			}

			hitCount++;
		}
	}


	//the bounding box is only visible if all four rays hit the plane, so:
	if ( hitCount == 4 )
	{
		//all four rays hit: return the box
		return visibleArea;
	}
	else
	{
		//else, use the area of the grid nearest to the camera which is 100*d
		//in size where d is the distance from the camera to the grid plane
		double distanceFromCameraToGridPlane = gridPlane.distanceTo( cam->getCameraPosition()->getCameraPosition() );

		Point3 closestPointOnPlaneToCam = cam->getCameraPosition()->getCameraPosition() - gridPlane.n * distanceFromCameraToGridPlane;

		//compute the size of the offset from the nearest point to the edges
		//(50 * d)
		double offsetSize = distanceFromCameraToGridPlane * 50.0;

		Vector3 visibleAreaOffset = Vector3( offsetSize, offsetSize, offsetSize );

		//generate a bounding box
		return BBox3( closestPointOnPlaneToCam - visibleAreaOffset, closestPointOnPlaneToCam + visibleAreaOffset );
	}
}


void ModellerGrid::paintLabel(const Grid::GridLine &l, Camera *cam)
{
	Segment3 clipped = l.line;



	if ( cam->getFrustum().clip( clipped ) )
	{
		//project onto screen
		Segment3 screen;
		screen.a = cam->transformWorldToScreen( clipped.a );
		screen.b = cam->transformWorldToScreen( clipped.b );

		//get the maximum dimension of the line (maximum of delta-x and delta-y)
		double maxDimension = std::max( fabs( screen.b.x - screen.a.x ), fabs( screen.b.y - screen.a.y ) );
		double pixelFraction = 1.0 / maxDimension;

		//compute the vector by which the end points must be moved
		Vector3 direction = clipped.getDirection();
		Vector3 deltaP = direction * pixelFraction;

		//move the end points inwards
		clipped.a += deltaP;
		clipped.b -= deltaP;

		//re-project
		screen.a = cam->transformWorldToScreen( clipped.a );
		screen.b = cam->transformWorldToScreen( clipped.b );



		Point3 textPosition;

		if ( fabs( screen.b.x - screen.a.x )  >  fabs( screen.b.y - screen.a.y ) )
		{
			textPosition = screen.a.x < screen.b.x ? clipped.a : clipped.b;
		}
		else
		{
			textPosition = screen.a.y < screen.b.y ? clipped.a : clipped.b;
		}



		std::ostringstream stream;
		stream << l.labelValue << (char)0x00;


		glDrawString3( textPosition, stream.str().c_str() );
	}
}

void ModellerGrid::computeGrid(Camera *cam)
{
	//get the grid plane
	Axis gridPlaneNormalAxis = computeGridPlaneNormalAxis( cam );
	Plane gridPlane = Plane( Vector3( gridPlaneNormalAxis ), 0.0 );

	//compute the matrices going from grid-plane -> z-plane and z-plane -> grid-plane
	Matrix4 gridToZPlane = Matrix4::axisToAxis( gridPlaneNormalAxis, AXIS_Z );
	Matrix4 zPlaneToGrid = Matrix4::axisToAxis( AXIS_Z, gridPlaneNormalAxis );


	//compute the visible area of the grid
	BBox3 visibleArea = computeVisibleArea( cam, gridPlane );

	//transform into z-plane
	Point3 lowerInZPlane = visibleArea.getLower() * gridToZPlane;
	Point3 upperInZPlane = visibleArea.getUpper() * gridToZPlane;

	//set the corners of the grid
	g.setCorners( lowerInZPlane, upperInZPlane );

	//generate the grid lines
	lineList.clear();

	for (int i = g.nLevels() - 1; i >= 0; i--)
	{
		LevelLineList lev;
		lineList.push_back( lev );
		int j = lineList.size() - 1;

		LevelLineList &level = lineList[j];

		level.col = g.getGridColour( j );

	 	g.generateHLines( i, level.hlines );
	 	g.generateVLines( i, level.vlines );


	 	for (int j = 0; j < level.hlines.size(); j++)
	 	{
	 		level.hlines[j].line *= zPlaneToGrid;
	 	}
	 	for (int j = 0; j < level.vlines.size(); j++)
	 	{
	 		level.vlines[j].line *= zPlaneToGrid;
	 	}
	 }



	//COMPUTE GRID OFFSET
	//This ensures that when orthographic projection is being used,
	//the plane on which the grid lies WHEN IT IS PAINTED, passes through
	//the position of the camera target.
	//This is done by offsetting the grid
	if ( cam->getCameraProjection()->getProjectionType() == CameraProjection::ORTHOGRAPHIC )
	{
		const Vector3 &forward = cam->getCameraPosition()->getForwardAxis();
		const Vector3 &normal = gridPlane.n;

		const Point3 &target = cam->getCameraPosition()->getTargetPosition();
		double distanceToTarget = gridPlane.distanceTo( target );

		double t = distanceToTarget / normal.dot( forward );

		gridOffset = forward * t;
	}
	else
	{
		gridOffset = Vector3();
	}


	gridRequiresRefreshFlag = false;
}



#endif
