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

from Britefury.Math.Math import Colour3f
from Britefury.Math.MathCellTypes import Colour3fCell

from Britefury.CellEdit.BasicWidgetSingleCellEdit import BasicWidgetSingleCellEdit



class Colour3fCellEditColourButton (BasicWidgetSingleCellEdit):
	"""Stringean cell editor base"""


	__valueclass__ = Colour3f


	def __init__(self):
		super( Colour3fCellEditColourButton, self ).__init__()
		self.colourButton = gtk.ColorButton()
		self.colourButton.connect( 'color-set', self._p_onColourSet )
		self._bIgnoreButton = False
		self._o_setWidgetSensitivity( False )


	def show(self):
		self.colourButton.show()

	def hide(self):
		self.colourButton.hide()


	def _o_setWidgetSensitivity(self, bSensitive):
		self.colourButton.set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		if isinstance( value, Colour3f ):
			self._bIgnoreButton = True
			gdkColour = gtk.gdk.Color( int( value.r * 65535.0 ), int( value.g * 65535.0 ), int( value.b * 65535.0 ) )
			self.colourButton.set_color( gdkColour )
			self._bIgnoreButton = False


	def _p_onColourSet(self, colourButton):
		if not self._bIgnoreButton:
			assert self._cell is not None
			gdkColour = self.colourButton.get_color()
			value = Colour3f( gdkColour.red / 65535.0,  gdkColour.green / 65535.0,  gdkColour.blue / 65535.0 )
			if self._cell.bLiteral  and  value != self._cell.literalValue:
				self._o_blockCell( value )
				self._cell.literalValue = value
				self._o_unblockCell()






if __name__ == '__main__':
	from Britefury.Cell.Cell import *
	from Britefury.UI.TestWindow import *
	from Britefury.UI.FormLayout import *
	from Britefury.I18n import i18n

	i18n.initialise()

	cell = Colour3fCell( Colour3f( 1.0, 0.5, 0.25 ) )

	colourButton = Colour3fCellEditColourButton()
	colourButton.show()
	colourButton.attachCell( cell )

	runTestWindow( colourButton.colourButton )
