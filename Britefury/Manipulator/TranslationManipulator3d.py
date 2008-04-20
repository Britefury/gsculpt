##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Math.Math import Point3,Vector2, Vector3, Matrix4, Quaternion

from Britefury.Math.MathCellTypes import Point3Cell

from Britefury.Cell.CellInterface import *

from Britefury.Manipulator.TranslationHandle3d import Translation2Handle3d, Translation3Handle3d

from Britefury.Manipulator.Manipulator import Manipulator





class _TranslationManipulator3d (Manipulator):
	startSignal = ClassSignal()
	manipulateSignal = ClassSignal()
	finishSignal = ClassSignal()



	__valuetype__ = None
	__handletype__ = None



	def __init__(self, snapping=None):
		super( _TranslationManipulator3d, self ).__init__()
		self._cell = None
		self._positionCell = None
		self._translatedPositionCell = Point3Cell( Point3() )
		self._translatedPositionCell.function = self._p_computeTranslatedPosition
		self._spaceRotationCell = None
		self._handle = self.__handletype__()
		self._handle.positionCell = self._translatedPositionCell
		self._initialValue = None
		self.snapping = snapping

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		super( _TranslationManipulator3d, self ).manipAttachToView( view )
		if self._cell is not None:
			self._handle.attachToView( self._view )

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		if self._cell is not None:
			self._handle.detachFromView()
		super( _TranslationManipulator3d, self ).manipDetachFromView()


	def attachCell(self, cell):
		assert self._cell is None, 'already attached to a cell'
		self._cell = cell
		# Trigger a recompute of _translatedPositionCell
		self._translatedPositionCell.function = self._p_computeTranslatedPosition

		self._o_watchCellValidity( self._cell )
		self._o_refreshSensitivity()

		self._handle.startSignal.connect( self._p_onStart )
		self._handle.translateSignal.connect( self._p_onTranslate )
		self._handle.finishSignal.connect( self._p_onFinish )

		if self._view is not None:
			self._handle.attachToView( self._view )

	def detachCell(self):
		assert self._cell is not None, 'not attached to a cell'

		# Trigger a recompute of _translatedPositionCell
		self._translatedPositionCell.function = self._p_computeTranslatedPosition

		self._o_stopWatchingCellValidity( self._cell )

		self._handle.startSignal.disconnect( self._p_onStart )
		self._handle.translateSignal.disconnect( self._p_onTranslate )
		self._handle.finishSignal.disconnect( self._p_onFinish )

		if self._view is not None:
			self._handle.detachFromView()
		self._cell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._handle.setSensitive( bSensitive )


	def _o_isValid(self):
		if self._cell is not None  and  self._positionCell is not None:
			return self._cell.isValid()  and  self._positionCell.isValid()
		else:
			return False


	def _p_onStart(self, handle, viewport):
		if self._cell is not None:
			self._initialValue = self._cell.value
			if not isinstance( self._initialValue, self.__valuetype__ ):
				self._initialValue = self.__valuetype__()
			self._p_commandHistoryBreak()
			self.startSignal.emit( self, viewport )

	def _p_onTranslate(self, handle, viewport, translation, constraints):
		if self._cell is not None:
			if self._cell.bLiteral:
				value = self._initialValue + translation
				if self.snapping is not None:
					if isinstance( self.snapping, CellInterface ):
						value = self._o_snapValue( self.snapping.getValue(), value, constraints, viewport )
					else:
						value = self._o_snapValue( self.snapping, value, constraints, viewport )
				if value != self._cell.literalValue:
					self._cell.literalValue = value
				self.manipulateSignal.emit( self, viewport )

	def _p_onFinish(self, handle, viewport):
		if self._cell is not None:
			self._initialValue = None
			self._p_commandHistoryBreak()
			self.finishSignal.emit( self, viewport )



	def _p_computeTranslatedPosition(self):
		position = Point3()
		value = Vector3()
		spaceRotation = Quaternion()
		if self._positionCell is not None  and  self._positionCell.isValid():
			position = self._positionCell.getValue()
			if not isinstance( position, Point3 ):
				position = Point3()
		if self._spaceRotationCell is not None  and  self._spaceRotationCell.isValid():
			spaceRotation = self._spaceRotationCell.getValue()
			if not isinstance( spaceRotation, Quaternion ):
				spaceRotation = Quaternion()
		if self._cell is not None  and  self._cell.isValid():
			value = self._cell.getValue()
			if isinstance( value, Vector2 ):
				value = Vector3( value )
			if not isinstance( value, Vector3 ):
				value = Vector3()
		return position  +  value * Matrix4.rotate( spaceRotation )




	def _o_snapValue(self, snapping, value, constraints, viewport):
		assert False, 'abstract'


	def _p_setPositionCell(self, positionCell):
		if self._positionCell is not None:
			self._o_watchCellValidity( self._positionCell )
		self._positionCell = positionCell
		if self._positionCell is not None:
			self._o_stopWatchingCellValidity( self._positionCell )
		# Trigger a recompute of _translatedPositionCell
		self._translatedPositionCell.function = self._p_computeTranslatedPosition


	def _p_setSpaceRotationCell(self, spaceRotationCell):
		self._spaceRotationCell = spaceRotationCell
		self._handle.spaceRotationCell = spaceRotationCell


	positionCell = property( None, _p_setPositionCell )
	spaceRotationCell = property( None, _p_setSpaceRotationCell )





class Translation2Manipulator3d (_TranslationManipulator3d):
	__valuetype__ = Vector2
	__handletype__ = Translation2Handle3d



	def _o_snapValue(self, snapping, value, constraints, viewport):
		return snapping.snapVector2( value, constraints, viewport.pointerStatus.bControlKey, viewport.pointerStatus.bShiftKey )





class Translation3Manipulator3d (_TranslationManipulator3d):
	__valuetype__ = Vector3
	__handletype__ = Translation3Handle3d



	def _o_snapValue(self, snapping, value, constraints, viewport):
		return snapping.snapVector3( value, constraints, viewport.pointerStatus.bControlKey, viewport.pointerStatus.bShiftKey )
