//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPOLYBLEND_CPP__
#define PYPOLYBLEND_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <PolyBlend/PolyBlend.h>


void export_PolyBlend()
{
	class_<PolyBlend>( "PolyBlend", init<double, double, double, double>() );
}


#endif
