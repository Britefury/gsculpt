##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.BasicWidgetSingleCellEdit import BasicWidgetSingleCellEdit
from Britefury.CellEdit.CellEditFunctionButton import CellEditFunctionButton



class BasicWidgetSingleCellEditWithFn (BasicWidgetSingleCellEdit):
	"""Basic widget based single cell editor with cell function button"""


	__valueclass__ = None


	def __init__(self):
		super( BasicWidgetSingleCellEditWithFn, self ).__init__()
		self._functionButton =CellEditFunctionButton()
		self._functionButton.show()


	def attachCell(self, cell):
		super( BasicWidgetSingleCellEditWithFn, self ).attachCell( cell )
		self._functionButton.attachCell( cell )

	def detachCell(self):
		self._functionButton.detachCell()
		super( BasicWidgetSingleCellEditWithFn, self ).detachCell()
