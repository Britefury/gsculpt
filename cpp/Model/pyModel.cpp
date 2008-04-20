//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMODEL_CPP__
#define PYMODEL_CPP__

#include <boost/python.hpp>
using namespace boost::python;



void export_GSProductModelBlank();
void export_GSProductModel();
void export_ModelPainter();
void export_MPick();
void export_MarkPredicate();
void export_VisualPlane();
void export_ModelDraw();



BOOST_PYTHON_MODULE(Model)
{
	export_GSProductModelBlank();
	export_GSProductModel();
	export_ModelPainter();
	export_MPick();
	export_MarkPredicate();
	export_VisualPlane();
	export_ModelDraw();
}



#endif
