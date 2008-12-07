//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJDATA_CPP__
#define OBJDATA_CPP__

#include <memory.h>

#include <ImportExportFilter/ObjImport/ObjStringUtils.h>
#include <ImportExportFilter/ObjImport/ObjLayout.h>

#include <ImportExportFilter/ObjImport/ObjData.h>




ObjModel::ModelIndexBuffer::ModelIndexBuffer(ObjLayout *layout)
{
	vGlobalToModel = new int[layout->numV];
	vtGlobalToModel = new int[layout->numVT];
	vnGlobalToModel = new int[layout->numVN];

	vModelToGlobal = new int[layout->numV];
	vtModelToGlobal = new int[layout->numVT];
	vnModelToGlobal = new int[layout->numVN];
}

ObjModel::ModelIndexBuffer::~ModelIndexBuffer()
{
	delete[] vGlobalToModel;
	delete[] vtGlobalToModel;
	delete[] vnGlobalToModel;

	delete[] vModelToGlobal;
	delete[] vtModelToGlobal;
	delete[] vnModelToGlobal;
}


void ObjModel::ModelIndexBuffer::reset(ObjLayout *layout)
{
	memset( vGlobalToModel, 0xff, sizeof(int) * layout->numV );
	memset( vtGlobalToModel, 0xff, sizeof(int) * layout->numVT );
	memset( vnGlobalToModel, 0xff, sizeof(int) * layout->numVN );
}



ObjModel::ObjModel(ObjModelLayout *layout)
{
	this->layout = layout;
	f = new ObjFace[layout->numF];
	fv = new ObjFaceVertex[layout->numFV];
	indexF = indexFV = 0;
}

ObjModel::~ObjModel()
{
	delete[] f;
	delete[] fv;
}

	

void ObjModel::addGlobalFace(ObjFace &face)
{
	ObjFace &dest = f[indexF];
	dest.numFV = face.numFV;
	dest.v = fv + indexFV;
	indexF++;
	indexFV += face.numFV;

	for (int i = 0; i < face.numFV; i++)
	{
		dest.v[i] = face.v[i];
	}
}

void ObjModel::build(ModelIndexBuffer &buffer)
{
	int vCount = 0, vtCount = 0, vnCount = 0;

	for (int i = 0; i < layout->numF; i++)
	{
		for (int j = 0; j < f[i].numFV; j++)
		{
			ObjFaceVertex &fv = f[i].v[j];
			
			if ( buffer.vGlobalToModel[fv.v] == -1 )
			{
				buffer.vModelToGlobal[vCount] = fv.v;
				buffer.vGlobalToModel[fv.v] = vCount++;
			}
			fv.v = buffer.vGlobalToModel[fv.v];


			if ( buffer.vtGlobalToModel[fv.vt] == -1 )
			{
				buffer.vtModelToGlobal[vtCount] = fv.vt;
				buffer.vtGlobalToModel[fv.vt] = vtCount++;
			}
			fv.vt = buffer.vtGlobalToModel[fv.vt];


			if ( buffer.vnGlobalToModel[fv.vn] == -1 )
			{
				buffer.vnModelToGlobal[vnCount] = fv.vn;
				buffer.vnGlobalToModel[fv.vn] = vnCount++;
			}
			fv.vn = buffer.vnGlobalToModel[fv.vn];
		}
	}

	vIndices = new int[vCount];
	vtIndices = new int[vtCount];
	vnIndices = new int[vnCount];
	memcpy( vIndices, buffer.vModelToGlobal, sizeof(int)*vCount );
	memcpy( vtIndices, buffer.vtModelToGlobal, sizeof(int)*vtCount );
	memcpy( vnIndices, buffer.vnModelToGlobal, sizeof(int)*vnCount );
}

	
	
	
ObjData::ObjReaderState::ObjReaderState()
{
	indexV = indexVT = indexVN = indexF = indexFV = 0;
}







ObjData::ObjData(ObjLayout *layout, FILE *file)
{
	this->layout = layout;

	// Build the models
	if ( layout->bProcessModels )
	{
		for (std::map<std::string, ObjModelLayout*>::iterator iter = layout->models.begin(); iter != layout->models.end(); ++iter)
		{
			models[iter->first] = new ObjModel( iter->second );
		}

		if ( layout->defaultModel != NULL )
		{
			defaultModel = models["default"];
			activeModels.push_back( defaultModel );
		}
	}


	v = new ObjVertex[layout->numV];
	vt = new ObjVTexture[layout->numVT];
	vn = new ObjVNormal[layout->numVN];
	f = new ObjFace[layout->numF];
	fv = new ObjFaceVertex[layout->numFV];


	ObjReaderState state;
	fseek( file, 0, SEEK_SET );
	char buffer[1025];
	char *line = fgets( buffer, 1024, file );
	while ( line != NULL )
	{
		processLine( state, line );
		line = fgets( buffer, 1024, file );
	}


	if ( layout->bProcessModels )
	{
		ObjModel::ModelIndexBuffer buffer( layout );
		for (std::map<std::string, ObjModel*>::iterator iter = models.begin(); iter != models.end(); ++iter)
		{
			buffer.reset( layout );
			iter->second->build( buffer );
		}
	}
}

ObjData::~ObjData()
{
	delete[] v;
	delete[] vt;
	delete[] vn;
	delete[] f;
	delete[] fv;

	for (std::map<std::string, ObjModel*>::iterator iter = models.begin(); iter != models.end(); ++iter)
	{
		delete iter->second;
	}
}


void ObjData::processLine(ObjReaderState &state, char *line)
{
	line = skipWhitespace( line );
	switch ( line[0] )
	{
	case 'v':
		switch ( line[1] )
		{
		case ' ':
		case '\t':
			readV( state.indexV, line + 2 );
			state.indexV++;
			break;
		case 't':
			if ( isWhitespace( line[2] ) )
			{
				readVT( state.indexVT, line + 3 );
				state.indexVT++;
			}
			break;
		case 'n':
			if ( isWhitespace( line[2] ) )
			{
				readVN( state.indexVN, line + 3 );
				state.indexVN++;
			}
			break;
		default:
			break;
		}
		break;
	case 'f':
		if ( isWhitespace( line[1] ) )
		{
			readF( state, state.indexF, line + 2 );
			state.indexF++;
			if ( layout->bProcessModels )
			{
				for (std::vector<ObjModel*>::iterator iter = activeModels.begin(); iter != activeModels.end(); iter++)
				{
					(*iter)->addGlobalFace( f[state.indexF] );
				}
			}
		}
		break;
	case 'g':
		if ( isWhitespace( line[1] ) )
		{
			readG( line + 2 );
		}
		break;
	default:
		break;
	}
}




void ObjData::readV(int index, char *line)
{
	line = skipWhitespace( line );
	readFloats( v[index].v, line, 4 );
}

void ObjData::readVN(int index, char *line)
{
	line = skipWhitespace( line );
	readFloats( vn[index].v, line, 3 );
}

void ObjData::readVT(int index, char *line)
{
	line = skipWhitespace( line );
	readFloats( vt[index].v, line, 3 );
}

void ObjData::readF(ObjReaderState &state, int index, char *line)
{
	line = skipWhitespace( line );
	ObjFace &face = f[index];
	face.v = fv + state.indexFV;
	state.indexFV += readFaceVerts( state, face, line );
}

void ObjData::readG(char *data)
{
	if ( layout->bProcessModels )
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
				std::map<std::string, ObjModel*>::iterator iter = models.find( name );
				if ( iter != models.end() )
				{
					activeModels.push_back( iter->second );
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



void ObjData::readFloats(float *f, char *data, int numFloats)
{
	for (int i = 0; i < numFloats; i++)
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

		*f = (float)strToDouble( data );
		f++;

		data = skipNonWhitespace( data );
	}
}

int ObjData::readFaceVerts(ObjReaderState &state, ObjFace &face, char *data)
{
	ObjFaceVertex *fv = face.v;
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
			readFaceVert( state, *fv, data );
			face.numFV++;
			fv++;
		}
		else
		{
			break;
		}

		data = d2;
	}
	return face.numFV;
}

void ObjData::readFaceVert(ObjReaderState &state, ObjFaceVertex &vertex, char *data)
{
	char *vD = data;
	char *vtD = findNextFaceVertTripletEntry( vD );
	char *vnD = findNextFaceVertTripletEntry( vtD );
	vertex.v = getFaceVertexIndex( vD, state.indexV );
	vertex.vt = getFaceVertexIndex( vtD, state.indexVT );
	vertex.vn = getFaceVertexIndex( vnD, state.indexVN );
}

char * ObjData::findNextFaceVertTripletEntry(char *data)
{
	if ( data == NULL )
	{
		return NULL;
	}
	else
	{
		data = findSlash( data );
		if ( data == NULL )
		{
			return NULL;
		}
		else
		{
			return data + 1;
		}
	}
}

int ObjData::getFaceVertexIndex(char *data, int currentPos)
{
	if ( data == NULL )
	{
		return -1;
	}
	else if ( *data == '/'  ||  *data == 0x0a )
	{
		return -1;
	}
	else
	{
		int index = strToInt( data );
		if ( index > 0 )
		{
			index--;
		}
		else if ( index < 0 )
		{
			index = currentPos + index;
		}
		return index;
	}
}


#endif
