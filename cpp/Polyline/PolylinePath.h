//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POLYLINEPATH_H__
#define POLYLINEPATH_H__

#include <Util/Array.h>

#include <Math/Vector3.h>
#include <Math/Matrix4.h>

#include <Polyline/PolylineTypes.h>




enum PolylinePathOrientation
{
	POLYLINEPATHORIENTATION_NOORIENTATION,
	POLYLINEPATHORIENTATION_INXPLANE,
	POLYLINEPATHORIENTATION_INYPLANE,
	POLYLINEPATHORIENTATION_INZPLANE,
	POLYLINEPATHORIENTATION_IN3D,
	POLYLINEPATHORIENTATION_MAX__
};



GS_DllExport void computePathMatrices(const Polyline &path, Array<Matrix4> &matrices, Axis sourceAxis, bool correctiveScaleFlag, PolylinePathOrientation orientation);
GS_DllExport void computePathMatrices(const Polyline &path, Array<Matrix4> &matrices, const Vector3 &sourceAxis, bool correctiveScaleFlag, PolylinePathOrientation orientation);



#endif
