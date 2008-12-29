//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GLEXTENSIONS_CPP__
#define GLEXTENSIONS_CPP__

#include <stdio.h>

#include <PlatformSpecific/IncludeGL.h>

#include <PlatformSpecific/GLExtensions.h>

#include <GL/gl.h>
#include <GL/glu.h>



#if defined(GSCULPT_PLATFORM_WIN32)
	#include <wingdi.h>
#elif defined(GSCULPT_PLATFORM_POSIX)
	#include <GL/glx.h>
#endif


typedef void (*GLFnPtr)();


static GLFnPtr glExtGetProcAddress(const char *name)
{
	#if defined(GSCULPT_PLATFORM_WIN32)
		return (GLFnPtr)wglGetProcAddress( (LPCSTR)name );
	#elif defined(GSCULPT_PLATFORM_POSIX)
		return (GLFnPtr)glXGetProcAddress( (const GLubyte*)name );
	#endif
}



GLExtensions::GLExtensions()
{
	glGenBuffers = NULL;
	glBindBuffer = NULL;
	glBufferData = NULL;
	glDeleteBuffers = NULL;
}



static GLExtensions extensions;



GS_DllExport GLExtensions * getGLExtensions()
{
	static bool bInitialised = false;

	if ( !bInitialised )
	{
		bInitialised = true;

		extensions.glGenBuffers = (PFNGLGENBUFFERSPROC)glExtGetProcAddress( "glGenBuffers" );
		extensions.glBindBuffer = (PFNGLBINDBUFFERPROC)glExtGetProcAddress( "glBindBuffer" );
		extensions.glBufferData = (PFNGLBUFFERDATAPROC)glExtGetProcAddress( "glBufferData" );
		extensions.glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glExtGetProcAddress( "glDeleteBuffers" );
	}

	if ( extensions.glGenBuffers != NULL  &&  extensions.glBindBuffer != NULL  &&  extensions.glBufferData != NULL  &&  extensions.glDeleteBuffers != NULL )
	{
		return &extensions;
	}
	else
	{
		return NULL;
	}
}


#endif
