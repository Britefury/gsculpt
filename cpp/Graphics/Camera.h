//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CAMERA_H__
#define CAMERA_H__

#include <Math/Point3.h>
#include <Math/Vector3.h>
#include <Math/Vector2.h>
#include <Math/Matrix4.h>
#include <Math/Segment3.h>
#include <Math/Frustum.h>
#include <Math/Plane.h>
#include <Math/Projection.h>

#include <Graphics/CameraPosition.h>
#include <Graphics/CameraProjection.h>
#include <Graphics/BackfaceCull.h>



/******************************************************************************
												Camera
 ******************************************************************************/

class GS_DllExport Camera : public CameraPositionListener, public CameraProjectionListener
{
private:
	CameraPosition *position;
	CameraProjection *projection;

	Frustum worldSpaceFrustum;

	Segment3 worldSpaceTopLeftRay, worldSpaceTopRightRay, worldSpaceBottomLeftRay, worldSpaceBottomRightRay;

	Matrix4 worldToScreenMatrix, screenToWorldMatrix;


	double screenW, screenH, fovY;

public:
	Camera(double screenWidth, double screenHeight, double fieldOfViewY);
	virtual ~Camera();



	void setScreenDimensions(double screenWidth, double screenHeight);
	void setFieldOfView(double fieldOfViewY);



	CameraPosition * getCameraPosition();
	void setCameraPosition(CameraPosition *pos);

	CameraProjection * getCameraProjection();
	void setCameraProjection(CameraProjection *proj);



	void setGLModelViewMatrix() const;
	void setGLProjectionMatrix() const;

	void glProjectionMatrixIdentity() const;
	void glModelViewMatrixIdentity() const;


	double getCameraToTargetDistance() const;



	Plane getNearClipPlane() const;
	Plane getFarClipPlane() const;

	double getNearClipDistance() const;
	double getFarClipDistance() const;



	const Frustum &getFrustum() const;



	const Matrix4 &getWorldToScreenMatrix() const;
	const Matrix4 &getScreenToWorldMatrix() const;


	Projection getProjection() const;



	double transformDistanceAcrossScreenToDistanceThruWorld(double d) const;
	double transformDistanceAcrossScreenToDistanceThruWorld(double d, double atDist) const;
	double transformFractionalDistAcrossScreenToDistThruWorld(double f, double atDist) const;
	double cameraZToScreenZ(double z) const;

	double py_transformDistanceAcrossScreenToDistanceThruWorld(double d) const
	{
		return transformDistanceAcrossScreenToDistanceThruWorld( d );
	}

	double py_transformDistanceAcrossScreenToDistanceThruWorld2(double d, double atDist) const
	{
		return transformDistanceAcrossScreenToDistanceThruWorld( d, atDist );
	}



	Point3 transformWorldToScreen(const Point3 &world) const;
	double transformWorldToCameraZ(const Point3 &world) const;

	Point3 transformScreenToWorldScreenZ(const Point2 &screen, double z) const;
	Point3 transformScreenToWorldCameraZ(const Point2 &screen, double z) const;
	Point3 transformScreenAtTargetDistanceToWorld(const Point2 &screen) const;

	const Segment3 & getTopLeftCornerRay() const;
	const Segment3 & getTopRightCornerRay() const;
	const Segment3 & getBottomLeftCornerRay() const;
	const Segment3 & getBottomRightCornerRay() const;

	Segment3 computeRay(const Point2 &screen) const;



	void oneToOne();




	virtual BackfaceCull * createBackfaceCull() const;


protected:
	void refreshCamera();

	virtual void worldToCameraChanged();
	virtual void cameraToTargetDistanceChanged();
	virtual void projectionChanged();
};


#endif
