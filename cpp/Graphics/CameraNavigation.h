//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CAMERANAVIGATION_H__
#define CAMERANAVIGATION_H__

#include <Graphics/CameraPosition.h>




/******************************************************************************
					CameraNavigation
 ******************************************************************************/

class GS_DllExport CameraNavigation : public CameraRotator
{
public:
	CameraNavigation();
	virtual ~CameraNavigation();

	virtual void rotate(CameraPosition *cam, double xrot, double yrot);
	virtual void pan(CameraPosition *cam, const Vector3 &v);
	virtual void moveInZ(CameraPosition *cam, double screenZ, double worldZ);
};



/******************************************************************************
					OrbitalCameraNavigation
 ******************************************************************************/

class GS_DllExport OrbitalCameraNavigation : public CameraNavigation
{
private:
	double zoompp;

public:
	OrbitalCameraNavigation();
	OrbitalCameraNavigation(double zoomPerPixel);

	virtual void updatePositions(const Vector3 &forwardAxis, double targetDistance,
										  Point3 &cameraPosition,
										  Point3 &targetPosition) const;

	virtual void rotate(CameraPosition *cam, double xrot, double yrot);
	virtual void pan(CameraPosition *cam, const Vector3 &v);
	virtual void moveInZ(CameraPosition *cam, double screenZ, double worldZ);
};



/******************************************************************************
					FirstPersonCameraNavigation
 ******************************************************************************/

class GS_DllExport FirstPersonCameraNavigation : public CameraNavigation
{
public:
	FirstPersonCameraNavigation();

	virtual void updatePositions(const Vector3 &forwardAxis, double targetDistance,
										  Point3 &cameraPosition,
										  Point3 &targetPosition) const;

	virtual void rotate(CameraPosition *cam, double xrot, double yrot);
	virtual void pan(CameraPosition *cam, const Vector3 &v);
	virtual void moveInZ(CameraPosition *cam, double screenZ, double worldZ);
};


#endif
