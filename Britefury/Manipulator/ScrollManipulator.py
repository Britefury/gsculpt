##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Util.SignalSlot import ClassSignal

from Britefury.WorkArea.Viewport import Viewport, ViewportScrollListener
from Britefury.WorkArea.Viewport3d import Viewport3d

from Britefury.Manipulator.Manipulator import Manipulator





class ScrollManipulator (Manipulator, ViewportScrollListener):
	manipulateSignal = ClassSignal()

	def __init__(self, lower, upper, rate):
		super( ScrollManipulator, self ).__init__()
		self._view = None
		self._cell = None
		self.lower = lower
		self.upper = upper
		self.rate = rate
		self._bScrollManipulatorSensitive = True

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		assert self._view is None, 'already attached to a view'
		self._view = view
		if self._cell is not None:
			self._view.addScrollListener( self, Viewport )

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		if self._cell is not None:
			self._view.removeScrollListener( self, Viewport )
		self._view = None


	def attachCell(self, cell):
		assert self._cell is None,  'variable already attached'
		self._cell = cell

		self._o_watchCellValidity( self._cell )
		self._o_refreshSensitivity()

		if self._view is not None:
			self._view.addScrollListener( self, Viewport )

	def detachCell(self):
		assert self._cell is not None,  'no variable attached'

		self._o_stopWatchingCellValidity( self._cell )

		if self._view is not None:
			self._view.removeScrollListener( self, Viewport )
		self._cell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._bScrollManipulatorSensitive = bSensitive

	def _o_isValid(self):
		if self._cell is not None:
			return self._cell.isValid()
		else:
			return False


	def evScroll(self, viewport, x, y):
		if self._cell is not None  and  self._bScrollManipulatorSensitive:
			value = self._cell.value  +  y * self.rate
			value = min( value, self.upper )
			value = max( value, self.lower )
			self._cell.literalValue = value
			self.manipulateSignal.emit( self, viewport )

