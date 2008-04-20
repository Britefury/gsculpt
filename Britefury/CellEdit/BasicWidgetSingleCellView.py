##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import pygtk
pygtk.require( '2.0' )
import gtk


from Britefury.Event.QueuedEvent import queueEvent

from Britefury.CellEdit.WidgetSingleCellEdit import WidgetSingleCellEdit



class BasicWidgetSingleCellView (WidgetSingleCellEdit):
	"""Basic widget based single cell editor"""


	__valueclass__ = None


	def attachCell(self, cell):
		super( BasicWidgetSingleCellView, self ).attachCell( cell )
		self._cell.changedSignal.connect( self._p_onCellChanged )
		self._o_refreshSensitivity()
		self._p_refreshFromCellValue()

	def detachCell(self):
		assert self._cell is not None, 'no cell attached'
		self._cell.changedSignal.disconnect( self._p_onCellChanged )
		super( BasicWidgetSingleCellView, self ).detachCell()


	def _o_setUISensitivity(self, bSensitive):
		self._o_setWidgetSensitivity( bSensitive )


	def _o_setWidgetSensitivity(self, bSensitive):
		assert False, 'abstract'


	def _o_setWidgetValue(self, value):
		assert False, 'abstract'


	def _p_onCellChanged(self):
		queueEvent( self._p_refreshFromCellValue )


	def _p_refreshFromCellValue(self):
		value = self._cell.getValue()
		if isinstance( value, self.__valueclass__ ):
			self._o_setWidgetValue( value )
