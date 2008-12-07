//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJSTRINGUTILS_H__
#define OBJSTRINGUTILS_H__

#include <stdio.h>


inline bool isWhitespace(const char x)
{
	return x == ' '  ||  x == '\t';
}

inline bool isAtEnd(const char *x)
{
	return x == NULL  ||  *x == 0x00  ||  *x == '\r'  ||  *x == '\n';
}



inline char * findSlash(char *data)
{
	while ( *data != '/' )
	{
		if ( isAtEnd( data ) )
		{
			return NULL;
		}

		data++;
	}
	return data;
}


inline bool isFaceTripletChar(char c)
{
	return ( c >= '0'  &&  c <= '9' )  ||  c == '-'  ||  c == '/';
}

inline char * skipFaceTriplet(char *data)
{
	while ( isFaceTripletChar( *data ) )
	{
		data++;
	}
	return data;
}


inline bool isNameChar(char c)
{
	return ( c >= '0'  &&  c <= '9' )  ||  ( c >= 'A'  &&  c <= 'Z' )  ||  ( c >= 'a'  &&  c <= 'z' );
}

inline char * skipName(char *data)
{
	while ( isNameChar( *data ) )
	{
		data++;
	}
	return data;
}


inline char * skipWhitespace(char *x)
{
	while ( isWhitespace( *x ) )
	{
		x++;
	}
	return x;
}

inline char * skipNonWhitespace(char *x)
{
	while ( !isWhitespace( *x ) )
	{
		if ( isAtEnd( x ) )
		{
			return NULL;
		}
		x++;
	}
	return x;
}


int strToInt(const char *data);
double strToDouble(const char *data);




void testObjStringUtils();




#endif
