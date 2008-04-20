//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
// ArrayMap
// O(n) insertion and removal
// O(log2(n)) access time
#ifndef ARRAYMAP_H__
#define ARRAYMAP_H__

#include <algorithm>
#include <utility>

#include <Util/Array.h>
#include <Util/ArraySet.h>




template <typename Key, typename Value> class ArrayMap
{
private:
	typedef std::pair<Key,Value> KeyValuePair;
	typedef Array<KeyValuePair> KeyValueTable;


	KeyValueTable data;


	struct KeyCompare
	{
		inline bool operator()(const KeyValuePair &a, const Key &b) const
		{
			return a.first < b;
		}

		inline bool operator()(const Key &a, const KeyValuePair &b) const
		{
			return a < b.first;
		}
	};


public:
	typedef typename KeyValueTable::const_iterator const_iterator;
	typedef typename KeyValueTable::iterator iterator;


public:
	// Default constructor
	inline ArrayMap()
	{
	}

	// Initialize from a set of keys
	inline ArrayMap(const ArraySet<Key> &keys)
	{
		data.resize( keys.size() );

		int i = 0;
		for (typename ArraySet<Key>::const_iterator iter = keys.begin(); iter != keys.end(); ++iter)
		{
			data[i++] = KeyValuePair( *iter, Value() );
		}
	}





	inline void insert(const Key &key, const Value &value)
	{
		iterator iter = std::lower_bound( begin(), end(), key, KeyCompare() );

		if ( iter != end() )
		{
			if ( iter->first == key )
			{
				iter->second = value;
				return;
			}
		}

		data.insert( iter, KeyValuePair( key, value ) );
	}

	inline bool remove(const Key &key)
	{
		iterator iter = std::lower_bound( begin(), end(), key, KeyCompare() );

		if ( iter != end() )
		{
			if ( iter->first == key )
			{
				data.remove( iter );
				return true;
			}
		}

		return false;
	}



	inline iterator find(const Key &key)
	{
		iterator iter = std::lower_bound( begin(), end(), key, KeyCompare() );

		if ( iter != end() )
		{
			return iter->first == key  ?  iter  :  end();
		}
		else
		{
			return end();
		}
	}

	inline const_iterator find(const Key &key) const
	{
		const_iterator iter = std::lower_bound( begin(), end(), key, KeyCompare() );

		if ( iter != end() )
		{
			return iter->first == key  ?  iter  :  end();
		}
		else
		{
			return end();
		}
	}


	inline bool contains(const Key &key) const
	{
		return find( key )  !=  end();
	}

	inline int size() const
	{
		return data.size();
	}




	inline bool set(const Key &key, const Value &value)
	{
		iterator iter = std::lower_bound( begin(), end(), key, KeyCompare() );

		if ( iter != end() )
		{
			if ( iter->first == key )
			{
				iter->second = value;
				return true;
			}
		}

		return false;
	}

	void clear()
	{
		data.clear();
	}





	inline iterator begin()
	{
		return data.begin();
	}

	inline iterator end()
	{
		return data.end();
	}

	inline const_iterator begin() const
	{
		return data.begin();
	}

	inline const_iterator end() const
	{
		return data.end();
	}
};



#endif
