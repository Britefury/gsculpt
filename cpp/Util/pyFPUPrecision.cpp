//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYFPUPRECISION_CPP__
#define PYFPUPRECISION_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Util/FPUPrecision.h>



void export_FPUPrecision()
{
	enum_<FPUPrecision>( "FPUPrecision" )
		.value( "SINGLE", FPUPRECISION_SINGLE )
		.value( "DOUBLE", FPUPRECISION_DOUBLE )
		.value( "EXTENDED", FPUPRECISION_EXTENDED );

	def( "setFPUPrecision", setFPUPrecision );
}


#endif
