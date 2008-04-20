##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import pygtk
import gtk
import gtk.gdk

from Britefury.Math import Math
from Britefury.Math.Math import Point2i


class PointerStatus (object):
	s_buttonMasks = ( gtk.gdk.BUTTON1_MASK, gtk.gdk.BUTTON2_MASK, gtk.gdk.BUTTON3_MASK, gtk.gdk.BUTTON4_MASK, gtk.gdk.BUTTON5_MASK )

	"""Status of the pointer; position, button states, etc."""
	def __init__(self, position = Point2i(), bInsideBounds = False):
		"""Initialise the pointer status
		@position: position of the pointer
		bInsideBounds: flag stating whether or not the pointer is inside the bounds of a work area"""
		self._position = position
		self._bInsideBounds = bInsideBounds
		self._buttonStates = 0


	def _p_setPosition(self, position):
		"""Private - set the pointer position
		@position: the pointer position"""
		self._position = position

	def _p_setButton(self, button, value):
		"""Private - set the state of a button
		@button: the index of the button
		@value: the button value"""
		assert button > 0  and  button <= 5, 'button index out of range'

		self._p_setStateBit( PointerStatus.s_buttonMasks[button - 1], value )


	def _p_setInsideBounds(self, value):
		"""Private - set the value of the inside bounds flag
		@value: the value of the inside bounds flag"""
		self._bInsideBounds = value

	def _p_setState(self, value):
		"""Private - set the button states field
		@value: bit-wise or of the flags, from Gtk/Gdk"""
		self._buttonStates = value

	def _p_reset(self):
		"""Private - reset"""
		self._position = Point2i()
		self._bInsideBounds = False
		self._buttonStates = 0



	def _p_getPosition(self):
		"""Get the pointer position"""
		return self._position

	def _p_getPositionF(self):
		"""Get the pointer position as a Point2"""
		return self._position.toPoint2()

	def isButtonPressed(self, button):
		"""Check if a button is pressed
		@button: the index of the button"""
		assert button > 0  and  button <= 5, 'button index out of range'

		return self._p_getStateBit( PointerStatus.s_buttonMasks[button - 1] )

	def _p_isInsideBounds(self):
		"""Returns true if the pointer is inside the bounds of the work area"""
		return self._bInsideBounds

	def _p_getState(self):
		"""Get the button states field"""
		return self._buttonStates

	def _p_isShiftKeyDown(self):
		"""Returns true if the shift key is pressed"""
		return self._p_getStateBit( gtk.gdk.SHIFT_MASK )

	def _p_isControlKeyDown(self):
		"""Returns true if the shift key is pressed"""
		return self._p_getStateBit( gtk.gdk.CONTROL_MASK )

	def _p_isAltKeyDown(self):
		"""Returns true if the shift key is pressed"""
		return self._p_getStateBit( gtk.gdk.MOD1_MASK )


	def _p_setStateBit(self, mask, value):
		"""Set the state of the bits defined by @mask to @value
		@mask: bit-wise or of the bits to modify
		@value: the value to set the bits to"""
		v = 0
		if value:
			v = mask
		self._buttonStates = ( self._buttonStates & ~mask )  |  v

	def _p_getStateBit(self, mask):
		return ( self._buttonStates & mask )  !=  0


	position = property( _p_getPosition )
	positionF = property( _p_getPositionF )
	numButtons = 5
	bInsideBounds = property( _p_isInsideBounds )
	state = property( _p_getState )
	bShiftKey = property( _p_isShiftKeyDown )
	bControlKey = property( _p_isControlKeyDown )
	bAltKey = property( _p_isAltKeyDown )





if __name__ == '__main__':
	p = PointerStatus()

	p._p_setButton( 1, True )
	assert p.getButton( 1 )
	p._p_setButton( 1, False )
	assert not p.getButton( 1 )


