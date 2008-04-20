##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.WidgetCellEdit import WidgetCellEdit


class WidgetSingleCellEdit (WidgetCellEdit):
	def __init__(self):
		super( WidgetSingleCellEdit, self ).__init__()
		self._cell = None


	def attachCell(self, cell):
		assert self._cell is None, 'cell already attached'
		self._cell = cell
		self._o_watchCellValidity( self._cell )
		self._o_refreshSensitivity()

	def detachCell(self):
		assert self._cell is not None, 'no cell attached'
		self._o_stopWatchingCellValidity( self._cell )
		self._cell = None
		self._o_refreshSensitivity()


	def _o_isValid(self):
		if self._cell is not None:
			return self._cell.isValid()
		else:
			return False
