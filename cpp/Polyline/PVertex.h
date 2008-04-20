//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PVERTEX_H__
#define PVERTEX_H__

#include <stdio.h>

#include <Math/Point3.h>
#include <Math/Matrix4.h>

#include <Transformation/Transformation.h>

#include <Model/MarkPredicate.h>

#include <Graphics/Region.h>

#include <Polyline/PolylineTypes.h>




class GS_DllExport PVertex
{
private:
	Point3 position, positionSave;

	struct
	{
		unsigned mark : 1;
		unsigned markSave : 1;
		unsigned normalSharp : 1;
		unsigned textureBreak : 1;
	} flags;


public:
	PVertex();
	PVertex(const Point3 &pos, bool marked);


	void read(FILE *f);
	void write(FILE *f) const;



	//mark, unmark, check marked status
	void mark();
	void unmark();
	void setMark(bool marked);
	bool isMarked() const;

	//normal sharpness
	void setNormalSharp(bool sharp);
	bool isNormalSharp() const;

	//texture break
	void setTextureBreak(bool texBreak);
	bool isTextureBreak() const;


	void copyPropertiesFrom(const PVertex &src);


	//set world, get world
	void setPosition(const Point3 &pos);
	const Point3 &getPosition() const;


	void copyFrom(const PVertex &v);



	//transform vertex (transform world coord)
	void transform(const Matrix4 &m);
	void transform(const Transformation *x);

	//mark vertex
	int mark(MarkPredicate predicate, const Region3d *region);
	int mark(MarkPredicate predicate);
	bool isInside(const Region3d *region) const;


	void savePosition();
	void restorePosition();

	void saveMark();
	int restoreMark();
};



#endif
