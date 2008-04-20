##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.SheetEdit.SheetEdit import *

from Britefury.Model.TargetListCellEdit import *



class TargetListFieldEdit (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'numTargets', 'removeTarget' ]


	def __init__(self, field, targetLabelText=_( '%d target' ), pluralTargetLabelText=_( '%d targets' ), removeTargetButtonText=_( 'Remove target' )):
		super( TargetListFieldEdit, self ).__init__( field )
		self._targetLabelText = targetLabelText
		self._pluralTargetLabelText = pluralTargetLabelText
		self._removeTargetButtonText = removeTargetButtonText


	def createElement(self, sheetEditContext):
		element = TargetListCellEdit( self._targetLabelText, self._pluralTargetLabelText, self._removeTargetButtonText )
		return element


	def initialiseElement(self, element, sheetEditContext):
		super( TargetListFieldEdit, self ).initialiseElement( element, sheetEditContext )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		super( TargetListFieldEdit, self ).shutdownElement( element, sheetEditContext )
		element.commandHistory = None
