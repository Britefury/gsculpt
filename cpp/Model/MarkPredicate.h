//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MARKPREDICATE_H__
#define MARKPREDICATE_H__

enum MarkStatus
{
	MARKSTATUS_NONE,
	MARKSTATUS_SOME,
	MARKSTATUS_ALL
};




enum MarkPredicate
{
	MARKPREDICATE_ADD,
	MARKPREDICATE_MARK,
	MARKPREDICATE_FILTER,
	MARKPREDICATE_INVERT,
	MARKPREDICATE_UNMARK,
	MARKPREDICATE__MAX__
};



GS_DllExport const char * getNameOfMarkPredicate(MarkPredicate predicate);


GS_DllExport inline int getNumberOfMarkPredicates()
{
	return MARKPREDICATE__MAX__;
}

GS_DllExport inline bool markPredicateIsMarked(MarkPredicate predicate, bool alreadyMarked, bool isInRegion)
{
	switch(predicate)
	{
	case MARKPREDICATE_ADD:
		return alreadyMarked || isInRegion;
	case MARKPREDICATE_MARK:
		return isInRegion;
	case MARKPREDICATE_FILTER:
		return alreadyMarked && isInRegion;
	case MARKPREDICATE_INVERT:
		return (!alreadyMarked && isInRegion) || (alreadyMarked && !isInRegion);
	case MARKPREDICATE_UNMARK:
		return alreadyMarked && !isInRegion;
	default:
		return alreadyMarked || isInRegion;
	}
}



#endif
