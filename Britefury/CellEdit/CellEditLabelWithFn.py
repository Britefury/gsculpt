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
from Britefury.CellEdit.CellEditFunctionButton import CellEditFunctionButton, __bShowFunctionButtons__



class CellEditLabelWithFn (object):
	"""Cell function button and label
	@label - the function button and label"""


	def __init__(self, labelText):
		super( CellEditLabelWithFn, self ).__init__()
		self._functionButton =CellEditFunctionButton()
		self._functionButton.show()
		self._label = gtk.Label( labelText )
		self._label.show()
		self.label = gtk.HBox( spacing=3 )
		self.label.pack_start( self._functionButton.button, False, False )
		self.label.pack_start( self._label, False, False )


	def attachCell(self, cell):
		self._functionButton.attachCell( cell )

	def detachCell(self):
		self._functionButton.detachCell()


	def show(self):
		self.label.show()

	def hide(self):
		self.label.hide()
