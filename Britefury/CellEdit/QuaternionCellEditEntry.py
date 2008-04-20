##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import math

import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.Math.Math import Quaternion

from Britefury.Event.QueuedEvent import queueEvent

from Britefury.CellEdit.BasicWidgetSingleCellEdit import BasicWidgetSingleCellEdit




class QuaternionCellEditEntry (BasicWidgetSingleCellEdit):
	"""Vector2 or Point2 cell editor - entry
	@x - the x widget
	@y - the y widget
	@z - the z widget"""

	__valueclass__ = object

	def __init__(self):
		super( QuaternionCellEditEntry, self ).__init__()

		self.x = gtk.SpinButton( None, climb_rate = 0.15, digits = 4 )
		self.x.set_increments( 0.1, 10.0 )
		self.x.set_range( -180.0, 180.0 )
		self.x.connect( 'value-changed', self._p_onXSpin )

		self.y = gtk.SpinButton( None, climb_rate = 0.15, digits = 4 )
		self.y.set_increments( 0.1, 10.0 )
		self.y.set_range( -180.0, 180.0 )
		self.y.connect( 'value-changed', self._p_onYSpin )

		self.z = gtk.SpinButton( None, climb_rate = 0.15, digits = 4 )
		self.z.set_increments( 0.1, 10.0 )
		self.z.set_range( -180.0, 180.0 )
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
		if isinstance( value, Quaternion ):
			self._bIgnoreSpin = True
			euler = value.toEuler()
			self.x.set_value( math.degrees( -euler.x ) )
			self.y.set_value( math.degrees( -euler.y ) )
			self.z.set_value( math.degrees( -euler.z ) )
			self._bIgnoreSpin = False


	def _p_onXSpin(self, spinButton):
		if not self._bIgnoreSpin:
			assert self._cell is not None
			if self._cell.isLiteral()  and  self._cell.isValid():
				q = self._cell.getValue()
				if q is not None:
					euler = q.toEuler()
					euler.x = math.radians( -self.x.get_value() )
					value = Quaternion.rotateEuler( euler )
					if value != self._cell.literalValue:
						self._o_blockCell( value )
						self._cell.literalValue = value
						self._o_unblockCell()

	def _p_onYSpin(self, spinButton):
		if not self._bIgnoreSpin:
			assert self._cell is not None
			if self._cell.isLiteral()  and  self._cell.isValid():
				q = self._cell.getValue()
				if q is not None:
					euler = q.toEuler()
					euler.y = math.radians( -self.y.get_value() )
					value = Quaternion.rotateEuler( euler )
					if value != self._cell.literalValue:
						self._o_blockCell( value )
						self._cell.literalValue = value
						self._o_unblockCell()

	def _p_onZSpin(self, spinButton):
		if not self._bIgnoreSpin:
			assert self._cell is not None
			if self._cell.isLiteral()  and  self._cell.isValid():
				q = self._cell.getValue()
				if q is not None:
					euler = q.toEuler()
					euler.z = math.radians( -self.z.get_value() )
					value = Quaternion.rotateEuler( euler )
					if value != self._cell.literalValue:
						self._o_blockCell( value )
						self._cell.literalValue = value
						self._o_unblockCell()


