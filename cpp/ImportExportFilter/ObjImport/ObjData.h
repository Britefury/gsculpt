//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJDATA_H__
#define OBJDATA_H__

#include <string>
#include <vector>
#include <map>

#include <ImportExportFilter/ObjImport/LineReader.h>
#include <ImportExportFilter/ObjImport/ObjLayout.h>



class ObjData;




class ObjVertex
{
public:
	float v[4];

	inline ObjVertex()
	{
		v[0] = v[1] = v[2] = 0.0f;
		v[3] = 1.0f;
	}
};

class ObjVNormal
{
public:
	float v[3];

	inline ObjVNormal()
	{
		v[0] = v[1] = v[2] = 0.0f;
	}
};

class ObjVTexture
{
public:
	float v[3];

	inline ObjVTexture()
	{
		v[0] = v[1] = v[2] = 0.0f;
	}
};

class ObjFaceVertex
{
public:
	int v, vt, vn;

	inline ObjFaceVertex()
	{
		v = vt = vn = -1;
	}
};

class ObjFace
{
public:
	int numFV;
	ObjFaceVertex *v;

	inline ObjFace()
	{
		numFV = 0;
		v = NULL;
	}
};




class ObjModel
{
private:
	class ModelIndexBuffer
	{
	public:
		int *vGlobalToModel, *vtGlobalToModel, *vnGlobalToModel;
		int *vModelToGlobal, *vtModelToGlobal, *vnModelToGlobal;

		ModelIndexBuffer(ObjLayout *layout);
		~ModelIndexBuffer();


		void reset(ObjLayout *layout);
	};

public:
	ObjModelLayout *layout;

	int numV, numVT, numVN, numF, numFV;
	int *vIndices, *vtIndices, *vnIndices;
	ObjFace *f;
	ObjFaceVertex *fv;



private:
	int indexF, indexFV;


public:
	ObjModel(ObjModelLayout *layout);
	~ObjModel();


private:
	void addGlobalFace(ObjFace &face);
	void build(ModelIndexBuffer &buffer, ProgressMonitor *monitor, float progressOffset, float progressScale);

	

	friend class ObjData;
};





class ObjData
{
private:
	class ObjReaderState
	{
	public:
		int indexV, indexVT, indexVN, indexF, indexFV;


		ObjReaderState();
	};

public:
	ObjLayout *layout;
	ObjVertex *v;
	ObjVNormal *vn;
	ObjVTexture *vt;
	ObjFace *f;
	ObjFaceVertex *fv;

	ObjModel *defaultModel;
	std::map<std::string, ObjModel*> models;

private:
	std::vector<ObjModel*> activeModels;



public:
	ObjData(ObjLayout *layout, LineReader &reader, ProgressMonitor *readGeometryMonitor = NULL, ProgressMonitor *buildModelsMonitor = NULL);
	~ObjData();


private:
	void processLine(ObjReaderState &state, char *line);
	void readV(int index, char *line);
	void readVN(int index, char *line);
	void readVT(int index, char *line);
	void readF(ObjReaderState &state, int index, char *line);
	void readG(char *line);

	void readFloats(float *f, char *data, int numFloats);
	int readFaceVerts(ObjReaderState &state, ObjFace &face, char *data);
	void readFaceVert(ObjReaderState &state, ObjFaceVertex &vertex, char *data);
	char * findNextFaceVertTripletEntry(char *data);
	int getFaceVertexIndex(char *data, int currentPos);
};


#endif
