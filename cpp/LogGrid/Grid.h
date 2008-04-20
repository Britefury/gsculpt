//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GRID_H__
#define GRID_H__

#include <Util/Array.h>

#include <Math/Point3.h>
#include <Math/Segment3.h>
#include <LogGrid/Ruler.h>




class GS_DllExport Grid
{
public:
	struct GridLine
	{
		Segment3 line;
		double labelValue;
	};

	typedef Array<GridLine> LineList;

private:
	Ruler r;
	bool useSmallestFlag;
	Point3 p1, p2;
	double mul;

public:
	Grid();
	Grid(const Ruler &rul, bool useSmallest, double multiplier);
	~Grid();


	void setCorners(const Point3 &p, const Point3 &q);

	void generateHLines(int level, LineList &lines) const;
	void generateVLines(int level, LineList &lines) const;

	int nLevels() const;
	const Ruler & getRuler() const;
};



#endif
