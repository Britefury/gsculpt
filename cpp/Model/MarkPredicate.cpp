//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MARKPREDICATE_CPP__
#define MARKPREDICATE_CPP__

#include <Model/MarkPredicate.h>



const char * getNameOfMarkPredicate(MarkPredicate predicate)
{
	static const char* predicateNameStrings[] = { "Or", "Mark", "And", "Invert", "Unmark" };

	return predicateNameStrings[predicate];
}



#endif
