//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POLYLINEPATH_CPP__
#define POLYLINEPATH_CPP__

#include <Math/index.h>
#include <Math/epsilon.h>
#include <Math/Point3.h>

#include <Polyline/Polyline.h>


#include <Polyline/PolylinePath.h>




static void computePathDirection(const Point3 &p0, const Point3 &p1, Vector3 &direction)
{
	direction = p1 - p0;

	if ( direction.sqrLength() > EPSILON )
	{
		direction.normalise();
	}
}

static void computePathDirection(const Point3 &p0, const Point3 &p1, const Point3 &p2, Vector3 &direction, Matrix4 *correctiveScale)
{
	Vector3 p0ToP1 = p1 - p0;
	Vector3 p1ToP2 = p2 - p1;

	if ( p0ToP1.sqrLength() > EPSILON )
	{
		p0ToP1.normalise();
	}
	if ( p1ToP2.sqrLength() > EPSILON )
	{
		p1ToP2.normalise();
	}

	direction = p0ToP1 + p1ToP2;
	if ( direction.sqrLength() > EPSILON )
	{
		direction.normalise();
	}


	if ( correctiveScale != NULL )
	{
		Vector3 tangent = ( p0ToP1 + p1ToP2 ) * 0.5  -  p0ToP1;

		if ( tangent.sqrLength() < SQR_EPSILON )
		{
			*correctiveScale = Matrix4();
			return;
		}
		else
		{
			tangent.normalise();

			double cosTheta = tangent.dot( -p0ToP1 );
			cosTheta = clamp( cosTheta, -1.0, 1.0 );
			double sinTheta = sin( acos( cosTheta ) );
			double scaleFactor = 1.0;

			if ( fabs( sinTheta )  >  EPSILON )
			{
			 	scaleFactor = 1.0 / sinTheta;
			}

			*correctiveScale = Matrix4::scale( tangent, scaleFactor );
		}
	}
}


static Matrix4 computeCorrection(const Matrix4 &initialMatrix, Vector3 initialDirection)
{
	Vector3 axis;


	if ( fabs( initialDirection.x )  <  fabs( initialDirection.y ) )
	{
		axis = Vector3( 1.0, 0.0, 0.0 );
	}
	else
	{
		axis = Vector3( 0.0, 1.0, 0.0 );
	}


	Vector3 polylineSpace = axis * initialMatrix.inverse();

	polylineSpace.z = 0.0;
	polylineSpace.normalise();


	return Matrix4::axisToAxis( axis, polylineSpace );
}


static void computePathMatrices(const Polyline &path, Array<Matrix4> &matrices, Matrix4 cumulativeXform, Vector3 prevDir, bool correctiveScaleFlag,
								PolylinePathOrientation orientation)
{
	matrices.reserve( path.size() );


	if ( orientation == POLYLINEPATHORIENTATION_NOORIENTATION )
	{
		for (int i = 0; i < path.size(); i++)
		{
			matrices.push_back( Matrix4::translate( path[i].getPosition().toVector3() ) );
		}
	}
	else
	{
		Polyline orientationPath = path;

		if ( orientation == POLYLINEPATHORIENTATION_INXPLANE )
		{
			orientationPath.flatten( AXIS_X );
		}
		else if ( orientation == POLYLINEPATHORIENTATION_INYPLANE )
		{
			orientationPath.flatten( AXIS_Y );
		}
		else if ( orientation == POLYLINEPATHORIENTATION_INZPLANE )
		{
			orientationPath.flatten( AXIS_Z );
		}



		Matrix4 correctiveScaleMatrix, *correctiveScale;
		correctiveScale = correctiveScaleFlag  ?  &correctiveScaleMatrix : NULL;




		Vector3 curDir;

		int pathSize = path.size();

		for (int i = 0; i < pathSize; i++)
		{
			if ( i == 0  &&  !orientationPath.isClosed() )
			{
				computePathDirection( orientationPath[0].getPosition(), orientationPath[1].getPosition(), curDir );
				correctiveScaleMatrix = Matrix4();
			}
			else if ( i == pathSize - 1  &&  !orientationPath.isClosed() )
			{
				computePathDirection( orientationPath[ pathSize - 2 ].getPosition(), orientationPath[ pathSize - 1 ].getPosition(), curDir );
				correctiveScaleMatrix = Matrix4();
			}
			else
			{
				int prev = prevIndex( i, pathSize );
				int next = nextIndex( i, pathSize );
				computePathDirection( orientationPath[prev].getPosition(),
									orientationPath[i].getPosition(),
									orientationPath[next].getPosition(),
									curDir, correctiveScale );
			}


			Matrix4 m = Matrix4::axisToAxis( prevDir, curDir );

			if ( i == 0 )
			{
				m = computeCorrection( m, curDir ) * m;
			}

			cumulativeXform = cumulativeXform * m;

			if ( correctiveScaleFlag )
			{
				matrices.push_back(
						cumulativeXform *
						correctiveScaleMatrix *
						Matrix4::translate( path[i].getPosition().toVector3() ) );
			}
			else
			{
				matrices.push_back(
						cumulativeXform *
						Matrix4::translate( path[i].getPosition().toVector3() ) );
			}

			prevDir = curDir;
		}
	}
}


void computePathMatrices(const Polyline &path, Array<Matrix4> &matrices, Axis sourceAxis, bool correctiveScaleFlag, PolylinePathOrientation orientation)
{
	computePathMatrices( path, matrices, Matrix4::axisToAxis( sourceAxis, AXIS_NEGATIVE_Z ), Vector3( AXIS_NEGATIVE_Z ), correctiveScaleFlag, orientation );
}

void computePathMatrices(const Polyline &path, Array<Matrix4> &matrices, const Vector3 &sourceAxis, bool correctiveScaleFlag, PolylinePathOrientation orientation)
{
	computePathMatrices( path, matrices, Matrix4(), sourceAxis, correctiveScaleFlag, orientation );
}


#endif
