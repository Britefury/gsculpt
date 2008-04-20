//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BLENDINGPOLYLINE_H__
#define BLENDINGPOLYLINE_H__

#include <Math/Point3.h>
#include <Math/Matrix4.h>
#include <Math/BBox3.h>



class GS_DllExport BlendingPolyline : public Array<Point3>
{
private:
	bool closedFlag;


public:
	BlendingPolyline();


	void operator*=(const Matrix4 &m);
	const Point3 &atWrapped(int i) const;
	Point3 &atWrapped(int i);


	void close();
	void open();
	void setClosed(bool closed);
	bool isClosed() const;


	void scaleToUnit();
	static void scaleToUnit(BlendingPolyline &p1, BlendingPolyline &p2);
};


#endif
