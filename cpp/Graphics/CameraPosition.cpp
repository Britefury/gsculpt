//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CAMERAPOSITION_CPP__
#define CAMERAPOSITION_CPP__

#include <Graphics/GLMatrix.h>

#include <Graphics/CameraPosition.h>



/******************************************************************************
						CameraRotator
 ******************************************************************************/

CameraRotator::~CameraRotator()
{
}


void CameraRotator::updatePositions(const Vector3 &forwardAxis, double targetDistance, Point3 &cameraPosition, Point3 &targetPosition) const
{
}




/******************************************************************************
					CameraPositionListener
 ******************************************************************************/

CameraPositionListener::~CameraPositionListener()
{
}




/******************************************************************************
						CameraPosition
 ******************************************************************************/

CameraPosition::CameraPosition()
{
	targetPosition = Point3( 0.0, 0.0, 0.0 );
	cameraPosition = Point3( 0.0, 0.0, 200.0 );

	forwardAxis =	Vector3( 0.0, 0.0, -1.0 );
	upAxis =			Vector3( 0.0, 1.0, 0.0 );
	rightAxis =		Vector3( 1.0, 0.0, 0.0 );

	cameraToTargetDistance = 200.0;

	axisAlignedFlag = true;

	listener = NULL;

	cameraToWorldModified();
}




void CameraPosition::setListener(CameraPositionListener *l)
{
	listener = l;
}



bool CameraPosition::isAxisAligned() const
{
	return axisAlignedFlag;
}



double CameraPosition::transformWorldToCameraZ(const Point3 &world) const
{
	return worldToCameraMatrix.transformZ( world );
}



const Vector3 &CameraPosition::getForwardAxis() const
{
	return forwardAxis;
}

const Vector3 &CameraPosition::getUpAxis() const
{
	return upAxis;
}

const Vector3 &CameraPosition::getRightAxis() const
{
	return rightAxis;
}


void CameraPosition::setGLModelViewMatrix() const
{
	glLoadIdentity();
	glMultMatrix( worldToCameraMatrix );
}



const Matrix4 &CameraPosition::getWorldToCameraMatrix() const
{
	return worldToCameraMatrix;
}

const Matrix4 &CameraPosition::getCameraToWorldMatrix() const
{
	return cameraToWorldMatrix;
}



const Point3 &CameraPosition::getCameraPosition() const
{
	return cameraPosition;
}

const Point3 &CameraPosition::getTargetPosition() const
{
	return targetPosition;
}



void CameraPosition::front()
{
	forwardAxis =	Vector3( 0.0, 0.0, -1.0 );
	upAxis =			Vector3( 0.0, 1.0, 0.0 );
	rightAxis =		Vector3( 1.0, 0.0, 0.0 );
	cameraPosition = targetPosition  -  forwardAxis * cameraToTargetDistance;
	cameraToWorldModified();
	axisAlignedFlag = true;
}

void CameraPosition::back()
{
	forwardAxis =	Vector3( 0.0, 0.0, 1.0 );
	upAxis =			Vector3( 0.0, 1.0, 0.0 );
	rightAxis =		Vector3( -1.0, 0.0, 0.0 );
	cameraPosition = targetPosition  -  forwardAxis * cameraToTargetDistance;
	cameraToWorldModified();
	axisAlignedFlag = true;
}

void CameraPosition::top()
{
	forwardAxis =	Vector3( 0.0, -1.0, 0.0 );
	upAxis =			Vector3( 0.0, 0.0, -1.0 );
	rightAxis =		Vector3( 1.0, 0.0, 0.0 );
	cameraPosition = targetPosition  -  forwardAxis * cameraToTargetDistance;
	cameraToWorldModified();
	axisAlignedFlag = true;
}

void CameraPosition::bottom()
{
	forwardAxis =	Vector3( 0.0, 1.0, 0.0 );
	upAxis =			Vector3( 0.0, 0.0, 1.0 );
	rightAxis =		Vector3( 1.0, 0.0, 0.0 );
	cameraPosition = targetPosition  -  forwardAxis * cameraToTargetDistance;
	cameraToWorldModified();
	axisAlignedFlag = true;
}

void CameraPosition::left()
{
	forwardAxis =	Vector3( 1.0, 0.0, 0.0 );
	upAxis =			Vector3( 0.0, 1.0, 0.0 );
	rightAxis =		Vector3( 0.0, 0.0, 1.0 );
	cameraPosition = targetPosition  -  forwardAxis * cameraToTargetDistance;
	cameraToWorldModified();
	axisAlignedFlag = true;
}

void CameraPosition::right()
{
	forwardAxis =	Vector3( -1.0, 0.0, 0.0 );
	upAxis =			Vector3( 0.0, 1.0, 0.0 );
	rightAxis =		Vector3( 0.0, 0.0, -1.0 );
	cameraPosition = targetPosition  -  forwardAxis * cameraToTargetDistance;
	cameraToWorldModified();
	axisAlignedFlag = true;
}



void CameraPosition::rotateAroundRightAxis(double angle, const CameraRotator *r)
{
	rotate( Matrix4::rotate( rightAxis, angle ), r );
}

void CameraPosition::rotateAroundUpAxis(double angle, const CameraRotator *r)
{
	rotate( Matrix4::rotateY( angle ), r );
}



void CameraPosition::translate(const Vector3 &translation)
{
	targetPosition += translation;
	cameraPosition += translation;
	cameraToWorldModified();
}


void CameraPosition::zoom(double z)
{
	setCameraToTargetDistance( getCameraToTargetDistance() * z );
}


void CameraPosition::origin()
{
	translate( -targetPosition.toVector3() );
}

void CameraPosition::viewZ0()
{
	double distance = targetPosition.dot( getForwardAxis() );
	Vector3 translation = getForwardAxis() * -distance;

	translate( translation );
}



void CameraPosition::lookat(const Point3 &pos, const Point3 &target)
{
	cameraToTargetDistance = pos.distanceTo( target );

	Matrix4 camMatrix = Matrix4::lookat( pos, target, Vector3( 0.0, 1.0, 0.0 ) );
	forwardAxis = Vector3( 0.0, 0.0, -1.0 ) * camMatrix;
	upAxis = Vector3( 0.0, 1.0, 0.0 ) * camMatrix;
	rightAxis = Vector3( 1.0, 0.0, 0.0 ) * camMatrix;

	targetPosition = target;
	cameraPosition = pos;
	axisAlignedFlag = false;

	cameraToTargetDistanceModified();
	cameraToWorldModified();
}



double CameraPosition::getCameraToTargetDistance() const
{
	return cameraToTargetDistance;
}

void CameraPosition::setCameraToTargetDistance(double d)
{
	cameraToTargetDistance = d;

	cameraPosition = targetPosition  -  forwardAxis * cameraToTargetDistance;
	cameraToTargetDistanceModified();
	cameraToWorldModified();
}



void CameraPosition::rotate(const Matrix4 &rotation, const CameraRotator *r)
{
	forwardAxis *= rotation;
	upAxis *= rotation;
	rightAxis *= rotation;
	r->updatePositions( forwardAxis, cameraToTargetDistance,
							  cameraPosition, targetPosition );
	cameraToWorldModified();
	axisAlignedFlag = false;
}



void CameraPosition::cameraToWorldModified()
{
	cameraToWorldMatrix = Matrix4( rightAxis, upAxis, -forwardAxis,
											 cameraPosition.toVector3() );
	worldToCameraMatrix = cameraToWorldMatrix.rigidBodyInverse();

	if ( listener != NULL )
	{
		listener->worldToCameraChanged();
	}
}

void CameraPosition::cameraToTargetDistanceModified()
{
	if ( listener != NULL )
	{
		listener->cameraToTargetDistanceChanged();
	}
}




#endif
