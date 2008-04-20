//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYGRAPHICS_CPP
#define PYGRAPHICS_CPP

#include <boost/python.hpp>
using namespace boost::python;



void export_BackfaceCull();
void export_CameraProjection();
void export_CameraPosition();
void export_CameraNavigation();
void export_Camera();
void export_Drawing();
void export_GLMatrix();
void export_Region();



BOOST_PYTHON_MODULE(Graphics)
{
	export_BackfaceCull();
	export_CameraProjection();
	export_CameraPosition();
	export_CameraNavigation();
	export_Camera();
	export_Drawing();
	export_GLMatrix();
	export_Region();
}



#endif
