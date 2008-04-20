//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPAINTER_CPP__
#define PYPAINTER_CPP__

#include <boost/python.hpp>
using namespace boost::python;



void export_PaintLayer();
void export_ViewSettings();
void export_ViewportSettings();
void export_Viewport3dSettings();
void export_ViewportUVSettings();



BOOST_PYTHON_MODULE(View)
{
	export_PaintLayer();
	export_ViewSettings();
	export_ViewportSettings();
	export_Viewport3dSettings();
	export_ViewportUVSettings();
}



#endif
