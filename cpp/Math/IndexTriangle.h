//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef INDEXTRIANGLE_H__
#define INDEXTRIANGLE_H__

#include <stdio.h>

#include <Util/Array.h>
#include <Util/SETree.h>



class GS_DllExport IndexTriangle
{
public:
	int a, b, c;


	IndexTriangle() : a( -1 ), b( -1 ), c( -1 )
	{
	}

	IndexTriangle(int ia, int ib, int ic) : a( ia ), b( ib ), c( ic )
	{
	}



	void read(FILE *f)
	{
		fread( &a, sizeof(int), 1, f );
		fread( &b, sizeof(int), 1, f );
		fread( &c, sizeof(int), 1, f );
	}

	void write(FILE *f) const
	{
		fwrite( &a, sizeof(int), 1, f );
		fwrite( &b, sizeof(int), 1, f );
		fwrite( &c, sizeof(int), 1, f );
	}


	void read(SENode &node)
	{
		node[0]  >>  a;
		node[1]  >>  b;
		node[2]  >>  c;
	}

	void write(SENode &node) const
	{
		node.addItem()  <<  a;
		node.addItem()  <<  b;
		node.addItem()  <<  c;
	}
};


class GS_DllExport Tesselation
{
private:
	Array<IndexTriangle> tris;

public:
	Tesselation();
	~Tesselation();


	void read(FILE *f);
	void write(FILE *f) const;
	void read(SENode &node);
	void write(SENode &node) const;

	void reserve(int s);
	void add(const IndexTriangle &t);
	const IndexTriangle & operator[](int i) const;
	const IndexTriangle & at(int i) const;
	int size() const;
	void clear();

	void optimiseMemoryUsageLean();
	void optimiseMemoryUsageFor(int s);
};



#endif
