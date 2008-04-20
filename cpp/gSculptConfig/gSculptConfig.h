//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GSCULPTCONFIG_H__
#define GSCULPTCONFIG_H__


bool gSculptConfigLittleEndian()
{
#ifdef GSCULPT_CONFIG_BIG_ENDIAN
	return false;
#else
	return true;
#endif
}



#endif
