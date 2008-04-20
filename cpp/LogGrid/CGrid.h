//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CGRID_H__
#define CGRID_H__

#include <Util/Array.h>

#include <Math/Vector3.h>
#include <Math/Point3.h>
#include <Math/Colour3f.h>

#include <LogGrid/Grid.h>


/*
*******************************************************************************
													CGrid
*******************************************************************************
*/

class GS_DllExport CGrid : public Grid
{
private:
	//colours of levels shown
	Array<Colour3f> colours;

public:
	CGrid();
	CGrid(const Ruler &rul, bool useSmallest, double multiplier);
	CGrid(const Ruler &rul, bool useSmallest, double multiplier, const Array<Colour3f> &cols);
	~CGrid();


	//get the color for a specific grid number
	Colour3f & getGridColour(int gridNo);
	const Colour3f & getGridColour(int gridNo) const;


	const Array<Colour3f> & getColours() const;
	void setColours(const Array<Colour3f> &c);
};


#endif
