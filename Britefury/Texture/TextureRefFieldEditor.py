##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.SheetEdit.SheetEdit import BasicSingleFieldWidgetEditor

from Britefury.Texture.TextureRefCell import TextureRefCellEditChooser


# Column editor for TextureRef

class TextureRefFieldEditChooser (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'chooser' ]

	def __init__(self, textureRefField, chooserDialogTitle):
		super( TextureRefFieldEditChooser, self ).__init__( textureRefField )
		self._chooserDialogTitle = chooserDialogTitle


	def createElement(self, sheetEditContext):
		return TextureRefCellEditChooser( sheetEditContext.parentWindow, self._chooserDialogTitle )

