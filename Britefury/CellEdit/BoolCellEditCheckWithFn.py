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


from Britefury.CellEdit.BasicWidgetSingleCellEditWithFn import BasicWidgetSingleCellEditWithFn




class BoolCellEditCheckWithFn (BasicWidgetSingleCellEditWithFn):
	"""Boolean cell editor base with cell function button"""


	__valueclass__ = bool


	def __init__(self, labelText):
		super( BoolCellEditCheckWithFn, self ).__init__()
		self._check = gtk.CheckButton( labelText )
		self._check.connect( 'clicked', self._p_onCheck )
		self._check.show()
		self.check = gtk.HBox( spacing=5 )
		self.check.pack_start( self._functionButton.button, False, False )
		self.check.pack_start( self._check )
		self._bIgnoreCheck = False
		self._o_setWidgetSensitivity( False )


	def show(self):
		self.check.show()

	def hide(self):
		self.check.hide()


	def _o_setWidgetSensitivity(self, bSensitive):
		self._check.set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		if isinstance( value, bool ):
			self._bIgnoreCheck = True
			self._check.set_active( value )
			self._bIgnoreCheck = False


	def _p_onCheck(self, check):
		if not self._bIgnoreCheck:
			assert self._cell is not None
			value = self._check.get_active()
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

	checkWithFn = BoolCellEditCheckWithFn( 'Boolean cell' )
	checkWithFn.show()
	checkWithFn.attachCell( cell )

	runTestWindow( checkWithFn.check )
