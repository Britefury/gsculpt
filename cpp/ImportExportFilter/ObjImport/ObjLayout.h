//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJLAYOUT_H__
#define OBJLAYOUT_H__

#include <string>
#include <vector>
#include <map>

#include <ImportExportFilter/ObjImport/LineReader.h>

#include <Util/ProgressMonitor.h>



class GS_DllExport ObjModelLayout
{
public:
	std::string name;
	int numF, numFV;


	inline ObjModelLayout(const std::string &name)
	{
		this->name = name;
		numF = numFV = 0;
	}
};



class GS_DllExport ObjLayout
{
public:
	// Number of vertices, vertex-texture-coords, vertex-normals, faces, face vertices (sum of face sizes)
	int numV, numVT, numVN, numF, numFV;
	ObjModelLayout *defaultModel;
	std::map<std::string, ObjModelLayout*> models;

private:
	std::vector<ObjModelLayout*> activeModels;
	bool bProcessModels;


public:
	ObjLayout(LineReader &reader, bool bProcessModels, ProgressMonitor *monitor = NULL);
	~ObjLayout();


private:
	void processLine(char *line);
	int initF(char *data);
	void initG(char *data);


	friend class ObjData;
};


#endif
