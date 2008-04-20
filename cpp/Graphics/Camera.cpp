//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CAMERA_CPP__
#define CAMERA_CPP__

#include <PlatformSpecific/IncludeGL.h>

#include <Math/Segment3.h>

#include <Graphics/Camera.h>

#include <Graphics/GLMatrix.h>





/******************************************************************************
												Camera
 ******************************************************************************/

Camera::Camera(double screenWidth, double screenHeight, double fieldOfViewY)
{
	screenW = screenWidth;
	screenH = screenHeight;
	fovY = fieldOfViewY;

	position = NULL;
	projection = NULL;
}

Camera::~Camera()
{
}



void Camera::setScreenDimensions(double screenWidth, double screenHeight)
{
	screenW = screenWidth;
	screenH = screenHeight;
	projection->setScreenDimensions( screenW, screenH );
}

void Camera::setFieldOfView(double fieldOfViewY)
{
	fovY = fieldOfViewY;
	projection->setFieldOfView( fovY );
}



CameraPosition * Camera::getCameraPosition()
{
	return position;
}

void Camera::setCameraPosition(CameraPosition *pos)
{
	if ( position != NULL )
	{
		position->setListener( NULL );
	}

	position = pos;
	position->setListener( this );

	if ( projection != NULL )
	{
		projection->setCameraToTargetDistance( position->getCameraToTargetDistance() );
	}

	refreshCamera();
}


CameraProjection * Camera::getCameraProjection()
{
	return projection;
}

void Camera::setCameraProjection(CameraProjection *proj)
{
	if ( projection != NULL )
	{
		projection->setListener( NULL );
	}

	projection = proj;
	if ( position != NULL )
	{
		projection->setProjectionParameters( position->getCameraToTargetDistance(), screenW, screenH, fovY );
	}

	projection->setListener( this );

	refreshCamera();
}




void Camera::setGLModelViewMatrix() const
{
	position->setGLModelViewMatrix();
}

void Camera::setGLProjectionMatrix() const
{
	projection->setGLProjectionMatrix();
}

void Camera::glProjectionMatrixIdentity() const
{
	glLoadIdentity();
	gluOrtho2D( 0.0, projection->getScreenWidth(), 0.0, projection->getScreenHeight() );
}

void Camera::glModelViewMatrixIdentity() const
{
	glLoadIdentity();
}




double Camera::getCameraToTargetDistance() const
{
	return position->getCameraToTargetDistance();
}



Plane Camera::getNearClipPlane() const
{
	Point3 nearClipPoint = position->getCameraPosition()  +  position->getForwardAxis() * getNearClipDistance();
	return Plane( position->getForwardAxis(), nearClipPoint );
}

Plane Camera::getFarClipPlane() const
{
	Point3 farClipPoint = position->getCameraPosition()  +  position->getForwardAxis() * getFarClipDistance();
	return Plane( -position->getForwardAxis(), farClipPoint );
}


double Camera::getNearClipDistance() const
{
	return projection->getNearClipDistance();
}

double Camera::getFarClipDistance() const
{
	return projection->getFarClipDistance();
}



const Frustum &Camera::getFrustum() const
{
	return worldSpaceFrustum;
}



const Matrix4 &Camera::getWorldToScreenMatrix() const
{
	return worldToScreenMatrix;
}

const Matrix4 &Camera::getScreenToWorldMatrix() const
{
	return screenToWorldMatrix;
}



Projection Camera::getProjection() const
{
	return Projection( getWorldToScreenMatrix(), getScreenToWorldMatrix(), getNearClipPlane(), getFarClipPlane() );
}



double Camera::transformDistanceAcrossScreenToDistanceThruWorld(double d) const
{
	return projection->transformDistanceAcrossScreenToDistanceThruWorld( d );
}

double Camera::transformDistanceAcrossScreenToDistanceThruWorld(double d, double atDist) const
{
	return projection->transformDistanceAcrossScreenToDistanceThruWorld( d, atDist );
}

double Camera::transformFractionalDistAcrossScreenToDistThruWorld(double f, double atDist) const
{
	return projection->transformFractionalDistAcrossScreenToDistThruWorld( f, atDist );
}

double Camera::cameraZToScreenZ(double z) const
{
	return projection->cameraZToScreenZ( z );
}


Point3 Camera::transformWorldToScreen(const Point3 &world) const
{
	return worldToScreenMatrix.transformHomogeneous( world );
}

double Camera::transformWorldToCameraZ(const Point3 &world) const
{
	return -position->getWorldToCameraMatrix().transformZ( world );
}



Point3 Camera::transformScreenToWorldScreenZ(const Point2 &screen, double z) const
{
	return screenToWorldMatrix.transformHomogeneous( screen, z );
}

Point3 Camera::transformScreenToWorldCameraZ(const Point2 &screen, double z) const
{
	double screenZ = cameraZToScreenZ( z );
	return transformScreenToWorldScreenZ( screen, screenZ );
}

Point3 Camera::transformScreenAtTargetDistanceToWorld(const Point2 &screen) const
{
	return transformScreenToWorldCameraZ( screen, position->getCameraToTargetDistance() );
}



const Segment3 & Camera::getTopLeftCornerRay() const
{
	return worldSpaceTopLeftRay;
}

const Segment3 & Camera::getTopRightCornerRay() const
{
	return worldSpaceTopRightRay;
}

const Segment3 & Camera::getBottomLeftCornerRay() const
{
	return worldSpaceBottomLeftRay;
}

const Segment3 & Camera::getBottomRightCornerRay() const
{
	return worldSpaceBottomRightRay;
}


Segment3 Camera::computeRay(const Point2 &screen) const
{
	Point3 near = transformScreenToWorldScreenZ( screen, 0.0 );
	Point3 far = transformScreenToWorldScreenZ( screen, 1.0 );

	return Segment3( near, far );
}



void Camera::oneToOne()
{
	position->setCameraToTargetDistance( projection->getOneToOneDistance() );
}



BackfaceCull * Camera::createBackfaceCull() const
{
	return projection->createBackfaceCull( position );
}



void Camera::refreshCamera()
{
	if ( position == NULL  ||  projection == NULL )
	{
		return;
	}

	worldToScreenMatrix = position->getWorldToCameraMatrix() * projection->getCameraToScreenMatrix();
	screenToWorldMatrix = projection->getScreenToCameraMatrix() * position->getCameraToWorldMatrix();
	worldSpaceFrustum = projection->getFrustum() * position->getCameraToWorldMatrix();

	worldSpaceTopLeftRay = projection->getTopLeftCornerRay() * position->getCameraToWorldMatrix();
	worldSpaceTopRightRay = projection->getTopRightCornerRay() * position->getCameraToWorldMatrix();
	worldSpaceBottomLeftRay = projection->getBottomLeftCornerRay() * position->getCameraToWorldMatrix();
	worldSpaceBottomRightRay = projection->getBottomRightCornerRay() * position->getCameraToWorldMatrix();
}


void Camera::worldToCameraChanged()
{
	refreshCamera();
}

void Camera::cameraToTargetDistanceChanged()
{
	projection->setCameraToTargetDistance( position->getCameraToTargetDistance() );
}

void Camera::projectionChanged()
{
	refreshCamera();
}


#endif
