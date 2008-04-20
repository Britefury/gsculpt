//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMARKPREDICATE_CPP__
#define PYMARKPREDICATE_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Model/MarkPredicate.h>



void export_MarkPredicate()
{
	enum_<MarkPredicate>( "MarkPredicate" )
		.value( "ADD", MARKPREDICATE_ADD )
		.value( "MARK", MARKPREDICATE_MARK )
		.value( "FILTER", MARKPREDICATE_FILTER )
		.value( "INVERT", MARKPREDICATE_INVERT )
		.value( "UNMARK", MARKPREDICATE_UNMARK );
}


#endif
