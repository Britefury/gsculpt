//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef FPUPRECISION_H__
#define FPUPRECISION_H__


enum FPUPrecision
{
	FPUPRECISION_SINGLE,
	FPUPRECISION_DOUBLE,
	FPUPRECISION_EXTENDED
};



GS_DllExport void setFPUPrecision(FPUPrecision precision);



#endif // FPUPRECISION_H__
