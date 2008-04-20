//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef FILEIO_H__
#define FILEIO_H__

#include <stdio.h>

#include <string>

#include <Util/Array.h>



GS_DllExport std::string readString(FILE *f);
GS_DllExport void writeString(FILE *f, const std::string &s);

GS_DllExport void readIntArray(FILE *f, Array<int> &a);
GS_DllExport void writeIntArray(FILE *f, const Array<int> &a);



#endif
