##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.GL.GL import *

from Britefury.FileIO.IOXml import ioObjectFactoryRegister

from Britefury.Graphics.Graphics import glMultMatrix

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Math.Math import Point3, Quaternion, Vector3, Matrix4, Segment3f, Segment3, Point3f

from Britefury.Sheet.Sheet import *

from Britefury.Background.BackgroundModelFileRef import BackgroundModelFileRef
from Britefury.Background.BackgroundModelFileRefCell import BackgroundModelFileRefCell

from Britefury.WorkArea.Viewport3d import Viewport3dPainter

from Britefury.Model.Model import getColourPalette

from Britefury.View.View import PaintLayer
from Britefury.View.DrawingTemplate import DrawingTemplate
from Britefury.View import BackgroundModelCommandTracker




def _beginSolidRendering():
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL )
	glEnable( GL_POLYGON_OFFSET_FILL )

	glPolygonOffset( 1.0, 1.0 )

	glEnable( GL_LIGHTING )


def _endSolidRendering():
	glDisable( GL_LIGHTING )

	glDisable( GL_POLYGON_OFFSET_FILL )





def _setupSolidMaterial():
	palette = getColourPalette()
	
	diffCol = palette.surfaceDiffuseColour
	specCol = palette.surfaceSpecularColour
	
	diff = [ diffCol.r, diffCol.g, diffCol.b, 0.0 ]
	spec = [ specCol.r, specCol.g, specCol.b, 0.0 ]
	shin = [ 35.0 ]

	glMaterialfv( GL_FRONT, GL_DIFFUSE, diff )
	glMaterialfv( GL_FRONT, GL_SPECULAR, spec )
	glMaterialfv( GL_FRONT, GL_SHININESS, shin )


def _setupTransparentMaterial():
	palette = getColourPalette()
	
	diffCol = palette.transparentBackgroundSurfaceDiffuseColour
	specCol = palette.transparentBackgroundSurfaceSpecularColour
	alpha = palette.transparentBackgroundSurfaceAlpha
	
	diff = [ diffCol.r, diffCol.g, diffCol.b, alpha ]
	spec = [ specCol.r, specCol.g, specCol.b, alpha ]
	shin = [ 35.0 ]

	glMaterialfv( GL_FRONT, GL_DIFFUSE, diff )
	glMaterialfv( GL_FRONT, GL_SPECULAR, spec )
	glMaterialfv( GL_FRONT, GL_SHININESS, shin )




class BackgroundModel (Sheet):
	trackerClass = BackgroundModelCommandTracker.BackgroundModelCommandTracker

	model = Field( BackgroundModelFileRef, BackgroundModelFileRef() )
	position = Field( Point3, Point3() )
	scale = Field( Vector3, Vector3( 1.0, 1.0, 1.0 ) )
	rotation = Field( Quaternion, Quaternion() )
	bOverlay = Field( bool, False )
	bVisible = Field( bool, True )

	modelPathChangedSignal = ClassSignal()


	def __init__(self):
		super( BackgroundModel, self ).__init__()
		self.cells.model.changedSignal.connect( self._onModelPathChanged )



	def paint3d(self, paintLayer):
		if self.bVisible:
			if self.bOverlay:
				if paintLayer == PaintLayer.PAINTLAYER_OVERLAY_BLEND  or  paintLayer == PaintLayer.PAINTLAYER_OVERLAY_BLEND_NODEPTH:
					glCullFace( GL_BACK )
					glEnable( GL_CULL_FACE )
	
					matrix = Matrix4.scale( self.scale )  *  Matrix4.rotate( self.rotation )  *  Matrix4.translate( self.position.toVector3() )
					glPushMatrix()
					glMultMatrix( matrix )
	
					model = None
					if self.model is not None:
						model = self.model.model
						
					if model is not None:
						model.initGL()
						_beginSolidRendering()
						_setupTransparentMaterial()
						model.drawGL()
						_endSolidRendering()
	
					glPopMatrix()
	
					glDisable( GL_CULL_FACE )
			else:
				if paintLayer == PaintLayer.PAINTLAYER_OBJECTS:
					glCullFace( GL_BACK )
					glEnable( GL_CULL_FACE )
	
					matrix = Matrix4.scale( self.scale )  *  Matrix4.rotate( self.rotation )  *  Matrix4.translate( self.position.toVector3() )
					glPushMatrix()
					glMultMatrix( matrix )
	
					model = None
					if self.model is not None:
						model = self.model.model
						
					if model is not None:
						model.initGL()
						_beginSolidRendering()
						_setupSolidMaterial()
						model.drawGL()
						_endSolidRendering()
	
					glPopMatrix()
	
					glDisable( GL_CULL_FACE )


	def templateDrawingPoint3d(self, ray, bBackfaceCulling):
		"""Template drawing point - 3D
		@ray - the ray to raytrace 		[Segment3d]
		@bBackfaceCulling - cull back faces
		Returns: a tuple of:
			bSuccess		 [bool]						Successful; did we hit anything?
			t			[float] (or None if no hit)			The intersection parameter
			intersection	[Point3] (or None if no hit)			The intersection point
			clippedRay	[Segment3] (or None if no hit)		The clipped ray"""
		if self.bVisible:
			model = None
			if self.model is not None:
				model = self.model.model
			
				if model is not None:
					modelToWorldMatrix = Matrix4.scale( self.scale )  *  Matrix4.rotate( self.rotation )  *  Matrix4.translate( self.position.toVector3() )
					worldToModelMatrix = modelToWorldMatrix.inverse()
					modelSpaceRay = ray * worldToModelMatrix
					bSuccess, triIndex, intersection, t = model.raytrace( Segment3f( modelSpaceRay ) )
					if bSuccess:
						intersection = intersection.toPoint3() * modelToWorldMatrix
						return True, t, intersection, Segment3( ray.a * modelToWorldMatrix, intersection )
		
		return False, None, None, None
	
	
	def _onModelPathChanged(self):
		self.modelPathChangedSignal.emit( self )




class BackgroundModelList (Viewport3dPainter, DrawingTemplate):
	trackerClass = BackgroundModelCommandTracker.BackgroundModelListCommandTracker

	modelPathChangedSignal = ClassSignal()
	modelAddedSignal = ClassSignal()
	modelInsertedSignal = ClassSignal()
	modelRemoveSignal = ClassSignal()
	modelRemovedSignal = ClassSignal()

	def __init__(self):
		self._models = []
		self._viewports = []

		self._commandTracker_ = None


	def newModel(self):
		model = BackgroundModel()
		self.appendModel( model )
		return model


	def appendModel(self, model):
		model.modelPathChangedSignal.connect( self._onModelPathChanged )
		model.cellModifiedSignal.connect( self._onModelModified )
		self._models.append( model )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onModelListModelAppended( self, model )
		self.modelAddedSignal.emit( self, model )
		self._postRedraw()


	def insertModel(self, index, model):
		model.modelPathChangedSignal.connect( self._onModelPathChanged )
		model.cellModifiedSignal.connect( self._onModelModified )
		self._models.insert( index, model )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onModelListModelInserted( self, index, model )
		self.modelInsertedSignal.emit( self, index, model )
		self._postRedraw()


	def removeModel(self, model):
		assert model in self._models
		self.modelRemoveSignal.emit( self, model )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onModelListModelRemoved( self, model )
		model.cellModifiedSignal.disconnect( self._onModelModified )
		model.modelPathChangedSignal.disconnect( self._onModelPathChanged )
		self._models.remove( model )
		self.modelRemovedSignal.emit( self, model )
		self._postRedraw()


	def clear(self):
		while len( self._models ) > 0:
			self.removeModel( self._models[-1] )


	def evPaint3d(self, viewport, paintLayer):
		for model in self._models:
			model.paint3d( paintLayer )


	def templateDrawingPoint3d(self, ray, bBackfaceCulling):
		"""Template drawing point - 3D
		@ray - the ray to raytrace 		[Segment3d]
		@bBackfaceCulling - cull back faces
		Returns: a tuple of:
			bSuccess		 [bool]						Successful; did we hit anything?
			t			[float] (or None if no hit)			The intersection parameter
			intersection	[Point3] (or None if no hit)			The intersection point
			clippedRay	[Segment3] (or None if no hit)		The clipped ray"""
		bSuccess = False
		t = None
		intersection = None
		clippedRay = None
		
		for model in self._models:
			_bSuccess, _t, _intersection, _clippedRay = model.templateDrawingPoint3d( ray, bBackfaceCulling )
			if _bSuccess:
				bSuccess = True
				t = _t
				intersection = _intersection
				clippedRay = _clippedRay
				ray = _clippedRay

		return bSuccess, t, intersection, clippedRay
	
	
	def __getitem__(self, index):
		return self._models[index]

	def __len__(self):
		return len( self._models )

	def __iter__(self):
		return iter( self._models )

	def __contains__(self, model):
		return model in self._models

	def index(self, model):
		return self._models.index( model )



	def __readxml__(self, xmlNode):
		self.clear()
		for node in xmlNode.childrenNamed( 'model' ):
			model = self.newModel()
			node  >>  model

	def __writexml__(self, xmlNode):
		for model in self._models:
			xmlNode.addChild( 'model' )  <<  model



	def painterAttachedToViewport(self, viewport):
		super( BackgroundModelList, self ).painterAttachedToViewport( viewport )
		self._viewports.append( viewport )


	def painterDetachedFromViewport(self, viewport):
		super( BackgroundModelList, self ).painterDetachedFromViewport( viewport )
		self._viewports.remove( viewport )


	def _postRedraw(self):
		for viewport in self._viewports:
			viewport.postRedraw()


	def _onModelModified(self, model):
		self._postRedraw()

	def _onModelPathChanged(self, model):
		self.modelPathChangedSignal.emit( self, model )


ioObjectFactoryRegister( 'BackgroundModelList', BackgroundModelList )

