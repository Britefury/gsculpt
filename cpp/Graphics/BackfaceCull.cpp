//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BACKFACECULL_CPP__
#define BACKFACECULL_CPP__

#include <Math/epsilon.h>

#include <Graphics/BackfaceCull.h>




/***********************************************************************************
						BackfaceCull
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( BackfaceCull, RTObject )


BackfaceCull::BackfaceCull()
{
}

BackfaceCull::BackfaceCull(const BackfaceCull &b)
{
}

BackfaceCull::~BackfaceCull()
{
}



bool BackfaceCull::isFrontFacing(const Plane &facePlane) const
{
	return true;
}





/***********************************************************************************
						OrthographicBackfaceCull
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( OrthographicBackfaceCull, BackfaceCull )


OrthographicBackfaceCull::OrthographicBackfaceCull()
{
}

OrthographicBackfaceCull::OrthographicBackfaceCull(const Vector3 &camForwardVector)
{
	forwardVector = camForwardVector;
}

OrthographicBackfaceCull::OrthographicBackfaceCull(const OrthographicBackfaceCull &b)
{
	forwardVector = b.forwardVector;
}

OrthographicBackfaceCull::~OrthographicBackfaceCull()
{
}



bool OrthographicBackfaceCull::isFrontFacing(const Plane &facePlane) const
{
	return facePlane.n.dot( forwardVector )  <  0.0;
}






/***********************************************************************************
						PerspectiveBackfaceCull
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( PerspectiveBackfaceCull, BackfaceCull )


PerspectiveBackfaceCull::PerspectiveBackfaceCull()
{
}

PerspectiveBackfaceCull::PerspectiveBackfaceCull(const Point3 &cameraPosition)
{
	camPosition = cameraPosition;
}

PerspectiveBackfaceCull::PerspectiveBackfaceCull(const PerspectiveBackfaceCull &b)
{
	camPosition = b.camPosition;
}

PerspectiveBackfaceCull::~PerspectiveBackfaceCull()
{
}



bool PerspectiveBackfaceCull::isFrontFacing(const Plane &facePlane) const
{
	return camPosition > facePlane;
}




#endif
