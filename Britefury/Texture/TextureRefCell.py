##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Cell.Cell import *

from Britefury.Sheet.Sheet import *

from TextureRef import TextureRef
from TextureChooser import TextureChooser

from Britefury.CellEdit.BasicWidgetSingleCellEdit import BasicWidgetSingleCellEdit




class TextureRefCell (Cell):
	valueClass = TextureRef
	bAllowNone = False


registerCellType( TextureRef, TextureRefCell )



class TextureRefCellEditChooser (BasicWidgetSingleCellEdit):
	__valueclass__ = TextureRef


	def __init__(self, parentWindow, dialogTitle):
		super( TextureRefCellEditChooser, self ).__init__()
		self.chooser = TextureChooser( None, parentWindow, dialogTitle )
		self.chooser.textureSignal.connect( self._p_onChooser )

		self._o_setWidgetSensitivity( False )


	def show(self):
		self.chooser.getWidget().show()

	def hide(self):
		self.chooser.getWidget().hide()

	def _o_setWidgetSensitivity(self, bSensitive):
		self.chooser.getWidget().set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		if value is not None:
			if isinstance( value, TextureRef ):
				self.chooser.texture = value.texture
		else:
			self.chooser.textire = None


	def _p_onChooser(self, event, texture):
		if event.bUserEvent:
			assert self._cell is not None
			value = self.check.get_active()
			if self._cell.bLiteral  and  value != self._cell.literalValue:
				self._o_blockCell( value )
				self._cell.literalValue = TextureRef( texture )
				self._o_unblockCell()


