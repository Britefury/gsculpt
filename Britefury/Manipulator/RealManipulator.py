##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Math.Math import Point3

from Britefury.WorkArea.Viewport3d import Viewport3d

from Britefury.Cell.CellInterface import *

from Britefury.Manipulator.ScreenSpaceHandle import ScreenSpace1Handle, ScreenSpace2Handle

from Britefury.Manipulator.Manipulator import Manipulator




class RealManipulator (Manipulator):
	class Scale (object):
		def scale(self, viewport, x):
			assert False, 'abstract'


	class FixedScale (Scale):
		def __init__(self, scaleFactor = 1.0):
			self.scaleFactor = scaleFactor

		def scale(self, viewport, x):
			return x * self.scaleFactor


	class FocalDistanceScale (Scale):
		def scale(self, viewport, x):
			assert isinstance( viewport, Viewport3d ), 'FocalDistanceScale does not work unless the viewport is a Viewport3d'
			return viewport.camera.transformDistanceAcrossScreenToDistanceThruWorld( x )


	class WorldSpaceScale (Scale):
		def __init__(self, focalPointCell):
			"""Constructor
			@focalPointCell - a cell that evaluates to the focal point (a Point3)"""
			self.focalPointCell = focalPointCell

		def scale(self, viewport, x):
			assert isinstance( viewport, Viewport3d ), 'WorldSpaceScale does not work unless the viewport is a Viewport3d'
			if self.focalPointCell is not None:
				focalPoint = self.focalPointCell.getValue()

				if isinstance( focalPoint, Point3 ):
					cam = viewport.camera
					pointDist = cam.transformWorldToCameraZ( focalPoint )
					return cam.transformDistanceAcrossScreenToDistanceThruWorld( x, pointDist )
				else:
					return viewport.camera.transformDistanceAcrossScreenToDistanceThruWorld( x )

			else:
				return viewport.camera.transformDistanceAcrossScreenToDistanceThruWorld( x )


	def _o_getScale(self, scale):
		if isinstance( scale, CellInterface ):
			return scale.getValue()
		else:
			return scale





class Real1Manipulator (RealManipulator):
	startSignal = ClassSignal()
	manipulateSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self, lower, upper, scale, snapping=None):
		super( Real1Manipulator, self ).__init__()
		self._view = None
		self._handle = ScreenSpace1Handle()
		self._handle.startSignal.connect( self._p_onStart )
		self._handle.dragSignal.connect( self._p_onDrag )
		self._handle.finishSignal.connect( self._p_onFinish )
		self._cell = None
		self.lower = lower
		self.upper = upper
		self.scale = scale
		self._initial = 0.0
		self.snapping = snapping

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		assert self._view is None, 'already attached to a view'
		self._view = view
		if self._cell is not None:
			self._handle.attachToView( self._view )

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		if self._cell is not None:
			self._handle.detachFromView()
		self._view = None


	def attachCell(self, cell):
		assert self._cell is None,  'cell already attached'
		self._cell = cell

		self._o_watchCellValidity( self._cell )
		self._o_refreshSensitivity()

		if self._view is not None:
			self._handle.attachToView( self._view )

	def detachCell(self):
		assert self._cell is not None,  'no cell attached'

		self._o_stopWatchingCellValidity( self._cell )

		if self._view is not None:
			self._handle.detachFromView()
		self._cell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._handle.setSensitive( bSensitive )

	def _o_isValid(self):
		if self._cell is not None:
			return self._cell.isValid()
		else:
			return False


	def _p_onStart(self, handle, viewport):
		if self._cell is not None:
			self._initial = self._cell.value
			if not isinstance( self._initial, float ):
				self._initial = 0.0
			self._p_commandHistoryBreak()
			self.startSignal.emit( self, viewport )

	def _p_onDrag(self, handle, viewport, delta):
		if self._cell is not None:
			if self._cell.isLiteral():
				value = self._initial  +  self._o_getScale( self.scale ).scale( viewport, delta )
				value = min( max( value, self.lower ), self.upper )
				if self.snapping is not None:
					if isinstance( self.snapping, CellInterface ):
						value = self.snapping.getValue().snap( value, viewport.pointerStatus.bControlKey, viewport.pointerStatus.bShiftKey )
					else:
						value = self.snapping.snap( value, viewport.pointerStatus.bControlKey, viewport.pointerStatus.bShiftKey )
				if value != self._cell.literalValue:
					self._cell.literalValue = value
				self.manipulateSignal.emit( self, viewport )

	def _p_onFinish(self, handle, viewport):
		if self._cell is not None:
			self._initial = None
			self._p_commandHistoryBreak()
			self.finishSignal.emit( self, viewport )






class Real2Manipulator (RealManipulator):
	startSignal = ClassSignal()
	manipulateSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self, xLower, xUpper, xScale, yLower, yUpper, yScale, xSnapping=None, ySnapping=None):
		super( Real2Manipulator, self ).__init__()
		self._view = None
		self._handle = ScreenSpace2Handle()
		self._handle.startSignal.connect( self._p_onStart )
		self._handle.dragSignal.connect( self._p_onDrag )
		self._handle.finishSignal.connect( self._p_onFinish )
		self._xCell = None
		self._yCell = None
		self.xLower = xLower
		self.xUpper = xUpper
		self.xScale = xScale
		self.yLower = yLower
		self.yUpper = yUpper
		self.yScale = yScale
		self._xInitial = 0.0
		self._yInitial = 0.0
		self.xSnapping = xSnapping
		self.ySnapping = ySnapping

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		assert self._view is None, 'already attached to a view'
		self._view = view
		self._handle.attachToView( self._view )

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		self._handle.detachFromView()
		self._view = None


	def attachCells(self, xCell, yCell):
		assert self._xCell is None  and  self._yCell is None,  'already attached to a cells'
		self._xCell = xCell
		self._yCell = yCell

		self._o_watchCellValidity( self._xCell )
		self._o_watchCellValidity( self._yCell )
		self._o_refreshSensitivity()

		if self._view is not None:
			self._handle.attachToView( self._view )

	def detachCells(self):
		assert self._xCell is not None  or  self._yCell is not None,  'not attached to cells'

		self._o_stopWatchingCellValidity( self._xCell )
		self._o_stopWatchingCellValidity( self._yCell )

		if self._view is not None:
			self._handle.detachFromView()
		self._xCell = None
		self._yCell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._handle.setSensitive( bSensitive )

	def _o_isValid(self):
		if self._xCell is not None  and  self._yCell is not None:
			return self._xCell.isValid()  and  self._yCell.isValid()
		else:
			return False


	def _p_onStart(self, handle, viewport):
		if self._xCell is not None:
			self._xInitial = self._xCell.value
			if not isinstance( self._xInitial, float ):
				self._xInitial = 0.0
		if self._yCell is not None:
			self._yInitial = self._yCell.value
			if not isinstance( self._yInitial, float ):
				self._yInitial = 0.0
		if self._xCell is not None  or  self._yCell is not None:
			self._p_commandHistoryBreak()
			self.startSignal.emit( self, viewport )


	def _p_onDrag(self, handle, viewport, deltaX, deltaY, constraintsX, constraintsY):
		if self._xCell is not None:
			if self._xCell.bLiteral:
				value = self._xInitial  +  self._o_getScale( self.xScale ).scale( viewport, deltaX )
				value = min( max( value, self.xLower ), self.xUpper );
				if self.xSnapping is not None  and  constraintsX != 0.0:
					if isinstance( self.xSnapping, CellInterface ):
						value = self.xSnapping.getValue().snap( value, viewport.pointerStatus.bControlKey, viewport.pointerStatus.bShiftKey )
					else:
						value = self.xSnapping.snap( value, viewport.pointerStatus.bControlKey, viewport.pointerStatus.bShiftKey )
				if value != self._xCell.literalValue:
					self._xCell.literalValue = value

		if self._yCell is not None:
			if self._yCell.bLiteral:
				value = self._yInitial  +  self._o_getScale( self.yScale ).scale( viewport, deltaY )
				value = min( max( value, self.yLower ), self.yUpper );
				if self.ySnapping is not None  and  constraintsY != 0.0:
					if isinstance( self.ySnapping, CellInterface ):
						value = self.ySnapping.getValue().snap( value, viewport.pointerStatus.bControlKey, viewport.pointerStatus.bShiftKey )
					else:
						value = self.ySnapping.snap( value, viewport.pointerStatus.bControlKey, viewport.pointerStatus.bShiftKey )
				if value != self._yCell.literalValue:
					self._yCell.literalValue = value

		if self._xCell is not None  or  self._yCell is not None:
			self.manipulateSignal.emit( self, viewport )

	def _p_onFinish(self, handle, viewport):
		self._xInitial = None
		self._yInitial = None
		if self._xCell is not None  or  self._yCell is not None:
			self._p_commandHistoryBreak()
			self.finishSignal.emit( self, viewport )


