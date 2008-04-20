##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.QuaternionCellEditDirectionPopup import QuaternionCellEditDirectionPopup

from Britefury.SheetEdit.SheetEdit import BasicSingleFieldWidgetEditor



class QuaternionFieldEditDirectionPopup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'popup' ]


	def __init__(self, field, popupLabelText):
		super( QuaternionFieldEditDirectionPopup, self ).__init__( field )
		self._popupLabelText = popupLabelText

	def createElement(self, sheetEditContext):
		return QuaternionCellEditDirectionPopup( sheetEditContext.view, self._popupLabelText )




if __name__ == '__main__':
	import pygtk
	pygtk.require( '2.0' )
	import gtk


	from Britefury.Sheet.Sheet import *
	from Britefury.SheetEdit.SheetEdit import SheetEditor
	from Britefury.SheetEdit.FieldLabelWithFn import FieldLabelWithFn
	from Britefury.SheetEdit.QuaternionFieldEditEntry import QuaternionFieldEditEntry
	from Britefury.UI.FormLayout import FormLayoutFactory
	from Britefury.UI.TestWindow import *
	from Britefury.CommandHistory.CommandHistory import CommandHistory
	from Britefury.Math.Math import *
	from Britefury.I18n import i18n
	i18n.initialise()


	class ExampleSheet (Sheet):
		a = Field( Quaternion, Quaternion() )


	class ExampleEditor (SheetEditor):
		sheet = SheetRefField( ExampleSheet )

		aLabel = FieldLabelWithFn( sheet.a, 'A' )
		aEntry = QuaternionFieldEditEntry( sheet.a )
		aPopup = QuaternionFieldEditDirectionPopup( sheet.a, 'Direction' )

		layout = FormLayoutFactory()
		layout.row()  <<  aLabel.label
		layout.row()  <<  aPopup.popup
		layout.row()  <<  'X:'  <<  aEntry.x
		layout.row()  <<  'Y:'  <<  aEntry.y
		layout.row()  <<  'Z:'  <<  aEntry.z
		layout.finalise()



	window = makeEmptyTestWindow()

	commandHistory = CommandHistory()


	sheet = ExampleSheet()
	editor = ExampleEditor( None, None, window, commandHistory )

	editor.sheet = sheet

	window.add( editor.getSheetEditorWidget() )

	cell = ExampleEditor.sheet.a._f_getCellFromInstance( editor )

	gtk.main()


