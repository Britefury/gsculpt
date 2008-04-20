//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BRUSH_H__
#define BRUSH_H__

#include <Util/Array.h>
#include <Util/BitList.h>

#include <Math/BBox3.h>
#include <Math/Point3.h>

#include <RTType/RTType.h>



class GS_DllExport Brush : public RTObject
{
RT_ABSTRACT_TYPE;
public:
	Brush();
	Brush(const Brush &b);
	virtual ~Brush();


	virtual bool intersectsBBox(const Point3 &brushPosition, const BBox3 &box) const = 0;
	virtual double computePointWeight(const Point3 &brushPosition, const Point3 &point) const = 0;
};



#endif // BRUSH_H__
