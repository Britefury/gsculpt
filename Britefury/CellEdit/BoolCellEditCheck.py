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



class BoolCellEditCheck (BasicWidgetSingleCellEdit):
	"""Boolean cell editor base"""


	__valueclass__ = bool


	def __init__(self, labelText):
		super( BoolCellEditCheck, self ).__init__()
		self.check = gtk.CheckButton( labelText )
		self.check.connect( 'clicked', self._p_onCheck )
		self._bIgnoreCheck = False
		self._o_setWidgetSensitivity( False )


	def show(self):
		self.check.show()

	def hide(self):
		self.check.hide()


	def _o_setWidgetSensitivity(self, bSensitive):
		self.check.set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		if isinstance( value, bool ):
			self._bIgnoreCheck = True
			self.check.set_active( value )
			self._bIgnoreCheck = False


	def _p_onCheck(self, check):
		if not self._bIgnoreCheck:
			assert self._cell is not None
			value = self.check.get_active()
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

	cell = BoolCell( False )

	check = BoolCellEditCheck( 'Boolean cell' )
	check.show()
	check.attachCell( cell )

	runTestWindow( check.check )
