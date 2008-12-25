//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJIMPORT_H__
#define OBJIMPORT_H__

#include <Mesh/MMesh/MImportMesh.h>

#include <ImportExportFilter/ObjImport/ObjData.h>



GS_DllExport MImportMesh * convertObjDataGlobalModelToImportMesh(ObjData &objData);
GS_DllExport MImportMesh * convertObjModelToImportMesh(ObjData &objData, ObjModel &objModel);



#endif
