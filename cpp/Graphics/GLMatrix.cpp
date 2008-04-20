//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GLMATRIX_CPP__
#define GLMATRIX_CPP__

#include <PlatformSpecific/IncludeGL.h>

#include <Graphics/GLMatrix.h>



GLenum glCurrentMatrixStack()
{
	GLint mode;
	glGetIntegerv( GL_MATRIX_MODE, &mode );
	return mode == GL_MODELVIEW ? GL_MODELVIEW_MATRIX :
			 mode == GL_PROJECTION ? GL_PROJECTION_MATRIX :
			 GL_TEXTURE_MATRIX;
}

void glGetMatrixd(double *matrix)
{
	glGetDoublev( glCurrentMatrixStack(), matrix );
}

void glGetMatrix(Matrix4 &m)
{
	glGetDoublev( glCurrentMatrixStack(), m.d );
}

void glMultMatrix(const Matrix4 &m)
{
	glMultMatrixd( m.d );
}



#endif
