//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MESHPAINTER_CPP__
#define MESHPAINTER_CPP__

// Uncomment line below to enable rendering of the mesh pointers (displays pointer values for verts, edges, and faces)
// #define DEBUG_PRINT_MESH_POINTERS


#ifdef DEBUG_PRINT_MESH_POINTERS
#include <sstream>
#endif



#include <Mesh/MMesh/MVertex.h>
#include <Mesh/MMesh/MEdge.h>
#include <Mesh/MMesh/MFace.h>
#include <Mesh/MMesh/MMesh.h>

#include <Mesh/MeshPainter.h>

#include <PlatformSpecific/IncludeGL.h>

//#include <Texture/TextureTable.h>

//#include <Material/Material.h>
//#include <Material/MaterialTable.h>

#include <Graphics/Drawing.h>
#include <Graphics/GLMatrix.h>

#include <Model/ModelDraw.h>

#include <View/ViewSettings.h>







RT_IMPLEMENT_TYPE( MeshPainter, ModelPainter );

REGISTER_PAINTER( GSProductMesh, MeshPainter );


MeshPainter::MeshPainter()
					: bMeshLiveSubdivisionEnabled( false ), meshLiveSubdivisionIterations( 1 ), meshLiveSubdivisionMaxFaces( 81920 ),
					meshLiveSubdivisionNormalSharpness( MPRESERVENORMALSHARPNESS_IFSHARP ), meshLiveSubdivisionWireframeMode( MESHLIVESUBDWIRE_CONTROL )
{
}



void MeshPainter::setBackgroundMeshViewSettings(bool bEnabled, int iterations, int maxFaces, MPreserveNormalSharpness normalSharpness, MeshLiveSubdWireframeMode wireframeMode)
{
	bMeshLiveSubdivisionEnabled = bEnabled;
	meshLiveSubdivisionIterations = iterations;
	meshLiveSubdivisionMaxFaces = maxFaces;
	meshLiveSubdivisionNormalSharpness = normalSharpness;
	meshLiveSubdivisionWireframeMode = wireframeMode;
}




void MeshPainter::i_paintModel3d(Viewport3dSettings *viewportSettings, GSProductModel *model, PaintLayer layer, bool root, bool background, int reflectionCount)
{
	ModelPainter::i_paintModel3d( viewportSettings, model, layer, root, background, reflectionCount );


	if ( model != NULL )
	{
		gs_assert_is_instance_of( model, GSProductMesh, "MeshPainter::i_paintModel3d()" );

		drawMesh( viewportSettings, static_cast<GSProductMesh*>( model ), layer, background, reflectionCount );
	}
}

void MeshPainter::i_paintModelUV(ViewportUVSettings *viewportSettings, GSProductModel *model, bool root)
{
	ModelPainter::i_paintModelUV( viewportSettings, model, root );


	if ( model != NULL )
	{
		gs_assert_is_instance_of( model, GSProductMesh, "MeshPainter::i_paintModelUV()" );

		drawMeshUV( viewportSettings, static_cast<GSProductMesh*>( model ) );
	}
}

void MeshPainter::drawMesh(Viewport3dSettings *viewportSettings, GSProductMesh *meshProduct, PaintLayer layer, bool background, int reflectionCount)
{
	MMesh *mesh = &( meshProduct->getMMeshForDisplay() );

	if ( mesh->getVertices().size() == 0 )
	{
		return;
	}

	ViewSettings *viewSettings = viewportSettings->getViewSettings();
	bool verticesFlag = viewportSettings->bForegroundVertices;
	bool cullFlag = viewSettings->bBackfaceCull;
	bool multilayerFlag = viewSettings->bMultilayer;
	bool markedfacesFlag = viewportSettings->bForegroundMarkedFaces;
	MeshLiveSubdWireframeMode liveSubdWireMode = background  ?  meshLiveSubdivisionWireframeMode  :  viewSettings->meshLiveSubdivisionWireframeMode;
	bool bLiveSubdivision = background  ?  bMeshLiveSubdivisionEnabled  :  viewSettings->bMeshLiveSubdivisionEnabled;
	bool bBackgroundTransparent = viewportSettings->bBackgroundTransparent;

	bool wireframeFlag, solidFlag, smoothFlag, texturedFlag;

	if ( background )
	{
		wireframeFlag = viewportSettings->bBackgroundWireframe;
		solidFlag = viewportSettings->bBackgroundSolid;
		smoothFlag = viewportSettings->bBackgroundSmooth;
		texturedFlag = viewportSettings->bBackgroundMaterial;
	}
	else
	{
		wireframeFlag = viewportSettings->bForegroundWireframe;
		solidFlag = viewportSettings->bForegroundSolid;
		smoothFlag = viewportSettings->bForegroundSmooth;
		texturedFlag = viewportSettings->bForegroundMaterial;
	}


	MMesh *subdMesh = NULL;

	if ( bLiveSubdivision )
	{
		int subdIterations = background  ?  meshLiveSubdivisionIterations  :  viewSettings->meshLiveSubdivisionIterations;
		int subdMaxFaces = background  ?  meshLiveSubdivisionMaxFaces  :  viewSettings->meshLiveSubdivisionMaxFaces;
		MPreserveNormalSharpness normalSharpness = background  ?  meshLiveSubdivisionNormalSharpness  :  viewSettings->meshLiveSubdivisionNormalSharpness;
		subdMesh = mesh->getLiveSubdivisionMesh( subdIterations, subdMaxFaces, normalSharpness );
	}

	if ( background  &&  bBackgroundTransparent )
	{
		if ( reflectionCount & 0x1 )
		{
			glCullFace( GL_FRONT );
		}
		else
		{
			glCullFace( GL_BACK );
		}
		glEnable( GL_CULL_FACE );


		if ( bLiveSubdivision )
		{
			if ( solidFlag   &&   ( layer == PAINTLAYER_OVERLAY_BLEND  ||  layer == PAINTLAYER_OVERLAY_BLEND_NODEPTH ) )
			{
				beginSolidRendering();
				drawSolidTransparent( subdMesh, smoothFlag );
				endSolidRendering();
			}
		}
		else
		{
			if ( solidFlag   &&   ( layer == PAINTLAYER_OVERLAY_BLEND  ||  layer == PAINTLAYER_OVERLAY_BLEND_NODEPTH ) )
			{
				beginSolidRendering();
				drawSolidTransparent( mesh, smoothFlag );
				endSolidRendering();
			}
		}

		if ( wireframeFlag )
		{
			if ( layer == PAINTLAYER_OVERLAY_BLEND  ||  layer == PAINTLAYER_OVERLAY_BLEND_NODEPTH )
			{
				drawWireframe( mesh, background, layer == PAINTLAYER_OVERLAY_BLEND_NODEPTH, reflectionCount != 0, true );
			}
		}

		glDisable( GL_CULL_FACE );
	}
	else
	{
		if ( cullFlag )
		{
			if ( reflectionCount & 0x1 )
			{
				glCullFace( GL_FRONT );
			}
			else
			{
				glCullFace( GL_BACK );
			}
			glEnable( GL_CULL_FACE );
		}


		if ( bLiveSubdivision )
		{
			if ( solidFlag  &&  layer == PAINTLAYER_OBJECTS )
			{
				beginSolidRendering();
				drawSolidUntextured( subdMesh, background, false, true, true );
				endSolidRendering();
			}
		}
		else
		{
			if ( solidFlag  &&  layer == PAINTLAYER_OBJECTS )
			{
				beginSolidRendering();
				drawSolid( viewportSettings, mesh, background, texturedFlag, reflectionCount != 0, smoothFlag );
				endSolidRendering();
			}
		}

		if ( bLiveSubdivision  &&  liveSubdWireMode != MESHLIVESUBDWIRE_NONE  &&  !background )
		{
			if ( layer == PAINTLAYER_WIREFRAME  ||  ( layer == PAINTLAYER_TRANSPARENCY  &&  multilayerFlag ) )
			{
				drawSubdividedWireframe( subdMesh, layer == PAINTLAYER_TRANSPARENCY, liveSubdWireMode );
			}
		}

		if ( wireframeFlag )
		{
			if ( layer == PAINTLAYER_WIREFRAME  ||  ( layer == PAINTLAYER_TRANSPARENCY  &&  multilayerFlag ) )
			{
				drawWireframe( mesh, background, layer == PAINTLAYER_TRANSPARENCY, reflectionCount != 0, false );
			}
		}

		if ( verticesFlag  &&  !background )
		{
			if ( layer == PAINTLAYER_WIREFRAME  ||  ( layer == PAINTLAYER_TRANSPARENCY  && multilayerFlag ) )
			{
				drawVertices( mesh, layer == PAINTLAYER_TRANSPARENCY, reflectionCount != 0 );
			}
		}

		if ( cullFlag )
		{
			glDisable( GL_CULL_FACE );
		}

		if ( markedfacesFlag  &&  layer == PAINTLAYER_OVERLAY_BLEND  &&  !background  &&  reflectionCount == 0 )
		{
			drawMarkedFacesPass( mesh );
		}
	}
}

void MeshPainter::drawSolid(Viewport3dSettings *viewportSettings, MMesh *mesh, bool background, bool texturedFlag, bool reflection, bool smooth)
{
//	ViewSetting *viewSettings = viewportSettings->getViewSettings();
//	MaterialTable &matTable = *( viewSettings->getMaterialTable() );



/*	if ( texturedFlag )
	{
		Array<int> materialList;
		mesh->getUsedMaterialList( materialList );
		//make sure untextured faces get rendered
		materialList.push_back( -1 );

		for (int m = 0; m < materialList.size(); m++)
		{
			UniqueID materialID = materialList[m];
			Material *mat = matTable.getMaterial( materialID );


			if ( mat != NULL )
			{
				//get the diffuse attributes: colour and the texture ID
				mat->renderInit();
				int numPasses = mat->getRenderNumPasses();

				for (int i = 0; i < numPasses; i++)
				{
					if ( reflection )
					{
						mat->renderGL( i, getReflectedSurfaceFilterColour() );
					}
					else
					{
						mat->renderGL( i );
					}
					drawSolidPass( mesh, materialID, smooth );
				}

				mat->renderFinish();
			}
			else
			{
				glDisable( GL_TEXTURE_2D );

				drawSolidPassUntextured( mesh, materialID, reflection, smooth );
			}
		}
	}
	else
	{*/
		drawSolidUntextured( mesh, background, reflection, false, smooth );
	//}
}

/*void MeshPainter::drawSolidPass(MMesh *mesh, int materialID, bool smooth)
{
	Array<MFace*> &faces = mesh->getFaces();

	int first = 0;
	int last = faces.size() - 1;

	glBegin( GL_TRIANGLES );

	for (int i = first; i <= last; i++)
	{
		MFace *fc = faces[i];

		int mat = fc->getMaterialID();

		if ( mat == materialID )
		{
			const Tesselation *t = fc->getTesselation();

			if ( t != NULL )
			{
				for (int i = 0; i < t->size(); i++)
				{
					drawTexturedFace( fc, t->at( i ).a, t->at( i ).b, t->at( i ).c, smooth );
				}
			}
			else
			{
				int a = 0, b = 1;

				for (int c = 2; c < fc->getSize(); c++)
				{
					drawTexturedFace( fc, a, b, c, smooth );

					b = c;
				}
			}
		}
	}

	glEnd();
}*/

/*void MeshPainter::drawSolidPassUntextured(MMesh *mesh, int materialID, bool reflection, bool smooth)
{
	Array<MFace*> &faces = mesh->getFaces();

	int first = 0;
	int last = faces.size() - 1;
	setupUntexturedMaterial( reflection );


	glBegin( GL_TRIANGLES );

	for (int i = first; i <= last; i++)
	{
		MFace *fc = faces[i];

		int mat = fc->getMaterialID();

		if ( mat == materialID )
		{
			const Tesselation *t = fc->getTesselation();

			if ( t != NULL )
			{
				for (int i = 0; i < t->size(); i++)
				{
					drawUntexturedFace( fc, t->at( i ).a, t->at( i ).b, t->at( i ).c, smooth );
				}
			}
			else
			{
				int a = 0, b = 1;

				for (int c = 2; c < fc->getSize(); c++)
				{
					drawUntexturedFace( fc, a, b, c, smooth );

					b = c;
				}
			}
		}
	}

	glEnd();
}*/

void MeshPainter::drawSolidUntextured(MMesh *mesh, bool background, bool reflection, bool subdivided, bool smooth)
{
	Array<MFace*> &faces = mesh->getFaces();

	int first = 0;
	int last = faces.size() - 1;

	if ( subdivided )
	{
		setupSubdividedMaterial();
	}
	else
	{
		setupUntexturedMaterial( reflection  &&  !background );
	}

	glBegin( GL_TRIANGLES );

	for (int i = first; i <= last; i++)
	{
		MFace *fc = faces[i];

		const Tesselation *t = fc->getTesselation();

		if ( t != NULL )
		{
			for (int i = 0; i < t->size(); i++)
			{
				drawUntexturedFace( fc, t->at( i ).a, t->at( i ).b, t->at( i ).c, smooth );
			}
		}
		else
		{
			int a = 0, b = 1;

			for (int c = 2; c < fc->getSize(); c++)
			{
				drawUntexturedFace( fc, a, b, c, smooth );

				b = c;
			}
		}
	}

	glEnd();


#ifdef DEBUG_PRINT_MESH_POINTERS
	if ( !reflection  &&  !subdivided )
	{
		glDisable( GL_LIGHTING );
		glColor3d( 0.0, 0.5, 0.0 );
		for (int i = first; i <= last; i++)
		{
			Point3 pos = faces[i]->computeCentroid();
			std::ostringstream stream;
			stream << faces[i] << (char)0x00;
			glDrawString3( pos, stream.str().c_str() );
		}
		glEnable( GL_LIGHTING );
	}
#endif
}

void MeshPainter::drawSolidTransparent(MMesh *mesh, bool smooth)
{
	Array<MFace*> &faces = mesh->getFaces();

	int first = 0;
	int last = faces.size() - 1;

	setupTransparentMaterial();

	glBegin( GL_TRIANGLES );

	for (int i = first; i <= last; i++)
	{
		MFace *fc = faces[i];

		const Tesselation *t = fc->getTesselation();

		if ( t != NULL )
		{
			for (int i = 0; i < t->size(); i++)
			{
				drawUntexturedFace( fc, t->at( i ).a, t->at( i ).b, t->at( i ).c, smooth );
			}
		}
		else
		{
			int a = 0, b = 1;

			for (int c = 2; c < fc->getSize(); c++)
			{
				drawUntexturedFace( fc, a, b, c, smooth );

				b = c;
			}
		}
	}

	glEnd();
}

void MeshPainter::drawMarkedFacesPass(MMesh *mesh)
{
	Array<MFace*> &faces = mesh->getFaces();


	int first = 0;
	int last = faces.size() - 1;

	glDepthMask( GL_FALSE );
	glDisable( GL_LIGHTING );
	glDisable( GL_TEXTURE_2D );

	glBegin( GL_TRIANGLES );
	glColor4( getMarkedFaceColour(), getMarkedFaceAlpha() );


	for (int i = first; i <= last; i++)
	{
		MFace *fc = faces[i];

		if ( fc->isFaceMarked() )
		{
			const Tesselation *t = fc->getTesselation();

			if ( t != NULL )
			{
				for (int i = 0; i < t->size(); i++)
				{
					drawFaceMark( fc, t->at( i ).a, t->at( i ).b, t->at( i ).c );
				}
			}
			else
			{
				int a = 0, b = 1;

				for (int c = 2; c < fc->getSize(); c++)
				{
					drawFaceMark( fc, a, b, c );

					b = c;
				}
			}
		}
	}
	glEnd();

	glDepthMask( GL_TRUE );
}



void MeshPainter::setupUntexturedMaterial(bool reflection)
{
	Colour3f diffCol = reflection  ?  getSurfaceDiffuseColour() * getReflectedSurfaceFilterColour() : getSurfaceDiffuseColour();
	const Colour3f &specCol = getSurfaceSpecularColour();
	GLfloat diff[] = { diffCol.r, diffCol.g, diffCol.b, 0.0f };
	GLfloat spec[] = { specCol.r, specCol.g, specCol.b, 0.0f };
	GLfloat shin[] = { 35.0f };

	glMaterialfv( GL_FRONT, GL_DIFFUSE, diff );
	glMaterialfv( GL_FRONT, GL_SPECULAR, spec );
	glMaterialfv( GL_FRONT, GL_SHININESS, shin );
}

void MeshPainter::setupSubdividedMaterial()
{
	const Colour3f &diffCol = getLiveSubdSurfaceDiffuseColour();
	const Colour3f &specCol = getLiveSubdSurfaceSpecularColour();

	GLfloat diff[] = { diffCol.r, diffCol.g, diffCol.b, 0.0f };
	GLfloat spec[] = { specCol.r, specCol.g, specCol.b, 0.0f };
	GLfloat shin[] = { 35.0f };

	glMaterialfv( GL_FRONT, GL_DIFFUSE, diff );
	glMaterialfv( GL_FRONT, GL_SPECULAR, spec );
	glMaterialfv( GL_FRONT, GL_SHININESS, shin );
}

void MeshPainter::setupTransparentMaterial()
{
	const Colour3f &diffCol = getTransparentBackgroundSurfaceDiffuseColour();
	const Colour3f &specCol = getTransparentBackgroundSurfaceSpecularColour();
	float alpha = getTransparentBackgroundSurfaceAlpha();

	GLfloat diff[] = { diffCol.r, diffCol.g, diffCol.b, alpha };
	GLfloat spec[] = { specCol.r, specCol.g, specCol.b, alpha };
	GLfloat shin[] = { 35.0f };

	glMaterialfv( GL_FRONT, GL_DIFFUSE, diff );
	glMaterialfv( GL_FRONT, GL_SPECULAR, spec );
	glMaterialfv( GL_FRONT, GL_SHININESS, shin );
}

void MeshPainter::beginSolidRendering()
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_POLYGON_OFFSET_FILL );

	glPolygonOffset( 1.0, 1.0 );

	glEnable( GL_LIGHTING );
}

void MeshPainter::endSolidRendering()
{
	glDisable( GL_LIGHTING );

	glDisable( GL_POLYGON_OFFSET_FILL );
}



/*void MeshPainter::drawTexturedFace(MFace *fc, int a, int b, int c, bool smooth)
{
	if ( smooth )
	{
		const Point3 &vertexA = fc->getVertex( a )->getPosition();
		const Point2f &texA = fc->getVertexAttrib( a )->getPoint();
		const Vector3f &normalA = fc->getVertexNormal( a )->getNormal();
		const Point3 &vertexB = fc->getVertex( b )->getPosition();
		const Point2f &texB = fc->getVertexAttrib( b )->getPoint();
		const Vector3f &normalB = fc->getVertexNormal( b )->getNormal();
		const Point3 &vertexC = fc->getVertex( c )->getPosition();
		const Point2f &texC = fc->getVertexAttrib( c )->getPoint();
		const Vector3f &normalC = fc->getVertexNormal( c )->getNormal();

		glNormal3f( normalA.x, normalA.y, normalA.z );
		glTexCoord2f( texA.x, texA.y );
		glVertex3f( vertexA.x, vertexA.y, vertexA.z );
		glNormal3f( normalB.x, normalB.y, normalB.z );
		glTexCoord2f( texB.x, texB.y );
		glVertex3f( vertexB.x, vertexB.y, vertexB.z );
		glNormal3f( normalC.x, normalC.y, normalC.z );
		glTexCoord2f( texC.x, texC.y );
		glVertex3f( vertexC.x, vertexC.y, vertexC.z );
	}
	else
	{
		const Vector3 &normal = fc->getPlane().n;
		const Point3 &vertexA = fc->getVertex( a )->getPosition();
		const Point2f &texA = fc->getVertexAttrib( a )->getPoint();
		const Point3 &vertexB = fc->getVertex( b )->getPosition();
		const Point2f &texB = fc->getVertexAttrib( b )->getPoint();
		const Point3 &vertexC = fc->getVertex( c )->getPosition();
		const Point2f &texC = fc->getVertexAttrib( c )->getPoint();

		glNormal3f( normal.x, normal.y, normal.z );
		glTexCoord2f( texA.x, texA.y );
		glVertex3f( vertexA.x, vertexA.y, vertexA.z );
		glTexCoord2f( texB.x, texB.y );
		glVertex3f( vertexB.x, vertexB.y, vertexB.z );
		glTexCoord2f( texC.x, texC.y );
		glVertex3f( vertexC.x, vertexC.y, vertexC.z );
	}
}*/

void MeshPainter::drawUntexturedFace(MFace *fc, int a, int b, int c, bool smooth)
{
	if ( smooth )
	{
		const Point3 &vertexA = fc->getVertex( a )->getPosition();
		const Vector3f &normalA = fc->getVertexNormal( a )->getNormal();
		const Point3 &vertexB = fc->getVertex( b )->getPosition();
		const Vector3f &normalB = fc->getVertexNormal( b )->getNormal();
		const Point3 &vertexC = fc->getVertex( c )->getPosition();
		const Vector3f &normalC = fc->getVertexNormal( c )->getNormal();

		glNormal3f( normalA.x, normalA.y, normalA.z );
		glVertex3f( vertexA.x, vertexA.y, vertexA.z );
		glNormal3f( normalB.x, normalB.y, normalB.z );
		glVertex3f( vertexB.x, vertexB.y, vertexB.z );
		glNormal3f( normalC.x, normalC.y, normalC.z );
		glVertex3f( vertexC.x, vertexC.y, vertexC.z );
	}
	else
	{
		const Vector3 &normal = fc->getPlane().n;
		const Point3 &vertexA = fc->getVertex( a )->getPosition();
		const Point3 &vertexB = fc->getVertex( b )->getPosition();
		const Point3 &vertexC = fc->getVertex( c )->getPosition();

		glNormal3f( normal.x, normal.y, normal.z );
		glVertex3f( vertexA.x, vertexA.y, vertexA.z );
		glVertex3f( vertexB.x, vertexB.y, vertexB.z );
		glVertex3f( vertexC.x, vertexC.y, vertexC.z );
	}
}

void MeshPainter::drawFaceMark(MFace *fc, int a, int b, int c)
{
	const Point3 &vertexA = fc->getVertex( a )->getPosition();
	const Point3 &vertexB = fc->getVertex( b )->getPosition();
	const Point3 &vertexC = fc->getVertex( c )->getPosition();

	glVertex3f( vertexA.x, vertexA.y, vertexA.z );
	glVertex3f( vertexB.x, vertexB.y, vertexB.z );
	glVertex3f( vertexC.x, vertexC.y, vertexC.z );
}



void MeshPainter::drawVertices(MMesh *mesh, bool overlay, bool reflection)
{
	const Array<MVertex*> &vertices = mesh->getVertices();

	if ( vertices.size() > 0 )
	{
		int first = 0;
		int last = vertices.size() - 1;


		if ( reflection )
		{
			glPointSize( getReflectedVertexPointSize() );

			Colour3f vertexColour = getReflectedVertexColour();
			if ( overlay )
			{
				computeOverlayColour( vertexColour );
			}
			glColor3( vertexColour );


			glBegin( GL_POINTS );
			for (int i = first; i <= last; i++)
			{
				glVertex3( vertices[i]->getPosition() );
			}
			glEnd();
		}
		else
		{
			double unmarkedPointSize = getUnmarkedVertexPointSize();
			double markedPointSize = getMarkedVertexPointSize();
			Colour3f unmarkedColour = getUnmarkedVertexColour();
			Colour3f markedColour = getMarkedVertexColour();


			if ( overlay )
			{
				computeOverlayColour( unmarkedColour );
				computeOverlayColour( markedColour );
			}


			if ( unmarkedPointSize == markedPointSize )
			{
				// Marked and unmarked vertex point sizes are the same; render each vertex, changing colour as necessary
				bool marked = graphicsOverrideMarkState( vertices[first]->isVertexMarked() );

				glPointSize( unmarkedPointSize );
				if ( marked )
				{
					glColor3( markedColour );
				}
				else
				{
					glColor3( unmarkedColour );
				}

				glBegin( GL_POINTS );
				for (int i = first; i <= last; i++)
				{
					bool thisMarked = graphicsOverrideMarkState( vertices[i]->isVertexMarked() );
					if ( thisMarked != marked )
					{
						if ( thisMarked )
						{
							glColor3( markedColour );
						}
						else
						{
							glColor3( unmarkedColour );
						}
						marked = thisMarked;
					}

					glVertex3( vertices[i]->getPosition() );
				}
				glEnd();
			}
			else
			{
				// Marked and unmarked vertex point sizes are different; render unmarked, then marked
				glColor3( unmarkedColour );
				glPointSize( unmarkedPointSize );
				glBegin( GL_POINTS );
				for (int i = first; i <= last; i++)
				{
					if ( !graphicsOverrideMarkState( vertices[i]->isVertexMarked() ) )
					{
						glVertex3( vertices[i]->getPosition() );
					}
				}
				glEnd();

				glColor3( markedColour );
				glPointSize( markedPointSize );
				glBegin( GL_POINTS );
				for (int i = first; i <= last; i++)
				{
					if ( graphicsOverrideMarkState( vertices[i]->isVertexMarked() ) )
					{
						glVertex3( vertices[i]->getPosition() );
					}
				}
				glEnd();
			}
		}

#ifdef DEBUG_PRINT_MESH_POINTERS
		if ( !reflection  &&  !overlay )
		{
			const Colour3f &unmarkedColour = getUnmarkedVertexColour();
			glColor3( unmarkedColour );
			for (int i = first; i <= last; i++)
			{
				Point3 pos = vertices[i]->getPosition();
				std::ostringstream stream;
				stream << vertices[i] << (char)0x00;
				glDrawString3( pos, stream.str().c_str() );
			}
		}
#endif
	}

	glPointSize( 1.0 );
}




void MeshPainter::drawWireframe(MMesh *mesh, bool background, bool overlay, bool reflection, bool bBackgroundTransparent)
{
	const Array<MEdge*> &edges = mesh->getEdges();


	if ( edges.size() > 0 )
	{
		if ( background  ||  reflection )
		{
			// Render edges with single colour, single line width
			// Compute the edge colour
			Colour3f edgeColour;
			double edgeLineWidth = 1.0;

			if ( background )
			{
				if ( bBackgroundTransparent )
				{
					edgeColour = getTransparentBackgroundEdgeColour();
					edgeLineWidth = getTransparentBackgroundEdgeLineWidth();
				}
				else
				{
					edgeColour = getBackgroundEdgeColour();
					edgeLineWidth = getBackgroundEdgeLineWidth();
				}
			}
			else if ( reflection )
			{
				edgeColour = getReflectedEdgeColour();
				edgeLineWidth = getReflectedEdgeLineWidth();
			}

			if ( overlay )
			{
				computeOverlayColour( edgeColour );
			}


			glColor3( edgeColour );
			glLineWidth( edgeLineWidth );
			glBegin( GL_LINES );
			for (int i = 0; i < edges.size(); i++)
			{
				glVertex3( edges[i]->getVertexA()->getPosition() );
				glVertex3( edges[i]->getVertexB()->getPosition() );
			}
			glEnd();
		}
		else
		{
			double unmarkedLineWidth = getUnmarkedEdgeLineWidth();
			double markedLineWidth = getMarkedEdgeLineWidth();
			Colour3f unmarkedColour = getUnmarkedEdgeColour();
			Colour3f markedColour = getMarkedEdgeColour();

			if ( overlay )
			{
				computeOverlayColour( unmarkedColour );
				computeOverlayColour( markedColour );
			}


			if ( unmarkedLineWidth == markedLineWidth )
			{
				// Marked and unmarked edge line widths are the same; render all at once, switching colour when necessary
				glLineWidth( unmarkedLineWidth );

				bool marked = edges[0]->isEdgeMarked();
				if ( marked )
				{
					glColor3( markedColour );
				}
				else
				{
					glColor3( unmarkedColour );
				}
				glBegin( GL_LINES );
				for (int i = 0; i < edges.size(); i++)
				{
					bool thisMarked = edges[i]->isEdgeMarked();
					if ( thisMarked != marked )
					{
						if ( thisMarked )
						{
							glColor3( markedColour );
						}
						else
						{
							glColor3( unmarkedColour );
						}
						marked = thisMarked;
					}

					glVertex3( edges[i]->getVertexA()->getPosition() );
					glVertex3( edges[i]->getVertexB()->getPosition() );
				}
				glEnd();
			}
			else
			{
				// Marked and unmarked edge line widths differ; render unmarked edges first, then marked edges
				glColor3( unmarkedColour );
				glLineWidth( unmarkedLineWidth );
				glBegin( GL_LINES );
				for (int i = 0; i < edges.size(); i++)
				{
					if ( !edges[i]->isEdgeMarked() )
					{
						glVertex3( edges[i]->getVertexA()->getPosition() );
						glVertex3( edges[i]->getVertexB()->getPosition() );
					}
				}
				glEnd();

				glColor3( markedColour );
				glLineWidth( markedLineWidth );
				glBegin( GL_LINES );
				for (int i = 0; i < edges.size(); i++)
				{
					if ( edges[i]->isEdgeMarked() )
					{
						glVertex3( edges[i]->getVertexA()->getPosition() );
						glVertex3( edges[i]->getVertexB()->getPosition() );
					}
				}
				glEnd();
			}
		}


#ifdef DEBUG_PRINT_MESH_POINTERS
		if ( !reflection  &&  !overlay  &&  !background )
		{
			for (int i = 0; i < edges.size(); i++)
			{
				Point3 pos = Point3::average( edges[i]->getVertexA()->getPosition(), edges[i]->getVertexB()->getPosition() );
				std::ostringstream stream;
				stream << edges[i] << (char)0x00;
				glDrawString3( pos, stream.str().c_str() );
			}
		}
#endif
	}

	glLineWidth( 1.0 );
}


void MeshPainter::drawSubdividedWireframe(MMesh *mesh, bool overlay, MeshLiveSubdWireframeMode wireMode)
{
	const Array<MEdge*> &edges = mesh->getEdges();


	if ( edges.size() > 0 )
	{
		Colour3f colour = getLiveSubdEdgeColour();

		if ( overlay )
		{
			computeOverlayColour( colour );
		}

		glColor3( colour );

		glLineWidth( getLiveSubdEdgeLineWidth() );
		glBegin( GL_LINES );
		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i]->isLiveSubdivisionEven()  ||  wireMode == MESHLIVESUBDWIRE_FULL )
			{
				glVertex3( edges[i]->getVertexA()->getPosition() );
				glVertex3( edges[i]->getVertexB()->getPosition() );
			}
		}
		glEnd();
	}

	glLineWidth( 1.0 );
}




void MeshPainter::drawMeshUV(ViewportUVSettings *viewportSettings, GSProductMesh *meshProduct)
{
	MMesh *mesh = &( meshProduct->getMMeshForDisplay() );

	if ( mesh->getVertices().size() == 0 )
	{
		return;
	}


	Array<MFace*> &faces = mesh->getFaces();


	glDisable( GL_LIGHTING );
	glDisable( GL_TEXTURE_2D );

	glBegin( GL_LINES );
	glColor4( getUVEdgeColour(), getUVEdgeAlpha() );


	for (int i = 0; i < faces.size(); i++)
	{
		MFace *fc = faces[i];

		int a = fc->getSize() - 1;

		for (int b = 0; b < fc->getSize(); b++)
		{
			glVertex3( fc->getVertexAttrib( a )->getPoint() );
			glVertex3( fc->getVertexAttrib( b )->getPoint() );

			a = b;
		}
	}
	glEnd();
}



#endif
