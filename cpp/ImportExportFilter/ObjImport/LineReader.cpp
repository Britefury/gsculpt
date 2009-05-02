//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LINEREADER_CPP__
#define LINEREADER_CPP__

#include <stdlib.h>
#include <string.h>

#include <ImportExportFilter/ObjImport/LineReader.h>


LineReader::LineReader(FILE *f)
{
	file = f;
	largeBuffer = NULL;
	largeBufferSize = 0;
	fileStart = ftell( file );
}

LineReader::~LineReader()
{
	if ( largeBuffer != NULL )
	{
		free( largeBuffer );
	}
}



void LineReader::reset()
{
	fseek( file, fileStart, SEEK_SET );
}

int LineReader::getFileSize()
{
	int pos = ftell( file );
	fseek( file, 0, SEEK_END );
	int end = ftell( file );
	fseek( file, pos, SEEK_SET );
	return end - fileStart;
}

int LineReader::getPos()
{
	return ftell( file )  -  fileStart;
}



char * LineReader::readLine()
{
	buffer[LINEREADER_BUFFERSIZE-1] = 0xff;
	buffer[LINEREADER_BUFFERSIZE-2] = 0xff;
	char *line = fgets( buffer, LINEREADER_BUFFERSIZE, file );

	if ( line == NULL )
	{
		return NULL;
	}

	if ( buffer[LINEREADER_BUFFERSIZE-1] == 0x00  &&  buffer[LINEREADER_BUFFERSIZE-2] != '\n' )
	{
		return readLongLine();
	}
	else
	{
		return line;
	}
}


char * LineReader::readLongLine()
{
	char *destBuffer = largeBuffer;
	char *line = NULL;
	int destPos = 0;

	while ( buffer[LINEREADER_BUFFERSIZE-1] == 0x00  &&  buffer[LINEREADER_BUFFERSIZE-2] != '\n' )
	{
		// The line is longer than the buffer
		if ( largeBuffer == NULL )
		{
			largeBufferSize = LINEREADER_BUFFERSIZE * 2 - 1;
			largeBuffer = (char*)malloc( largeBufferSize );

			destBuffer = largeBuffer;
		}
		else
		{
			int pos = destPos + LINEREADER_BUFFERSIZE;

			if ( pos > largeBufferSize )
			{
				largeBuffer = (char*)realloc( largeBuffer, pos );
				largeBufferSize = pos;
			}
		}

		memcpy( destBuffer, buffer, LINEREADER_BUFFERSIZE );
		destBuffer += LINEREADER_BUFFERSIZE - 1;
		destPos += LINEREADER_BUFFERSIZE - 1;


		buffer[LINEREADER_BUFFERSIZE-1] = 0xff;
		buffer[LINEREADER_BUFFERSIZE-2] = 0xff;
		line = fgets( buffer, LINEREADER_BUFFERSIZE, file );

		if ( line == NULL )
		{
			return largeBuffer;
		}
	}


	int n = strlen( buffer );
	int pos = destPos + n + 1;

	if ( pos > largeBufferSize )
	{
		largeBuffer = (char*)realloc( largeBuffer, pos );
		largeBufferSize = pos;
	}

	memcpy( destBuffer, buffer, n + 1 );
	destBuffer += n;
	destPos += n;

	return largeBuffer;
}




void testLineReader()
{
	const char *content = "0123\n0123456789abcdef\n0123456789abcdeg\n0123\n";

	FILE *f = fopen( "test.txt", "w" );
	fwrite( content, 1, strlen( content ), f );
	fclose( f );


	f = fopen( "test.txt", "r" );
	LineReader r( f );

	char *line = r.readLine();

	while ( line != NULL )
	{
		printf( "%s\n", line );
		line = r.readLine();
	}
}



#endif

