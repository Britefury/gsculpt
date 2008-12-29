//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GLEXTENSIONS_H__
#define GLEXTENSIONS_H__

#include <PlatformSpecific/IncludeGL.h>

#include <GL/gl.h>
#include <GL/glu.h>


class GLExtensions
{
public:
	PFNGLGENBUFFERSPROC glGenBuffers;
	PFNGLBINDBUFFERPROC glBindBuffer;
	PFNGLBUFFERDATAPROC glBufferData;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers;


	GLExtensions();
};



GS_DllExport GLExtensions * getGLExtensions();


#endif
