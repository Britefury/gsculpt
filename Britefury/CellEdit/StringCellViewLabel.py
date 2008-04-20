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


from Britefury.CellEdit.BasicWidgetSingleCellView import BasicWidgetSingleCellView



class StringCellViewLabel (BasicWidgetSingleCellView):
	"""Stringean cell editor base"""


	__valueclass__ = str


	def __init__(self):
		super( StringCellViewLabel, self ).__init__()
		self.label = gtk.Label()
		self._o_setWidgetSensitivity( False )


	def show(self):
		self.label.show()

	def hide(self):
		self.label.hide()


	def _o_setWidgetSensitivity(self, bSensitive):
		self.label.set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		if isinstance( value, str ):
			self.label.set_text( value )











if __name__ == '__main__':
	from Britefury.Cell.Cell import *
	from Britefury.UI.TestWindow import *
	from Britefury.UI.FormLayout import *
	from Britefury.I18n import i18n

	i18n.initialise()

	cell = StringCell( 'Hello world' )

	label = StringCellViewLabel()
	label.show()
	label.attachCell( cell )

	runTestWindow( label.label )
