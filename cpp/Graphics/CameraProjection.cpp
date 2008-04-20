//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CAMERAPROJECTION_CPP__
#define CAMERAPROJECTION_CPP__

#include <math.h>

#include <PlatformSpecific/IncludeGL.h>

#include <Math/Point4.h>

#include <Graphics/CameraProjection.h>





/******************************************************************************
					CameraProjectionListener
 ******************************************************************************/

CameraProjectionListener::~CameraProjectionListener()
{
}




/******************************************************************************
						CameraProjection
 ******************************************************************************/

CameraProjection::CameraProjection()
{
	cameraToTargetDistance = 200.0;

	fovY = DEGTORAD( 60.0 );
	double yProjection = tan( fovY * 0.5 );
	fovYGradient = 2.0 * yProjection;

	screenW = 640.0;
	screenH = 480.0;
	nearClipFactor = 0.001;
	farClipFactor = 1000.0;

	listener = NULL;
}

CameraProjection::CameraProjection(double nearClipFac, double farClipFac)
{
	cameraToTargetDistance = 200.0;

	fovY = DEGTORAD( 60.0 );
	double yProjection = tan( fovY * 0.5 );
	fovYGradient = 2.0 * yProjection;

	screenW = 640.0;
	screenH = 480.0;
	nearClipFactor = nearClipFac;
	farClipFactor = farClipFac;

	listener = NULL;
}

CameraProjection::~CameraProjection()
{
}



void CameraProjection::setListener(CameraProjectionListener *l)
{
	listener = l;
}



void CameraProjection::setProjectionParameters(double camToTargDist, double screenWidth, double screenHeight, double fieldOfViewY)
{
	cameraToTargetDistance = camToTargDist;

	fovY = fieldOfViewY;
	double yProjection = tan( fieldOfViewY * 0.5 );
	fovYGradient = 2.0 * yProjection;

	screenW = screenWidth;
	screenH = screenHeight;

	projectionModified();
}


CameraProjection::CameraProjectionType CameraProjection::getProjectionType() const
{
	return NONE;
}


void CameraProjection::setScreenDimensions(double screenWidth, double screenHeight)
{
	screenW = screenWidth;
	screenH = screenHeight;

	projectionModified();
}

void CameraProjection::setClipFactors(double nearClipFac, double farClipFac)
{
	nearClipFactor = nearClipFac;
	farClipFactor = farClipFac;
}

void CameraProjection::setFieldOfView(double fieldOfViewY)
{
	fovY = fieldOfViewY;
	double yProjection = tan( fieldOfViewY * 0.5 );
	fovYGradient = 2.0 * yProjection;

	projectionModified();
}

void CameraProjection::setCameraToTargetDistance(double distance)
{
	cameraToTargetDistance = distance;

	projectionModified();
}

double CameraProjection::getScreenWidth() const
{
	return screenW;
}

double CameraProjection::getScreenHeight() const
{
	return screenH;
}

double CameraProjection::getNearClipDistance() const
{
	return cameraToTargetDistance * nearClipFactor;
}

double CameraProjection::getFarClipDistance() const
{
	return cameraToTargetDistance * farClipFactor;
}


double CameraProjection::getOneToOneDistance() const
{
	return screenH / fovYGradient;
}

double CameraProjection::transformDistanceAcrossScreenToDistanceThruWorld(double d) const
{
	return d * cameraToTargetDistance * fovYGradient  /  screenH;
}

double CameraProjection::transformDistanceAcrossScreenToDistanceThruWorld(double d, double atDist) const
{
	return transformFractionalDistAcrossScreenToDistThruWorld( d / screenH, atDist );
}

double CameraProjection::transformFractionalDistAcrossScreenToDistThruWorld( double f, double atDist) const
{
	return 0.0;
}



double CameraProjection::cameraZToScreenZ(double z) const
{
	return z;
}



const Frustum &CameraProjection::getFrustum() const
{
	return cameraSpaceFrustum;
}



const Segment3 & CameraProjection::getTopLeftCornerRay() const
{
	return cameraSpaceTopLeftRay;
}

const Segment3 & CameraProjection::getTopRightCornerRay() const
{
	return cameraSpaceTopRightRay;
}

const Segment3 & CameraProjection::getBottomLeftCornerRay() const
{
	return cameraSpaceBottomLeftRay;
}

const Segment3 & CameraProjection::getBottomRightCornerRay() const
{
	return cameraSpaceBottomRightRay;
}



Vector2 CameraProjection::getViewPlaneSize() const
{
	double aspectRatio = screenW / screenH;

	double ySize = cameraToTargetDistance * fovYGradient;
	return Vector2( ySize * aspectRatio, ySize );
}


void CameraProjection::setGLProjectionMatrix() const
{
	glLoadIdentity();
}


const Matrix4 &CameraProjection::getCameraToScreenMatrix() const
{
	return cameraToScreenMatrix;
}

const Matrix4 &CameraProjection::getScreenToCameraMatrix() const
{
	return screenToCameraMatrix;
}


BackfaceCull * CameraProjection::createBackfaceCull(CameraPosition *position) const
{
	return NULL;
}


void CameraProjection::projectionModified()
{
	updateProjection();

	if ( listener != NULL )
	{
		listener->projectionChanged();
	}
}


void CameraProjection::updateProjection()
{
}





/******************************************************************************
									PerspectiveCameraProjection
 ******************************************************************************/

PerspectiveCameraProjection::PerspectiveCameraProjection() : CameraProjection()
{
}

PerspectiveCameraProjection::PerspectiveCameraProjection(double nearClipFac, double farClipFac)
														: CameraProjection( nearClipFac, farClipFac )
{
}

PerspectiveCameraProjection::~PerspectiveCameraProjection()
{
}



CameraProjection::CameraProjectionType PerspectiveCameraProjection::getProjectionType() const
{
	return PERSPECTIVE;
}


double PerspectiveCameraProjection::cameraZToScreenZ(double z) const
{
	double near = getNearClipDistance();
	double far = getFarClipDistance();

	double a = far  /  ( near - far );
	double b = far * near  /  ( near - far );
	return ( -z * a  +  b ) / z;
}


void PerspectiveCameraProjection::setGLProjectionMatrix() const
{
	double aspectRatio = screenW / screenH;
	glLoadIdentity();
	gluPerspective( RADTODEG( fovY ), aspectRatio, getNearClipDistance(), getFarClipDistance() );
}



double PerspectiveCameraProjection::transformFractionalDistAcrossScreenToDistThruWorld(double f, double atDist) const
{
	return f * atDist * fovYGradient;
}



BackfaceCull * PerspectiveCameraProjection::createBackfaceCull(CameraPosition *position) const
{
	return new PerspectiveBackfaceCull( position->getCameraPosition() );
}


void PerspectiveCameraProjection::updateProjection()
{
	double near = getNearClipDistance();
	double far = getFarClipDistance();
	double aspectRatio = screenW / screenH;

	cameraSpaceFrustum = PerspectiveFrustum( fovY, aspectRatio, near, far );

	double yProj = fovYGradient * 0.5;
	double xProj = yProj * aspectRatio;

	cameraSpaceTopLeftRay = Segment3( Point3( -xProj * near, yProj * near, -near ), Point3( -xProj * far, yProj * far, -far ) );
	cameraSpaceTopRightRay = Segment3( Point3( xProj * near, yProj * near, -near ), Point3( xProj * far, yProj * far, -far ) );
	cameraSpaceBottomLeftRay = Segment3( Point3( -xProj * near, -yProj * near, -near ), Point3( -xProj * far, -yProj * far, -far ) );
	cameraSpaceBottomRightRay = Segment3( Point3( xProj * near, -yProj * near, -near ), Point3( xProj * far, -yProj * far, -far ) );

	cameraToScreenMatrix = Matrix4::perspective( near, far )  *  Matrix4::camera( fovY, aspectRatio )  *  Matrix4::screen( screenW, screenH );
	screenToCameraMatrix = cameraToScreenMatrix.inverse();
}





/******************************************************************************
									OrthographicCameraProjection
 ******************************************************************************/

OrthographicCameraProjection::OrthographicCameraProjection() : CameraProjection()
{
	nearClipFactor = -1000.0;
	farClipFactor = 1000.0;
}

OrthographicCameraProjection::OrthographicCameraProjection(double nearClipFac, double farClipFac)
														: CameraProjection( nearClipFac, farClipFac )
{
}

OrthographicCameraProjection::~OrthographicCameraProjection()
{
}



CameraProjection::CameraProjectionType OrthographicCameraProjection::getProjectionType() const
{
	return ORTHOGRAPHIC;
}


double OrthographicCameraProjection::cameraZToScreenZ(double z) const
{
	double near = getNearClipDistance();
	double far = getFarClipDistance();

	double a = 1.0  /  ( far - near );
	double b = -near * a;
	return z * a  +  b;
}


void OrthographicCameraProjection::setGLProjectionMatrix() const
{
	double aspectRatio = screenW / screenH;

	double volHeight = cameraToTargetDistance * fovYGradient;
	double volWidth = volHeight * aspectRatio;

	glLoadIdentity();
	glOrtho( -volWidth * 0.5, volWidth * 0.5, -volHeight * 0.5, volHeight * 0.5, getNearClipDistance(), getFarClipDistance() );
}



double OrthographicCameraProjection::transformFractionalDistAcrossScreenToDistThruWorld(double f, double atDist) const
{
	return f * cameraToTargetDistance * fovYGradient;
}



BackfaceCull * OrthographicCameraProjection::createBackfaceCull(CameraPosition *position) const
{
	return new OrthographicBackfaceCull( position->getForwardAxis() );
}



void OrthographicCameraProjection::updateProjection()
{
	double near = getNearClipDistance();
	double far = getFarClipDistance();
	double aspectRatio = screenW / screenH;

	double volHeight = cameraToTargetDistance * fovYGradient;
	double volWidth = volHeight * aspectRatio;

	double xProj = volWidth * 0.5;
	double yProj = volHeight * 0.5;

	cameraSpaceFrustum = OrthographicFrustum( volHeight, aspectRatio, near, far );

	cameraSpaceTopLeftRay = Segment3( Point3( -xProj, yProj, -near ), Point3( -xProj, yProj, -far ) );
	cameraSpaceTopRightRay = Segment3( Point3( xProj, yProj, -near ), Point3( xProj, yProj, -far ) );
	cameraSpaceBottomLeftRay = Segment3( Point3( -xProj, -yProj, -near ), Point3( -xProj, -yProj, -far ) );
	cameraSpaceBottomRightRay = Segment3( Point3( xProj, -yProj, -near ), Point3( xProj, -yProj, -far ) );

	cameraToScreenMatrix = Matrix4::orthographic( getNearClipDistance(), getFarClipDistance() ) *
							Matrix4::scale( Vector3( 1.0 / xProj, 1.0 / yProj, 1.0 ) ) *
							Matrix4::screen( screenW, screenH );
	screenToCameraMatrix = cameraToScreenMatrix.inverse();
}


#endif
