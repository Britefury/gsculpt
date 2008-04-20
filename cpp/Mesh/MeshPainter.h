//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MESHPAINTER_H__
#define MESHPAINTER_H__

#include <Mesh/GSProductMesh.h>
#include <Mesh/MeshLiveSubdWireframeMode.h>

#include <Model/ModelPainter.h>

//#include <Texture/Texture.h>


class GS_DllExport MeshPainter : public ModelPainter
{
RT_TYPE;
private:
	bool bMeshLiveSubdivisionEnabled;
	int meshLiveSubdivisionIterations, meshLiveSubdivisionMaxFaces;
	MPreserveNormalSharpness meshLiveSubdivisionNormalSharpness;
	MeshLiveSubdWireframeMode meshLiveSubdivisionWireframeMode;

public:
	MeshPainter();


	void setBackgroundMeshViewSettings(bool bEnabled, int iterations, int maxFaces, MPreserveNormalSharpness normalSharpness, MeshLiveSubdWireframeMode wireframeMode);



protected:
	virtual void i_paintModel3d(Viewport3dSettings *viewportSettings, GSProductModel *model, PaintLayer layer, bool root, bool background, int reflectionCount);
	virtual void i_paintModelUV(ViewportUVSettings *viewportSettings, GSProductModel *model, bool root);

private:
	void drawMesh(Viewport3dSettings *viewportSettings, GSProductMesh *meshProduct, PaintLayer layer, bool background, int reflectionCount);
	void drawSolid(Viewport3dSettings *viewportSettings, MMesh *mesh, bool background, bool texturedFlag, bool reflection, bool smooth);
	//void drawSolidPass(MMesh *mesh, int materialID, bool smooth);
	//void drawSolidPassUntextured(MMesh *mesh, int materialID, bool reflection, bool smooth);
	void drawSolidUntextured(MMesh *mesh, bool background, bool reflection, bool subdivided, bool smooth);
	void drawSolidTransparent(MMesh *mesh, bool smooth);
	void drawMarkedFacesPass(MMesh *mesh);

	void setupUntexturedMaterial(bool reflection);
	void setupSubdividedMaterial();
	void setupTransparentMaterial();
	void beginSolidRendering();
	void endSolidRendering();

	//void drawTexturedFace(MFace *fc, int a, int b, int c, bool smooth);
	void drawUntexturedFace(MFace *fc, int a, int b, int c, bool smooth);
	void drawFaceMark(MFace *fc, int a, int b, int c);

	void drawVertices(MMesh *mesh, bool overlay, bool reflection);
	void drawWireframe(MMesh *mesh, bool background, bool overlay, bool reflection, bool bBackgroundTransparent);
	void drawSubdividedWireframe(MMesh *mesh, bool overlay, MeshLiveSubdWireframeMode wireMode);



	void drawMeshUV(ViewportUVSettings *viewportSettings, GSProductMesh *meshProduct);
};


#endif
