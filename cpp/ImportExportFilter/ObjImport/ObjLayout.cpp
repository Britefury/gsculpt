//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJLAYOUT_CPP__
#define OBJLAYOUT_CPP__


#include <ImportExportFilter/ObjImport/ObjStringUtils.h>

#include <ImportExportFilter/ObjImport/ObjLayout.h>



ObjLayout::ObjLayout(FILE *file, bool bProcessModels)
{
	this->bProcessModels = bProcessModels;
	numV = numVT = numVN = numF = numFV = 0;
	char buffer[1025];

	if ( bProcessModels )
	{
		defaultModel = new ObjModelLayout( "default" );
		models["default"] = defaultModel;
		activeModels.push_back( defaultModel );
	}

	fseek( file, 0, SEEK_SET );
	char *line = fgets( buffer, 1024, file );
	while ( line != NULL )
	{
		processLine( line );
		line = fgets( buffer, 1024, file );
	}

	if ( bProcessModels )
	{
		if ( defaultModel->numF == 0 )
		{
			if ( models["default"]  ==  defaultModel )
			{
				models.erase( models.find( "default" ) );
			}
			for (std::vector<ObjModelLayout*>::iterator iter = activeModels.begin(); iter != activeModels.end(); iter++)
			{
				if ( defaultModel == *iter )
				{
					activeModels.erase( iter );
					break;
				}
			}
			delete defaultModel;
			defaultModel = NULL;
		}
	}
}

ObjLayout::~ObjLayout()
{
	for (std::map<std::string, ObjModelLayout*>::iterator iter = models.begin(); iter != models.end(); iter++)
	{
		delete iter->second;
	}

	activeModels.clear();
	models.clear();
}



void ObjLayout::processLine(char *line)
{
	line = skipWhitespace( line );
	switch ( line[0] )
	{
	case 'v':
		switch ( line[1] )
		{
		case ' ':
		case '\t':
			numV++;
			break;
		case 't':
			if ( isWhitespace( line[2] ) )
			{
				numVT++;
			}
			break;
		case 'n':
			if ( isWhitespace( line[2] ) )
			{
				numVN++;
			}
			break;
		default:
			break;
		}
		break;
	case 'f':
		if ( isWhitespace( line[1] ) )
		{
			int faceSize = initF( line + 2 );
			numF++;
			if ( bProcessModels )
			{
				for (std::vector<ObjModelLayout*>::iterator iter = activeModels.begin(); iter != activeModels.end(); iter++)
				{
					ObjModelLayout *model = *iter;
					model->numF++;
					model->numFV += faceSize;
				}
			}
		}
		break;
	case 'g':
		if ( isWhitespace( line[1] ) )
		{
			initG( line + 2 );
		}
		break;
	default:
		break;
	}
}


int ObjLayout::initF(char *data)
{
	int faceSize = 0;
	while ( true )
	{
		if ( isAtEnd( data ) )
		{
			break;
		}

		data = skipWhitespace( data );

		if ( isAtEnd( data ) )
		{
			break;
		}

		char *d2 = skipFaceTriplet( data );
		if ( d2 != data )
		{
			numFV++;
			faceSize++;
		}
		else
		{
			break;
		}

		data = d2;
	}
	return faceSize;
}


void ObjLayout::initG(char *data)
{
	if ( bProcessModels )
	{
		activeModels.clear();
		while ( true )
		{
			if ( isAtEnd( data ) )
			{
				break;
			}

			data = skipWhitespace( data );

			if ( isAtEnd( data ) )
			{
				break;
			}

			char *d2 = skipName( data );
			if ( d2 != data )
			{
				int length = d2 - data;
				std::string name( data, length );
				if ( models.find( name ) != models.end() )
				{
					activeModels.push_back( models[name] );
				}
				else
				{
					ObjModelLayout *m = new ObjModelLayout( name );
					models[name] = m;
					activeModels.push_back( m );
				}
			}
			else
			{
				break;
			}

			data = d2;
		}

		if ( activeModels.size() == 0 )
		{
			activeModels.push_back( defaultModel );
		}
	}
}


#endif
