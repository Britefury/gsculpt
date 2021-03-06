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



class BasicWidgetSingleCellEdit (WidgetSingleCellEdit):
	"""Basic widget based single cell editor"""


	__valueclass__ = None


	def __init__(self):
		super( BasicWidgetSingleCellEdit, self ).__init__()
		self._bLiteral = False
		self._bCellBlocked = False
		self._ignoreCellValue = None


	def attachCell(self, cell):
		super( BasicWidgetSingleCellEdit, self ).attachCell( cell )
		self._cell.evaluatorSignal.connect( self._p_onCellEvaluator )
		self._cell.changedSignal.connect( self._p_onCellChanged )
		if self._cell.isValid():
			self._p_onCellEvaluator( None, self._cell.evaluator )
		self._o_refreshSensitivity()

	def detachCell(self):
		assert self._cell is not None, 'no cell attached'
		self._cell.evaluatorSignal.disconnect( self._p_onCellEvaluator )
		self._cell.changedSignal.disconnect( self._p_onCellChanged )
		self._bLiteral = False
		super( BasicWidgetSingleCellEdit, self ).detachCell()


	def _o_setUISensitivity(self, bSensitive):
		self._o_setWidgetSensitivity( bSensitive  and  self._bLiteral )


	def _o_setWidgetSensitivity(self, bSensitive):
		assert False, 'abstract'


	def _o_setWidgetValue(self, value):
		assert False, 'abstract'


	def _p_onCellEvaluator(self, oldEval, evaluator):
		if self._cell.isLiteral()  and  isinstance( evaluator, self.__valueclass__ ):
			self._bLiteral = True
			if not self._bCellBlocked  or  evaluator != self._ignoreCellValue:
				self._o_setWidgetValue( evaluator )
		else:
			self._bLiteral = False
			queueEvent( self._p_refreshFromCellValue )
		self._o_refreshSensitivity()

	def _p_onCellChanged(self):
		if not self._bLiteral:
			queueEvent( self._p_refreshFromCellValue )


	def _p_refreshFromCellValue(self):
		if self._cell is not None:
			value = self._cell.getValue()
			if isinstance( value, self.__valueclass__ ):
				if not self._bCellBlocked  or  value != self._ignoreCellValue:
					self._o_setWidgetValue( value )


	def _o_blockCell(self, ignoreValue):
		self._bCellBlocked = True
		self._ignoreCellValue = ignoreValue

	def _o_unblockCell(self):
		self._bCellBlocked = False
		self._ignoreCellValue = None
