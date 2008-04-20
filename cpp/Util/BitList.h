//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BITLIST_H__
#define BITLIST_H__

#include <memory.h>

#include <assert.h>

#define UNIT_SIZE_IN_BITS 32
#define UNIT_SIZE_IN_BYTES 4


#define DIVIDE_BY_UNIT_SIZE_IN_BITS_RIGHT_SHIFT		5
#define REMAINDER_UNIT_SIZE_IN_BITS_MASK 			0x1f
#define MULTIPLY_UNIT_SIZE_IN_BYTES_LEFT_SHIFT		2


#define FULL_MASK		0xffffffff



/********************************************************************************
						BitList
 ********************************************************************************/

class GS_DllExport BitList
{
private:
	typedef unsigned int Unit;

	Unit *data;
	int numBits, dataSize;


public:
	inline BitList(int sz = 0)
			: numBits( sz )
	{
		if ( numBits != 0 )
		{
			dataSize = computeDataSize( numBits );
			data = new Unit[dataSize];
		}
		else
		{
			data = NULL;
			dataSize = 0;
		}

		zero();
	}

	inline BitList(const BitList &b)
			: numBits( b.numBits ), dataSize( b.dataSize )
	{
		if ( numBits != 0 )
		{
			data = new Unit[dataSize];
			memcpy( data, b.data, dataSize << MULTIPLY_UNIT_SIZE_IN_BYTES_LEFT_SHIFT );
		}
		else
		{
			data = NULL;
		}
	}


	inline ~BitList()
	{
		if ( data != NULL )
		{
			delete[] data;
			data = NULL;
		}
	}


	inline BitList & operator=(const BitList &b)
	{
		if ( dataSize != b.dataSize )
		{
			if ( data != NULL )
			{
				delete[] data;
				data = NULL;
			}

			dataSize = b.dataSize;

			if ( dataSize != 0 )
			{
				data = new Unit[dataSize];
			}
			else
			{
				data = NULL;
			}
		}

		numBits = b.numBits;
		if ( dataSize != 0 )
		{
			memcpy( data, b.data, dataSize << MULTIPLY_UNIT_SIZE_IN_BYTES_LEFT_SHIFT );
		}

		return *this;
	}


	inline bool operator[](int index) const
	{
		return get( index );
	}


	inline bool get(int index) const
	{
		int dataIndex = index >> DIVIDE_BY_UNIT_SIZE_IN_BITS_RIGHT_SHIFT;
		int bitIndex = index & REMAINDER_UNIT_SIZE_IN_BITS_MASK;
		return ( data[dataIndex] >> bitIndex )  &  0x1;
	}

	inline void set(int index, bool value) const
	{
		int x = value  ?  1  :  0;
		int dataIndex = index >> DIVIDE_BY_UNIT_SIZE_IN_BITS_RIGHT_SHIFT;
		int bitIndex = index & REMAINDER_UNIT_SIZE_IN_BITS_MASK;
		data[dataIndex] = ( data[dataIndex] & ~( 0x1 << bitIndex ) )  |  ( x  <<  bitIndex );
	}


	inline bool findSetBit(int start, int end) const
	{
		int startIndex = start >> DIVIDE_BY_UNIT_SIZE_IN_BITS_RIGHT_SHIFT;
		int startBit = start & REMAINDER_UNIT_SIZE_IN_BITS_MASK;

		int endIndex = end >> DIVIDE_BY_UNIT_SIZE_IN_BITS_RIGHT_SHIFT;
		int endBit = end & REMAINDER_UNIT_SIZE_IN_BITS_MASK;

		if ( ( data[startIndex] >> startBit )  !=  0 )
		{
			return true;
		}

		for (int i = startIndex + 1; i < endIndex; i++)
		{
			if ( data[i] != 0 )
			{
				return true;
			}
		}

		if ( endBit != 0 )
		{
			Unit mask = FULL_MASK  >>  ( UNIT_SIZE_IN_BITS - endBit );
			if ( ( data[endIndex] & mask )  !=  0 )
			{
				return true;
			}
		}

		return false;
	}

	inline bool findSetBit() const
	{
		return findSetBit( 0, numBits );
	}


	inline void resize(int sz)
	{
		int newDataSize = computeDataSize( sz );

		if ( newDataSize > dataSize )
		{
			// Allocate new data array
			Unit *newData = new Unit[newDataSize];

			// Copy existing data over
			if ( dataSize != 0 )
			{
				memcpy( newData, data, dataSize << MULTIPLY_UNIT_SIZE_IN_BYTES_LEFT_SHIFT );
			}


			// Free old data array
			if ( data != NULL )
			{
				delete[] data;
				data = NULL;
			}

			data = newData;
			dataSize = newDataSize;
		}

		numBits = sz;
	}


	inline void zero()
	{
		if ( data != NULL )
		{
			memset( data, 0, dataSize << MULTIPLY_UNIT_SIZE_IN_BYTES_LEFT_SHIFT );
		}
	}

	inline int size() const
	{
		return numBits;
	}



private:
	inline static int computeDataSize(int sz)
	{
		int dSize = sz >> DIVIDE_BY_UNIT_SIZE_IN_BITS_RIGHT_SHIFT;
		return ( sz & REMAINDER_UNIT_SIZE_IN_BITS_MASK )  >  0   ?   dSize + 1  :  dSize;
	}
};


#undef UNIT_SIZE_IN_BITS
#undef UNIT_SIZE_IN_BYTES

#undef DIVIDE_BY_UNIT_SIZE_IN_BITS_RIGHT_SHIFT
#undef REMAINDER_UNIT_SIZE_IN_BITS_MASK
#undef MULTIPLY_UNIT_SIZE_IN_BYTES_LEFT_SHIFT

#undef FULL_MASK


#endif
