//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJIMPORTPROGRESS_H__
#define OBJIMPORTPROGRESS_H__


class GS_DllExport ObjImportProgress
{
public:
	enum Stage
	{
		STAGE_SCAN_STRUCTURE,
		STAGE_READ_GEOMETRY,
		STAGE_BUILD_MODELS,
		STAGE_FINISHED
	};
	
	
	Stage stage;
	
	// Scan structure stage
	int fileSize;
	int bytesRead, numLines;
	
	// Read geometry stage
	int geometrySize, geometryRead;
	
	// Build models stage
	int numModels, modelsBuilt, modelFaces, modelFacesBuilt;
	
	
	
	
	
	inline ObjImportProgress()
	{
		stage = STAGE_SCAN_STRUCTURE;
		
		fileSize = bytesRead = numLines = 0;
		geometrySize = geometryRead = 0;
		numModels = modelsBuilt = modelFaces = modelFacesBuilt = 0;
	}
	
	
	
	
	inline static ObjImportProgress scanStructureProgress(int fileSize, int bytesRead, int numLines)
	{
		ObjImportProgress p;
		p.stage = STAGE_SCAN_STRUCTURE;
		
		p.fileSize = fileSize;
		p.bytesRead = bytesRead;
		p.numLines = numLines;
		
		return p;
	}

	inline static ObjImportProgress readGeometryProgress(int geometrySize, int geometryRead)
	{
		ObjImportProgress p;
		p.stage = STAGE_READ_GEOMETRY;
		
		p.geometrySize = geometrySize;
		p.geometryRead = geometryRead;
		
		return p;
	}
	
	inline static ObjImportProgress buildModelsProgress(int numModels, int modelsBuilt)
	{
		ObjImportProgress p;
		p.stage = STAGE_BUILD_MODELS;
		
		p.numModels = numModels;
		p.modelsBuilt = modelsBuilt;
		p.modelFaces = 0;
		p.modelFacesBuilt = 0;
		
		return p;
	}
	
	inline ObjImportProgress buildModelProgress(int modelFaces, int modelFacesBuilt)
	{
		ObjImportProgress p;
		p.stage = STAGE_BUILD_MODELS;
		
		p.numModels = numModels;
		p.modelsBuilt = modelsBuilt;
		p.modelFaces = modelFaces;
		p.modelFacesBuilt = modelFacesBuilt;
		
		return p;
	}
	
	inline static ObjImportProgress finished()
	{
		ObjImportProgress p;
		p.stage = STAGE_FINISHED;
		
		return p;
	}
	
};


#endif
