//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GLMATRIX_H__
#define GLMATRIX_H__

#include <PlatformSpecific/IncludeGL.h>

#include <Math/Point3.h>
#include <Math/Vector3.h>
#include <Math/Matrix4.h>



GS_DllExport GLenum glCurrentMatrixStack();
GS_DllExport void glGetMatrixd(double *matrix);
GS_DllExport void glGetMatrix(Matrix4 &m);
GS_DllExport void glMultMatrix(const Matrix4 &m);


#endif
