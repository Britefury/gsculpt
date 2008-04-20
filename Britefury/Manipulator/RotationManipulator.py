##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Vector3, Quaternion, Matrix4, Axes3

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Math.MathCellTypes import QuaternionCell

from Britefury.Manipulator.RotationHandle import RotationHandle
from Britefury.Manipulator.Manipulator import Manipulator




class RotationManipulator (Manipulator):
	startSignal = ClassSignal()
	manipulateSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self, snapping=None):
		super( RotationManipulator, self ).__init__()
		self._cell = None
		self._positionCell = None
		self._spaceRotationCell = None
		self._rotatedSpaceRotationCell = QuaternionCell( Quaternion() )
		self._rotatedSpaceRotationCell.function = self._p_computeRotatedSpaceRotation
		self._handle = RotationHandle( snapping )
		self._handle.spaceRotationCell = self._rotatedSpaceRotationCell
		self._initialValue = None

		self._o_setUISensitivity( False )


	def manipAttachToView(self, view):
		super( RotationManipulator, self ).manipAttachToView( view )
		if self._cell is not None:
			self._handle.attachToView( self._view )

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		if self._cell is not None:
			self._handle.detachFromView()
		super( RotationManipulator, self ).manipDetachFromView()


	def attachCell(self, cell):
		assert self._cell is None, 'already attached to a cell'
		self._cell = cell
		# Trigger a recompute of _rotatedSpaceRotationCell
		self._rotatedSpaceRotationCell.function = self._p_computeRotatedSpaceRotation

		self._o_watchCellValidity( self._cell )
		self._o_refreshSensitivity()

		self._handle.startSignal.connect( self._p_onStart )
		self._handle.rotateSignal.connect( self._p_onRotate )
		self._handle.finishSignal.connect( self._p_onFinish )

		if self._view is not None:
			self._handle.attachToView( self._view )

	def detachCell(self):
		assert self._cell is not None, 'not attached to a cell'

		# Trigger a recompute of _rotatedSpaceRotationCell
		self._rotatedSpaceRotationCell.function = self._p_computeRotatedSpaceRotation

		self._o_stopWatchingCellValidity( self._cell )

		self._handle.startSignal.disconnect( self._p_onStart )
		self._handle.rotateSignal.disconnect( self._p_onRotate )
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
			if not isinstance( self._initialValue, Quaternion ):
				self._initialValue = Quaternion()
			self._p_commandHistoryBreak()
			self.startSignal.emit( self, viewport )

	def _p_onRotate(self, handle, viewport, axis, angle):
		if self._cell is not None:
			if self._cell.bLiteral:
				value = self._initialValue * Quaternion.rotateAxis( axis, angle )
				if value != self._cell.literalValue:
					self._cell.literalValue = value
				self.manipulateSignal.emit( self, viewport )

	def _p_onFinish(self, handle, viewport):
		if self._cell is not None:
			self._initialValue = None
			self._p_commandHistoryBreak()
			self.finishSignal.emit( self, viewport )



	def _p_computeRotatedSpaceRotation(self):
		spaceRotation = Quaternion()
		value = Quaternion()
		if self._spaceRotationCell is not None  and  self._spaceRotationCell.isValid():
			spaceRotation = self._spaceRotationCell.getValue()
			if not isinstance( spaceRotation, Quaternion ):
				spaceRotation = Quaternion()
		if self._cell is not None  and  self._cell.isValid():
			value = self._cell.getValue()
			if not isinstance( value, Quaternion ):
				value = Quaternion()
		return spaceRotation * value




	def _p_getSnapping(self):
		return self._handle.snapping

	def _p_setSnapping(self, snapping):
		self._handle.snapping = snapping



	def _p_setPositionCell(self, positionCell):
		if self._positionCell is not None:
			self._o_watchCellValidity( self._positionCell )
		self._positionCell = positionCell
		if self._positionCell is not None:
			self._o_stopWatchingCellValidity( self._positionCell )
		self._handle.positionCell = positionCell


	def _p_setSpaceRotationCell(self, spaceRotationCell):
		self._spaceRotationCell = spaceRotationCell
		# Trigger a recompute of _rotatedSpaceRotationCell
		self._rotatedSpaceRotationCell.function = self._p_computeRotatedSpaceRotation


	snapping = property( _p_getSnapping, _p_setSnapping )
	positionCell = property( None, _p_setPositionCell )
	spaceRotationCell = property( None, _p_setSpaceRotationCell )

