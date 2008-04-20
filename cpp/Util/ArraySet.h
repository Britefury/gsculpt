//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef ARRAYSET_H__
#define ARRAYSET_H__

#include <algorithm>
#include <utility>

#include <Util/Array.h>




template <typename T> class ArraySet
{
private:
	typedef Array<T> SetData;

	SetData data;


public:
	typedef typename SetData::const_iterator const_iterator;
	typedef typename SetData::iterator iterator;


public:
	inline ArraySet()
	{
	}





	inline void clear()
	{
		data.clear();
	}


	inline void insert(const T &value)
	{
		iterator iter = std::lower_bound( begin(), end(), value );

		if ( iter != end() )
		{
			if ( *iter == value )
			{
				return;
			}
		}

		data.insert( iter, value );
	}

	inline void remove(iterator iter)
	{
		data.remove( iter );
	}

	inline bool remove(const T &value)
	{
		iterator iter = std::lower_bound( begin(), end(), value );

		if ( iter != end() )
		{
			if ( *iter == value )
			{
				remove( iter );
				return true;
			}
		}

		return false;
	}



	inline bool contains(const T &value) const
	{
		const_iterator iter = std::lower_bound( begin(), end(), value );

		if ( iter != end() )
		{
			return *iter == value;
		}
		else
		{
			return false;
		}
	}


	inline void setUnion(const ArraySet<T> &s)
	{
		SetData newData;

		newData.resize( size() + s.size() );

		iterator resultEnd = std::set_union( begin(), end(), s.begin(), s.end(), newData.begin() );
		newData.resize( resultEnd - newData.begin() );

		data.swapArray( newData );
	}

	inline void setIntersection(const ArraySet<T> &s)
	{
		SetData newData;

		newData.resize( std::min( size(), s.size() ) );

		iterator resultEnd = std::set_intersection( begin(), end(), s.begin(), s.end(), newData.begin() );
		newData.resize( resultEnd - newData.begin() );

		data.swapArray( newData );
	}

	inline void setDifference(const ArraySet<T> &s)
	{
		SetData newData;

		newData.resize( size() );

		iterator resultEnd = std::set_difference( begin(), end(), s.begin(), s.end(), newData.begin() );
		newData.resize( resultEnd - newData.begin() );

		data.swapArray( newData );
	}

	inline void setSymmetricDifference(const ArraySet<T> &s)
	{
		SetData newData;

		newData.resize( size() + s.size() );

		iterator resultEnd = std::set_symmetric_difference( begin(), end(), s.begin(), s.end(), newData.begin() );
		newData.resize( resultEnd - newData.begin() );

		data.swapArray( newData );
	}


	inline int size() const
	{
		return data.size();
	}

	inline bool isEmpty() const
	{
		return data.isEmpty();
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
