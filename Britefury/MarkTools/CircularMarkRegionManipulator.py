##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from OpenGL.GL import *

from copy import copy

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Math.Math import BBox2

from Britefury.Graphics.Graphics import Region3d, CircleRegion2d, ProjectedRegion3d

from Britefury.WorkArea.Viewport import ViewportEventHandler
from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter

from Britefury.View.View import PaintLayer

from Britefury.Model import ModelDraw

from Britefury.ProceduralTool.GSProcedureTool import *

from Britefury.MarkTools.MarkRegionManipulator import MarkRegionManipulator




class CircularMarkRegionManipulator (MarkRegionManipulator, ViewportEventHandler, Viewport3dPainter):
	startSignal = ClassSignal()
	manipulateSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self):
		super( CircularMarkRegionManipulator, self ).__init__()
		self._regionViewport = None

		self._p1 = None
		self._p2 = None
		self._bButtonDown = False


	def manipAttachToView(self, view):
		super( CircularMarkRegionManipulator, self ).manipAttachToView( view )
		self._view.addEventHandler( self, Viewport3d )
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		super( CircularMarkRegionManipulator, self ).manipDetachFromView()



	def evPrimaryButtonDown(self, viewport):
		super( CircularMarkRegionManipulator, self ).evPrimaryButtonDown( viewport )

		# Emit start signal
		self.startSignal.emit( viewport )

		# Handle key status
		self._o_handleKeyStatus( viewport )

		# Initialise rectangle
		self._p1 = copy( viewport.pointerStatus.positionF )
		self._p2 = copy( self._p1 )
		self._regionViewport = viewport

		# Button pressed
		self._bButtonDown = True

		# Create region
		self._o_createNewRegion( viewport )


	def evMotion(self, viewport, point):
		super( CircularMarkRegionManipulator, self ).evMotion( viewport, point )

		# Refresh the region
		if self._bButtonDown:
			self._p2 = copy( viewport.pointerStatus.positionF )

			if self._regionCell is not None:
				cam = viewport.camera
				circle = Circle2( self._p1, self._p1.distanceTo( self._p2 ) )
				region2d = CircleRegion2d( circle )
				region3d = ProjectedRegion3d( region2d, cam.projection )
				self._regionCell.literalValue = region3d

			self.manipulateSignal.emit( viewport )

			self._view.forcePendingRedraw()


	def evPrimaryButtonUp(self, viewport):
		super( CircularMarkRegionManipulator, self ).evPrimaryButtonUp( viewport )

		# Button released
		self._bButtonDown = False

		# Finished
		self._p_commandHistoryBreak()
		self._regionViewport = None
		self.finishSignal.emit()

		# Redraw
		self._view.postRedraw()



	def evPaint3d(self, viewport, paintLayer):
		# Paint the region
		self._o_paintRegion( viewport, paintLayer )


		if paintLayer == PaintLayer.PAINTLAYER_OVERLAY_BLEND_NODEPTH:
			if viewport is self._regionViewport:
				glDepthMask( False )

				glMatrixMode( GL_PROJECTION )
				glPushMatrix()
				viewport.glProjectionIdentity()

				glMatrixMode( GL_MODELVIEW )
				glPushMatrix()
				viewport.glModelViewIdentity()

				circle = Circle2( self._p1, self._p1.distanceTo( self._p2 ) )
				ModelDraw.drawMarkCircle( circle )

				glPopMatrix()
				glMatrixMode( GL_PROJECTION )
				glPopMatrix()
				glMatrixMode( GL_MODELVIEW )

				glDepthMask( True )





class CircularMarkRegionFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, predicateField, regionField, backfaceCullField, bEnabledField):
		super( CircularMarkRegionFieldManipulator, self ).__init__()
		self._predicateField = predicateField
		self._regionField = regionField
		self._backfaceCullField = backfaceCullField
		self._bEnabledField = bEnabledField


	def createElement(self, sheetEditContext):
		return CircularMarkRegionManipulator()

	def initialiseElement(self, element, sheetEditContext):
		if self._predicateField is not None:
			element.attachPredicateCell( predicate = self._p_getCell( self._predicateField, sheetEditContext ) )
		if self._regionField is not None:
			element.attachRegionCell( self._p_getCell( self._regionField, sheetEditContext ) )
		if self._backfaceCullField is not None:
			element.attachBackfaceCullCell( self._p_getCell( self._backfaceCullField, sheetEditContext ) )
		if self._bEnabledField is not None:
			element.attachEnabledCell( self._p_getCell( self._bEnabledField, sheetEditContext ) )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		element.detachCells()
		element.commandHistory = None


	def installElementEventHandlers(self, element, sheetEditContext):
		element.manipAttachToView( sheetEditContext.view )

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		element.manipDetachFromView()
