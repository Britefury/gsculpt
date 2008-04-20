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



class Vector2OrPoint2CellEditEntry (BasicWidgetSingleCellEdit):
	"""Vector2 or Point2 cell editor - entry
	@x - the x widget
	@y - the y widget"""

	__valueclass__ = object

	def __init__(self, minValue, maxValue, step, page, numDigits):
		super( Vector2OrPoint2CellEditEntry, self ).__init__()
		self.x = gtk.SpinButton( None, climb_rate = 0.15, digits = numDigits )
		self.x.set_increments( step, page )
		self.x.set_range( minValue, maxValue )
		self.x.connect( 'value-changed', self._p_onXSpin )

		self.y = gtk.SpinButton( None, climb_rate = 0.15, digits = numDigits )
		self.y.set_increments( step, page )
		self.y.set_range( minValue, maxValue )
		self.y.connect( 'value-changed', self._p_onYSpin )

		self._bIgnoreSpin = False

		self._o_setWidgetSensitivity( False )


	def show(self):
		self.x.show()
		self.y.show()

	def hide(self):
		self.x.hide()
		self.y.hide()



	def _o_setWidgetSensitivity(self, bSensitive):
		self.x.set_sensitive( bSensitive )
		self.y.set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		if isinstance( value, self.__valueclass__ ):
			self._bIgnoreSpin = True
			self.x.set_value( value.x )
			self.y.set_value( value.y )
			self._bIgnoreSpin = False


	def _p_onXSpin(self, spinButton):
		if not self._bIgnoreSpin:
			assert self._cell is not None
			if self._cell.bLiteral:
				value = self._cell.value
				value.x = self.x.get_value()
				if value != self._cell.literalValue:
					self._o_blockCell( value )
					self._cell.literalValue = value
					self._o_unblockCell()

	def _p_onYSpin(self, spinButton):
		if not self._bIgnoreSpin:
			assert self._cell is not None
			if self._cell.bLiteral:
				value = self._cell.value
				value.y = self.y.get_value()
				if value != self._cell.literalValue:
					self._o_blockCell( value )
					self._cell.literalValue = value
					self._o_unblockCell()






class Vector3OrPoint3CellEditEntry (BasicWidgetSingleCellEdit):
	"""Vector3 or Point3 cell editor - entry
	@x - the x widget
	@y - the y widget
	@z - the z widget"""

	__valueclass__ = object

	def __init__(self, minValue, maxValue, step, page, numDigits):
		super( Vector3OrPoint3CellEditEntry, self ).__init__()
		self.x = gtk.SpinButton( None, climb_rate = 0.15, digits = numDigits )
		self.x.set_increments( step, page )
		self.x.set_range( minValue, maxValue )
		self.x.connect( 'value-changed', self._p_onXSpin )

		self.y = gtk.SpinButton( None, climb_rate = 0.15, digits = numDigits )
		self.y.set_increments( step, page )
		self.y.set_range( minValue, maxValue )
		self.y.connect( 'value-changed', self._p_onYSpin )

		self.z = gtk.SpinButton( None, climb_rate = 0.15, digits = numDigits )
		self.z.set_increments( step, page )
		self.z.set_range( minValue, maxValue )
		self.z.connect( 'value-changed', self._p_onZSpin )

		self._bIgnoreSpin = False

		self._o_setWidgetSensitivity( False )


	def show(self):
		self.x.show()
		self.y.show()
		self.z.show()

	def hide(self):
		self.x.hide()
		self.y.hide()
		self.z.hide()



	def _o_setWidgetSensitivity(self, bSensitive):
		self.x.set_sensitive( bSensitive )
		self.y.set_sensitive( bSensitive )
		self.z.set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		if isinstance( value, self.__valueclass__ ):
			self._bIgnoreSpin = True
			self.x.set_value( value.x )
			self.y.set_value( value.y )
			self.z.set_value( value.z )
			self._bIgnoreSpin = False


	def _p_onXSpin(self, spinButton):
		if not self._bIgnoreSpin:
			assert self._cell is not None
			if self._cell.bLiteral:
				value = self._cell.value
				value.x = self.x.get_value()
				if value != self._cell.literalValue:
					self._o_blockCell( value )
					self._cell.literalValue = value
					self._o_unblockCell()

	def _p_onYSpin(self, spinButton):
		if not self._bIgnoreSpin:
			assert self._cell is not None
			if self._cell.bLiteral:
				value = self._cell.value
				value.y = self.y.get_value()
				if value != self._cell.literalValue:
					self._o_blockCell( value )
					self._cell.literalValue = value
					self._o_unblockCell()

	def _p_onZSpin(self, spinButton):
		if not self._bIgnoreSpin:
			assert self._cell is not None
			if self._cell.bLiteral:
				value = self._cell.value
				value.z = self.z.get_value()
				if value != self._cell.literalValue:
					self._o_blockCell( value )
					self._cell.literalValue = value
					self._o_unblockCell()


