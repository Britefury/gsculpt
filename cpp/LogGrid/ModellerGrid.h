//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MODELLERGRID_H__
#define MODELLERGRID_H__

#include <boost/python.hpp>


#include <Util/Array.h>

#include <Math/BBox3.h>
#include <Math/Segment3.h>
#include <Math/Colour3f.h>

#include <Graphics/Camera.h>

#include <LogGrid/CGrid.h>




class GS_DllExport ModellerGrid
{
public:
	enum GridPlane
	{
		GRIDPLANE_X,
		GRIDPLANE_Y,
		GRIDPLANE_Z,
		GRIDPLANE_AUTO
	};


private:
	CGrid g;
	bool gridRequiresRefreshFlag, enabledFlag;
	Vector3 gridOffset;

	GridPlane gridPlane;


	struct LevelLineList
	{
		Grid::LineList hlines, vlines;
		Colour3f col;
	};

	Array<LevelLineList> lineList;


public:
	ModellerGrid();
	~ModellerGrid();


	void setGrid(const CGrid &gr);


	void setGridPlane(GridPlane plane);
	GridPlane getGridPlane() const;


	void setEnabled(bool enabled);
	bool isEnabled() const;


	const Array<Colour3f> & getColours() const;
	void setColours(const Array<Colour3f> &c);


	boost::python::list py_getColours() const;
	void py_setColours(boost::python::list c);


	void paint3d(Camera *camera);
	void worldToScreenChangedNotify();


private:
	Axis getBestAxis(Vector3 forwardVector);
	Axis computeGridPlaneNormalAxis(Camera *cam);
	BBox3 computeVisibleArea(Camera *cam, const Plane &gridPlane);

	void paintLabel(const Grid::GridLine &l, Camera *cam);
	void computeGrid(Camera *cam);
};



#endif
