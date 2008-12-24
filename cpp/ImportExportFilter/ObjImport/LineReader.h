//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LINEREADER_H__
#define LINEREADER_H__

#include <stdio.h>

#define LINEREADER_BUFFERSIZE 1024



class LineReader
{
private:
	FILE *file;
	char *largeBuffer;
	int largeBufferSize;
	char buffer[LINEREADER_BUFFERSIZE];
	int fileStart;


public:
	LineReader(FILE *f);
	~LineReader();


	void reset();


	char * readLine();


private:
	char * readLongLine();
};



void testLineReader();


#endif

