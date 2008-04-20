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



class NumericCellEditSlider (BasicWidgetSingleCellEdit):
	"""Numeric cell editor - slider
	@slider - the entry widget"""

	__valueclass__ = object

	def __init__(self, minValue, maxValue, step, page):
		super( NumericCellEditSlider, self ).__init__()
		self.slider = gtk.HScale()
		self.slider.set_increments( step, page )
		self.slider.set_range( minValue, maxValue )
		self.slider.set_draw_value( False )
		self.slider.set_size_request( 125, -1 )
		self.slider.connect( 'value-changed', self._p_onSlider )
		self._bIgnoreSlider = False
		self._o_setWidgetSensitivity( False )


	def show(self):
		self.slider.show()

	def hide(self):
		self.slider.hide()


	def _o_setWidgetSensitivity(self, bSensitive):
		self.slider.set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		if isinstance( value, self.__valueclass__ ):
			self._bIgnoreSlider = True
			self.slider.set_value( value )
			self._bIgnoreSlider = False


	def _p_onSlider(self, scale):
		if not self._bIgnoreSlider:
			assert self._cell is not None
			value = self._o_convertValue( self.slider.get_value() )
			if self._cell.bLiteral  and  value != self._cell.literalValue:
				self._o_blockCell( value )
				self._cell.literalValue = value
				self._o_unblockCell()


	@staticmethod
	def _o_convertValue(value):
		return value


