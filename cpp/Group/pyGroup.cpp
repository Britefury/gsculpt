//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYGROUP_CPP__
#define PYGROUP_CPP__

#include <boost/python.hpp>
using namespace boost::python;



void export_GSProductGroup();
void export_GroupPainter();



BOOST_PYTHON_MODULE(Group)
{
	export_GSProductGroup();
	export_GroupPainter();
}



#endif
