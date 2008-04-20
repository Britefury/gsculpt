//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYBRUSHMODULE_CPP__
#define PYBRUSHMODULE_CPP__

#include <boost/python.hpp>
using namespace boost::python;



void export_Brush();
void export_SphericalBoundaryBrush();
void export_CubicSCurveBrush();
void export_FlatBrush();
void export_GaussianBrush();
void export_LinearBrush();
void export_SphericalBrush();



BOOST_PYTHON_MODULE(Brush)
{
	export_Brush();
	export_SphericalBoundaryBrush();
	export_CubicSCurveBrush();
	export_FlatBrush();
	export_GaussianBrush();
	export_LinearBrush();
	export_SphericalBrush();
}



#endif
