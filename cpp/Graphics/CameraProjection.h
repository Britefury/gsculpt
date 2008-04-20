//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CAMERAPROJECTION_H__
#define CAMERAPROJECTION_H__

#include <Math/Segment3.h>
#include <Math/Frustum.h>
#include <Math/Matrix4.h>

#include <Graphics/BackfaceCull.h>
#include <Graphics/CameraPosition.h>




/******************************************************************************
					CameraProjectionListener
 ******************************************************************************/

class GS_DllExport CameraProjectionListener
{
public:
	virtual ~CameraProjectionListener() = 0;

	virtual void projectionChanged() = 0;
};



/******************************************************************************
						CameraProjection
 ******************************************************************************/

class GS_DllExport CameraProjection
{
public:
	enum CameraProjectionType
	{
		NONE,
		PERSPECTIVE,
		ORTHOGRAPHIC
	};

protected:
	double cameraToTargetDistance;

	double fovY, fovYGradient;
	double screenW, screenH, nearClipFactor, farClipFactor;

	Frustum cameraSpaceFrustum;

	Matrix4 cameraToScreenMatrix, screenToCameraMatrix;

	Segment3 cameraSpaceTopLeftRay, cameraSpaceTopRightRay, cameraSpaceBottomLeftRay, cameraSpaceBottomRightRay;


	CameraProjectionListener *listener;


public:
	CameraProjection();
	CameraProjection(double nearClipFac, double farClipFac);
	virtual ~CameraProjection();



	void setListener(CameraProjectionListener *l);



	void setProjectionParameters(double camToTargDist, double screenWidth, double screenHeight, double fieldOfViewY);



	virtual CameraProjectionType getProjectionType() const;


	void setScreenDimensions(double screenWidth, double screenHeight);
	void setClipFactors(double nearClipFac, double farClipFac);
	void setFieldOfView(double fieldOfViewY);
	virtual void setCameraToTargetDistance(double distance);
	double getScreenWidth() const;
	double getScreenHeight() const;
	double getNearClipDistance() const;
	double getFarClipDistance() const;

	double getOneToOneDistance() const;
	double transformDistanceAcrossScreenToDistanceThruWorld(double d) const;
	double transformDistanceAcrossScreenToDistanceThruWorld(double d, double atDist) const;

	inline double py_transformDistanceAcrossScreenToDistanceThruWorld(double d) const
	{
		return transformDistanceAcrossScreenToDistanceThruWorld( d );
	}

	inline double py_transformDistanceAcrossScreenToDistanceThruWorldAtDist(double d, double atDist) const
	{
		return transformDistanceAcrossScreenToDistanceThruWorld( d, atDist );
	}

	virtual double transformFractionalDistAcrossScreenToDistThruWorld( double f, double atDist) const;

	virtual double cameraZToScreenZ(double z) const;

	const Frustum & getFrustum() const;



	const Segment3 & getTopLeftCornerRay() const;
	const Segment3 & getTopRightCornerRay() const;
	const Segment3 & getBottomLeftCornerRay() const;
	const Segment3 & getBottomRightCornerRay() const;


	Vector2 getViewPlaneSize() const;

	virtual void setGLProjectionMatrix() const;

	const Matrix4 &getCameraToScreenMatrix() const;
	const Matrix4 &getScreenToCameraMatrix() const;


	virtual BackfaceCull * createBackfaceCull(CameraPosition *position) const;


private:
	void projectionModified();

protected:
	virtual void updateProjection();
};




/******************************************************************************
					PerspectiveCameraProjection
 ******************************************************************************/

class GS_DllExport PerspectiveCameraProjection : public CameraProjection
{
public:
	PerspectiveCameraProjection();
	PerspectiveCameraProjection(double nearClipFac, double farClipFac);
	virtual ~PerspectiveCameraProjection();


	virtual CameraProjectionType getProjectionType() const;


	virtual double cameraZToScreenZ(double z) const;


	virtual void setGLProjectionMatrix() const;


	virtual double transformFractionalDistAcrossScreenToDistThruWorld(double f, double atDist) const;


 	virtual BackfaceCull * createBackfaceCull(CameraPosition *position) const;

protected:
	virtual void updateProjection();
};




/******************************************************************************
					OrthographicCameraProjection
 ******************************************************************************/

class GS_DllExport OrthographicCameraProjection : public CameraProjection
{
public:
	OrthographicCameraProjection();
	OrthographicCameraProjection(double nearClipFac, double farClipFac);
	virtual ~OrthographicCameraProjection();


	virtual CameraProjectionType getProjectionType() const;


	virtual double cameraZToScreenZ(double z) const;


	virtual void setGLProjectionMatrix() const;


	virtual double transformFractionalDistAcrossScreenToDistThruWorld(double f, double atDist) const;


 	virtual BackfaceCull * createBackfaceCull(CameraPosition *position) const;

protected:
	virtual void updateProjection();
};



#endif
