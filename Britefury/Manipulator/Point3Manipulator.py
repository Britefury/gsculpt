##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Math.Math import Point3

from Britefury.Manipulator.TranslationHandle3d import Translation3Handle3d

from Britefury.Manipulator.Manipulator import Manipulator




class Point3Manipulator (Manipulator):
	startSignal = ClassSignal()
	manipulateSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self, snapping=None):
		super( Point3Manipulator, self ).__init__()
		self._cell = None
		self._handle = Translation3Handle3d()
		self._initialValue = None
		self.snapping = snapping

		self._o_setUISensitivity( False )


	def manipAttachToView(self, view):
		super( Point3Manipulator, self ).manipAttachToView( view )
		if self._cell is not None:
			self._handle.attachToView( self._view )

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		if self._cell is not None:
			self._handle.detachFromView()
		super( Point3Manipulator, self ).manipDetachFromView()


	def attachCell(self, cell):
		assert self._cell is None, 'already attached to a cell'
		self._cell = cell
		self._handle.positionCell = cell

		self._o_watchCellValidity( self._cell )
		self._o_refreshSensitivity()

		self._handle.startSignal.connect( self._p_onStart )
		self._handle.translateSignal.connect( self._p_onTranslate )
		self._handle.finishSignal.connect( self._p_onFinish )

		if self._view is not None:
			self._handle.attachToView( self._view )

		if self._cell.isValid():
			self._p_onCell( self._cell.value )

	def detachCell(self):
		assert self._cell is not None, 'not attached to a cell'
		self._handle.positionCell = None

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
		if self._cell is not None:
			return self._cell.isValid()
		else:
			return False


	def _p_onStart(self, handle, viewport):
		if self._cell is not None:
			self._initialValue = self._cell.value
			if not isinstance( self._initialValue, Point3 ):
				self._initialValue = Point3()
			self._p_commandHistoryBreak()
			self.startSignal.emit( self, viewport )

	def _p_onTranslate(self, handle, viewport, translation, constraints):
		if self._cell is not None:
			if self._cell.bLiteral:
				value = self._initialValue + translation
				if self.snapping is not None:
					value = self.snapping.snapPoint3( value, constraints, viewport.pointerStatus.bControlKey, viewport.pointerStatus.bShiftKey )
				if value != self._cell.literalValue:
					self._cell.literalValue = value
				self.manipulateSignal.emit( self, viewport )

	def _p_onFinish(self, handle, viewport):
		if self._cell is not None:
			self._initialValue = None
			self._p_commandHistoryBreak()
			self.finishSignal.emit( self, viewport )

