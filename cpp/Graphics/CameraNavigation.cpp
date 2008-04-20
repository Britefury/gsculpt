//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CAMERANAVIGATION_CPP__
#define CAMERANAVIGATION_CPP__

#include <Graphics/CameraNavigation.h>





/******************************************************************************
					CameraNavigation
 ******************************************************************************/

CameraNavigation::CameraNavigation()
{
}

CameraNavigation::~CameraNavigation()
{
}



void CameraNavigation::rotate(CameraPosition *cam, double xrot, double yrot)
{
}

void CameraNavigation::pan(CameraPosition *cam, const Vector3 &v)
{
}

void CameraNavigation::moveInZ(CameraPosition *cam, double screenZ, double worldZ)
{
}





/******************************************************************************
					OrbitalCameraNavigation
 ******************************************************************************/

OrbitalCameraNavigation::OrbitalCameraNavigation() : CameraNavigation()
{
	zoompp = 1.00695555;
}

OrbitalCameraNavigation::OrbitalCameraNavigation(double zoomPerPixel)
													  : CameraNavigation()
{
	zoompp = zoomPerPixel;
}


void OrbitalCameraNavigation::updatePositions(const Vector3 &forwardAxis,
															 double targetDistance,
															 Point3 &cameraPosition,
															 Point3 &targetPosition) const
{
	cameraPosition = targetPosition  -  forwardAxis * targetDistance;
}


void OrbitalCameraNavigation::rotate(CameraPosition *cam, double xrot, double yrot)
{
	cam->rotateAroundUpAxis( xrot, this );
	cam->rotateAroundRightAxis( yrot, this );
}

void OrbitalCameraNavigation::pan(CameraPosition *cam, const Vector3 &v)
{
	cam->translate( -v );
}

void OrbitalCameraNavigation::moveInZ(CameraPosition *cam,
												  double screenZ, double worldZ)
{
	cam->zoom( pow( zoompp, -screenZ ) );
}





/******************************************************************************
					FirstPersonCameraNavigation
 ******************************************************************************/

FirstPersonCameraNavigation::FirstPersonCameraNavigation() : CameraNavigation()
{
}


void FirstPersonCameraNavigation::updatePositions(const Vector3 &forwardAxis,
																  double targetDistance,
																  Point3 &cameraPosition,
																  Point3 &targetPosition) const
{
	targetPosition = cameraPosition  +  forwardAxis * targetDistance;
}


void FirstPersonCameraNavigation::rotate(CameraPosition *cam,
													  double xrot, double yrot)
{
	cam->rotateAroundUpAxis( xrot, this );
	cam->rotateAroundRightAxis( yrot, this );
}

void FirstPersonCameraNavigation::pan(CameraPosition *cam, const Vector3 &v)
{
	cam->translate( -v );
}

void FirstPersonCameraNavigation::moveInZ(CameraPosition *cam,
														double screenZ, double worldZ)
{
	cam->translate( cam->getForwardAxis() * worldZ );
}


#endif
