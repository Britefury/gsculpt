//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef RULER_H__
#define RULER_H__

#include <Util/Array.h>

#include <LogGrid/LogMultiScale.h>



class GS_DllExport Ruler
{
private:
	LogMultiScale sc;

	Array<LogMultiScale::Level> levels;


public:
	Ruler();
	Ruler(const LogMultiScale &s);
	~Ruler();


	void setLength(double l);

	Array<double> generateTicks(double lower, double upper, int level) const;

	int nLevels() const;
	const LogMultiScale & getScale() const;
	const Array<LogMultiScale::Level> & getLevels() const;
};


#endif
