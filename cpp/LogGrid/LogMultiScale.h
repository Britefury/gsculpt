//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LOGMULTISCALE_H__
#define LOGMULTISCALE_H__

#include <Util/Array.h>

#include <LogGrid/LogScale.h>



class GS_DllExport LogMultiScale : public LogScale
{
private:
	int nLevs;

public:
	LogMultiScale();
	LogMultiScale(double bs, const Array<double> pwr, int nLevels);


	Array<LogScale::Level> generateLevels(double sz) const;

	int nLevels() const;
};


#endif
