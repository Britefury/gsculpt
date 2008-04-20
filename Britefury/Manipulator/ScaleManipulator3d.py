##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Vector2, Vector3

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Cell.CellInterface import *

from Britefury.Manipulator.ScaleHandle3d import Scale2Handle3d, Scale3Handle3d

from Britefury.Manipulator.Manipulator import Manipulator




class _ScaleManipulator3d (Manipulator):
	__valuetype__ = None
	__handletype__ = None


	startSignal = ClassSignal()
	manipulateSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self, snapping=None):
		super( _ScaleManipulator3d, self ).__init__()
		self._cell = None
		self._positionCell = None
		self._handle = self.__handletype__()
		self._initialValue = None
		self.snapping = snapping

		self._o_setUISensitivity( False )


	def manipAttachToView(self, view):
		super( _ScaleManipulator3d, self ).manipAttachToView( view )
		if self._cell is not None:
			self._handle.attachToView( self._view )

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		if self._cell is not None:
			self._handle.detachFromView()
		super( _ScaleManipulator3d, self ).manipDetachFromView()


	def attachCell(self, cell):
		assert self._cell is None, 'already attached to a cell'
		self._cell = cell

		self._o_watchCellValidity( self._cell )
		self._o_refreshSensitivity()

		self._handle.startSignal.connect( self._p_onStart )
		self._handle.scaleSignal.connect( self._p_onScale )
		self._handle.finishSignal.connect( self._p_onFinish )

		if self._view is not None:
			self._handle.attachToView( self._view )

	def detachCell(self):
		assert self._cell is not None, 'not attached to a cell'

		self._o_stopWatchingCellValidity( self._cell )

		self._handle.startSignal.disconnect( self._p_onStart )
		self._handle.scaleSignal.disconnect( self._p_onScale )
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
				self._initialValue = self._o_defaultInitialValue()
			self._p_commandHistoryBreak()
			self.startSignal.emit( self, viewport )

	def _p_onScale(self, handle, viewport, scale, constraints):
		if self._cell is not None:
			if self._cell.bLiteral:
				if self.snapping is not None:
					if isinstance( self.snapping, CellInterface ):
						scale = self._o_snapValue( self.snapping.getValue(), scale, constraints, viewport )
					else:
						scale = self._o_snapValue( self.snapping, scale, constraints, viewport )
				value = self._o_applyScale( self._initialValue, scale )
				if value != self._cell.literalValue:
					self._cell.literalValue = value
				self.manipulateSignal.emit( self, viewport )

	def _p_onFinish(self, handle, viewport):
		if self._cell is not None:
			self._initialValue = None
			self._p_commandHistoryBreak()
			self.finishSignal.emit( self, viewport )



	def _o_defaultInitialValue(self):
		assert False, 'abstract'

	def _o_applyScale(self, initial, scale):
		assert False, 'abstract'

	def _o_snapValue(self, snapping, scale, constraints, viewport):
		assert False, 'abstract'


	def _p_setPositionCell(self, positionCell):
		if self._positionCell is not None:
			self._o_watchCellValidity( self._positionCell )
		self._positionCell = positionCell
		if self._positionCell is not None:
			self._o_stopWatchingCellValidity( self._positionCell )
		self._handle.positionCell = positionCell

	def _p_setSpaceRotationCell(self, spaceRotationCell):
		self._handle.spaceRotationCell = spaceRotationCell



	positionCell = property( None, _p_setPositionCell )
	spaceRotationCell = property( None, _p_setSpaceRotationCell )







class Scale2Manipulator3d (_ScaleManipulator3d):
	__valuetype__ = Vector2
	__handletype__ = Scale2Handle3d


	def _o_defaultInitialValue(self):
		return Vector2( 1.0, 1.0 )

	def _o_applyScale(self, initial, scale):
		return Vector2( initial.x * scale.x, initial.y * scale.y )

	def _o_snapValue(self, snapping, scale, constraints, viewport):
		return snapping.snapVector2( scale, constraints, viewport.pointerStatus.bControlKey, viewport.pointerStatus.bShiftKey )





class Scale3Manipulator3d (_ScaleManipulator3d):
	__valuetype__ = Vector3
	__handletype__ = Scale3Handle3d


	def _o_defaultInitialValue(self):
		return Vector3( 1.0, 1.0, 1.0 )

	def _o_applyScale(self, initial, scale):
		return Vector3( initial.x * scale.x, initial.y * scale.y, initial.z * scale.z )

	def _o_snapValue(self, snapping, scale, constraints, viewport):
		return snapping.snapVector3( scale, constraints, viewport.pointerStatus.bControlKey, viewport.pointerStatus.bShiftKey )
