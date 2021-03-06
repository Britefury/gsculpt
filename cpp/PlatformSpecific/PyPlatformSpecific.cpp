//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPLATFORMSPECIFIC_CPP__
#define PYPLATFORMSPECIFIC_CPP__

#include <boost/python.hpp>

using namespace boost::python;

#include <PlatformSpecific/GLExtensions.h>



static bool initialisePlatformSpecific()
{
	return true;
}



static void exportPlatformSpecific()
{
	def( "initialisePlatformSpecific", initialisePlatformSpecific );
}


BOOST_PYTHON_MODULE(PlatformSpecific)
{
	exportPlatformSpecific();	
}


#endif
