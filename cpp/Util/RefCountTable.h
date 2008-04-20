//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef REFCOUNTTABLE_H__
#define REFCOUNTTABLE_H__

#include <map>

#include <Util/Array.h>



template <class Key> class RefCountTable
{
private:
	typedef std::map<Key, int> RefTable;

public:
	typedef typename RefTable::const_iterator const_iterator;

private:
	RefTable table;


public:
	inline RefCountTable()
	{
	};


	void ref(const Key &k)
	{
		typename RefTable::iterator iter;
		iter = table.find( k );

		if ( iter == table.end() )
		{
			//not already in table; add
			table[k] = 1;
		}
		else
		{
			//already in table; increment
			( iter->second )++;
		}
	}

	void unref(const Key &k)
	{
		typename RefTable::iterator iter = table.find( k );

		//can only unref a key that is already in the tavle
		if ( iter != table.end() )
		{
			int &count = iter->second;
			count--;

			//if count <= 0, remove
			if ( count <= 0 )
			{
				table.erase( iter );
			}
		}
	}


	void getKeyList(Array<Key> &l) const
	{
		l.reserveExtra( table.size() );

		for (typename RefTable::const_iterator iter = table.begin(); iter != table.end(); ++iter)
		{
			l.push_back( iter->first );
		}
	}


	int size() const
	{
		return table.size();
	}

	int estimateContentCost() const
	{
		return size()  *  ( sizeof(Key) + sizeof(int) );
	}


	const_iterator begin()
	{
		return table.begin();
	}

	const_iterator end()
	{
		return table.end();
	}
};



#endif
