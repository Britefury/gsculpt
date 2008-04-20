//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CAMERAPOSITION_H__
#define CAMERAPOSITION_H__

#include <Math/Point3.h>
#include <Math/Vector3.h>
#include <Math/Matrix4.h>



/******************************************************************************
						CameraRotator
 ******************************************************************************/

class GS_DllExport CameraRotator
{
public:
	virtual ~CameraRotator();

	virtual void updatePositions(const Vector3 &forwardAxis, double targetDistance, Point3 &cameraPosition, Point3 &targetPosition) const;
};



/******************************************************************************
					CameraPositionListener
 ******************************************************************************/

class GS_DllExport CameraPositionListener
{
public:
	virtual ~CameraPositionListener();

	virtual void worldToCameraChanged() = 0;
	virtual void cameraToTargetDistanceChanged() = 0;
};



/******************************************************************************
						CameraPosition
 ******************************************************************************/


class GS_DllExport CameraPosition
{
private:
	Point3 targetPosition, cameraPosition;
	Vector3 forwardAxis, upAxis, rightAxis;
	double cameraToTargetDistance;
	Matrix4 cameraToWorldMatrix, worldToCameraMatrix;

	bool axisAlignedFlag;

	CameraPositionListener *listener;

public:
	CameraPosition();



	void setListener(CameraPositionListener *l);


	bool isAxisAligned() const;


	double transformWorldToCameraZ(const Point3 &world) const;

	const Vector3 &getForwardAxis() const;
	const Vector3 &getUpAxis() const;
	const Vector3 &getRightAxis() const;



	void setGLModelViewMatrix() const;

	const Matrix4 &getWorldToCameraMatrix() const;
	const Matrix4 &getCameraToWorldMatrix() const;




	const Point3 &getCameraPosition() const;
	const Point3 &getTargetPosition() const;


	void front();
	void back();
	void top();
	void bottom();
	void left();
	void right();

	void rotateAroundRightAxis(double angle, const CameraRotator *r);
	void rotateAroundUpAxis(double angle, const CameraRotator *r);

	void translate(const Vector3 &translation);


	void zoom(double z);

	void origin();
	void viewZ0();


	void lookat(const Point3 &pos, const Point3 &target);



	double getCameraToTargetDistance() const;
	void setCameraToTargetDistance(double d);

private:
	void rotate(const Matrix4 &rotation, const CameraRotator *r);

	void cameraToWorldModified();
	void cameraToTargetDistanceModified();
};

#endif
