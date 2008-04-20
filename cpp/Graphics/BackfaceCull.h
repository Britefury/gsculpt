//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BACKFACECULL_H__
#define BACKFACECULL_H__

#include <Math/Vector3.h>
#include <Math/Point3.h>
#include <Math/Plane.h>

#include <RTType/RTType.h>




/***********************************************************************************
						BackfaceCull
 ***********************************************************************************/

class GS_DllExport BackfaceCull : public RTObject
{
RT_TYPE;
public:
	BackfaceCull();
	BackfaceCull(const BackfaceCull &b);
	virtual ~BackfaceCull();


	virtual bool isFrontFacing(const Plane &facePlane) const;
};





/***********************************************************************************
						OrthographicBackfaceCull
 ***********************************************************************************/

class GS_DllExport OrthographicBackfaceCull : public BackfaceCull
{
RT_TYPE;
public:
	Vector3 forwardVector;


	OrthographicBackfaceCull();
	OrthographicBackfaceCull(const Vector3 &camForwardVector);
	OrthographicBackfaceCull(const OrthographicBackfaceCull &b);
	virtual ~OrthographicBackfaceCull();


	virtual bool isFrontFacing(const Plane &facePlane) const;
};




/***********************************************************************************
						PerspectiveBackfaceCull
 ***********************************************************************************/

class GS_DllExport PerspectiveBackfaceCull : public BackfaceCull
{
RT_TYPE;
public:
	Point3 camPosition;


	PerspectiveBackfaceCull();
	PerspectiveBackfaceCull(const Point3 &cameraPosition);
	PerspectiveBackfaceCull(const PerspectiveBackfaceCull &b);
	virtual ~PerspectiveBackfaceCull();


	virtual bool isFrontFacing(const Plane &facePlane) const;
};



#endif

