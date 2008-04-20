//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef UNIQUEID_H__
#define UNIQUEID_H__

#include <map>
#include <string>


#include <Util/Array.h>


typedef int UniqueID;


typedef Array<UniqueID> UniqueIDList;




inline void offsetUniqueIDList(UniqueIDList &idList, UniqueID offset)
{
	for (int i = 0; i < idList.size(); i++)
	{
		idList[i] += offset;
	}
}




class UIDTranslator : public std::map<UniqueID,UniqueID>
{
public:
	inline UIDTranslator() : std::map<UniqueID,UniqueID>()
	{
	}

	inline UniqueID translate(UniqueID id) const
	{
		const_iterator iter = find( id );

		if ( iter != end() )
		{
			return iter->second;
		}
		else
		{
			return -1;
		}
	}
};




class TableOfUIDTranslators : public std::map<std::string, UIDTranslator>
{
public:
	inline TableOfUIDTranslators() : std::map<std::string, UIDTranslator>()
	{
	}

	inline const UIDTranslator * getTranslator(const std::string &name) const
	{
		const_iterator iter = find( name );

		if ( iter != end() )
		{
			return &( iter->second );
		}
		else
		{
			return NULL;
		}
	}

	inline UIDTranslator * getTranslator(const std::string &name)
	{
		iterator iter = find( name );

		if ( iter != end() )
		{
			return &( iter->second );
		}
		else
		{
			return NULL;
		}
	}
};


#endif
