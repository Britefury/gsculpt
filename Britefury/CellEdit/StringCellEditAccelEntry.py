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


from Britefury.UI.GtkAccelEntry import GtkAccelEntry

from Britefury.CellEdit.BasicWidgetSingleCellEdit import BasicWidgetSingleCellEdit



class StringCellEditAccelEntry (BasicWidgetSingleCellEdit):
	"""Stringean cell editor base"""


	__valueclass__ = str


	def __init__(self):
		super( StringCellEditAccelEntry, self ).__init__()
		self.entry = GtkAccelEntry()
		self.entry.connect( 'accel-set', self._p_onAccel )
		self._bIgnoreEntry = False
		self._o_setWidgetSensitivity( False )


	def show(self):
		self.entry.show()

	def hide(self):
		self.entry.hide()


	def _o_setWidgetSensitivity(self, bSensitive):
		self.entry.set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		if isinstance( value, str ):
			self._bIgnoreEntry = True
			self.entry.set_accel( value )
			self._bIgnoreEntry = False


	def _p_onAccel(self, entry, accelName):
		if not self._bIgnoreEntry:
			assert self._cell is not None
			if self._cell.bLiteral  and  accelName != self._cell.literalValue:
				self._o_blockCell( accelName )
				self._cell.literalValue = accelName
				self._o_unblockCell()










if __name__ == '__main__':
	from Britefury.Cell.Cell import *
	from Britefury.UI.TestWindow import *
	from Britefury.UI.FormLayout import *
	from Britefury.I18n import i18n

	i18n.initialise()

	cell = StringCell( 'a' )

	entry = StringCellEditAccelEntry()
	entry.show()
	entry.attachCell( cell )

	runTestWindow( entry.entry )
