//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef VISUALPLANE_H__
#define VISUALPLANE_H__

#include <Math/Point3.h>
#include <Math/Quaternion.h>
#include <Math/Plane.h>
#include <Math/Colour3f.h>

#include <View/PaintLayer.h>



class GS_DllExport VisualPlane
{
private:
	Point3 position;
	Quaternion rotation;
	double size;

	Plane plane;

	bool bVisible;


public:
	VisualPlane();
	VisualPlane(const VisualPlane &vp);
	VisualPlane(const Point3 &position, const Quaternion &rotation, double size, bool bVisible);


	const Point3 & getPosition() const;
	const Quaternion & getRotation() const;
	double getSize() const;
	const Plane & getPlane() const;
	bool isVisible() const;


	void setPosition(const Point3 &pos);
	void setRotation(const Quaternion &rot);
	void setSize(double sz);
	void setVisible(bool bVisible);

	void draw();
	void paint(const Colour3f &colour, float alpha, PaintLayer paintLayer);



private:
	void refreshPlane();
};



#endif
