//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef FILEIO_CPP__
#define FILEIO_CPP__

#include <FileIO/fileio.h>



std::string readString(FILE *f)
{
	int sz;
	char *s;
	std::string r;

	fread(&sz, sizeof(int), 1, f);

	s = new char[sz];

	fread(s, sizeof(char), sz, f);

	std::string result = s;

	delete[] s;

	return result;
}

void writeString(FILE *f, const std::string &s)
{
	int sz = s.size() + 1;

	fwrite( &sz, sizeof(int), 1, f );
	fwrite( s.c_str(), sizeof(char), sz, f );
}



void readIntArray(FILE *f, Array<int> &a)
{
	int size;
	fread( &size, sizeof(int), 1, f );
	a.clear();
	a.reserve( size );

	for (int i = 0; i < size; i++)
	{
		int n;
		fread( &n, sizeof(int), 1, f );
		a.push_back(n);
	}
}

void writeIntArray(FILE *f, const Array<int> &a)
{
	int size = a.size();
	fwrite( &size, sizeof(int), 1, f );

	for (int i = 0; i < size; i++)
	{
		int n = a[i];
		fwrite( &n, sizeof(int), 1, f );
	}
}




#endif
