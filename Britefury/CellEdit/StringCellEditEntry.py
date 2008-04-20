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


from Britefury.CellEdit.BasicWidgetSingleCellEdit import BasicWidgetSingleCellEdit



class StringCellEditEntry (BasicWidgetSingleCellEdit):
	"""Stringean cell editor base"""


	__valueclass__ = str


	def __init__(self):
		super( StringCellEditEntry, self ).__init__()
		self.entry = gtk.Entry()
		self.entry.connect( 'activate', self._p_onActivate )
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
			self.entry.set_text( value )
			self._bIgnoreEntry = False


	def _p_onActivate(self, entry):
		if not self._bIgnoreEntry:
			assert self._cell is not None
			value = self.entry.get_text()
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

	cell = StringCell( 'Hello world' )

	entry = StringCellEditEntry()
	entry.show()
	entry.attachCell( cell )

	runTestWindow( entry.entry )
