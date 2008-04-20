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

from Britefury.CellEdit.BasicWidgetSingleCellEdit import BasicWidgetSingleCellEdit



class NumericCellEditEntry (BasicWidgetSingleCellEdit):
	"""Numeric cell editor - entry
	@entry - the entry widget"""

	__valueclass__ = object

	def __init__(self, minValue, maxValue, step, page, numDigits):
		super( NumericCellEditEntry, self ).__init__()
		self.entry = gtk.SpinButton( None, climb_rate=0.15, digits=numDigits )
		self.entry.set_increments( step, page )
		self.entry.set_range( minValue, maxValue )
		self.entry.connect( 'value-changed', self._p_onSpin )
		self.entry.set_size_request( 60, -1 )
		self._bIgnoreSpin = False
		self._o_setWidgetSensitivity( False )


	def show(self):
		self.entry.show()

	def hide(self):
		self.entry.hide()


	def _o_setWidgetSensitivity(self, bSensitive):
		self.entry.set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		if isinstance( value, self.__valueclass__ ):
			self._bIgnoreSpin = True
			self.entry.set_value( value )
			self._bIgnoreSpin = False


	def _p_onSpin(self, spinButton):
		if not self._bIgnoreSpin:
			assert self._cell is not None
			value = self._o_convertValue( self.entry.get_value() )
			if self._cell.bLiteral  and  value != self._cell.literalValue:
				self._o_blockCell( value )
				self._cell.literalValue = value
				self._o_unblockCell()


	@staticmethod
	def _o_convertValue(value):
		return value


