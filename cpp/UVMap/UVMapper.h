//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef UVMAPPER_H__
#define UVMAPPER_H__

#include <Util/Array.h>

#include <Math/Point3.h>
#include <Math/Vector3f.h>
#include <Math/Point2f.h>
#include <Math/Plane.h>



/********************************************************************************
						UVMapper
 ********************************************************************************/

class GS_DllExport UVMapper
{
private:
	bool invertU, invertV;

public:
	UVMapper(bool invertU = false, bool invertV = false);
	virtual ~UVMapper() = 0;


	void invert(Point2f &texCoord);

	virtual void uvMap(const Array<Point3> &facePoints, const Array<Vector3f> &normals, const Plane &facePlane, Array<Point2f> &texCoords, int &faceMaterial) = 0;
};


#endif
