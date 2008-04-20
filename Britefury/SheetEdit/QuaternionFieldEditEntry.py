##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.QuaternionCellEditEntry import QuaternionCellEditEntry

from Britefury.SheetEdit.SheetEdit import BasicSingleFieldWidgetEditor



class QuaternionFieldEditEntry (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'x', 'y', 'z' ]

	def createElement(self, sheetEditContext):
		return QuaternionCellEditEntry()




if __name__ == '__main__':
	import pygtk
	pygtk.require( '2.0' )
	import gtk


	from Britefury.Sheet.Sheet import *
	from Britefury.SheetEdit.SheetEdit import SheetEditor
	from Britefury.SheetEdit.FieldLabelWithFn import FieldLabelWithFn
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
		aEdit = QuaternionFieldEditEntry( sheet.a )

		layout = FormLayoutFactory()
		layout.row()  <<  aLabel.label
		layout.row()  <<  'X:'  <<  aEdit.x
		layout.row()  <<  'Y:'  <<  aEdit.y
		layout.row()  <<  'Z:'  <<  aEdit.z
		layout.finalise()



	window = makeEmptyTestWindow()

	commandHistory = CommandHistory()


	sheet = ExampleSheet()
	editor = ExampleEditor( None, None, window, commandHistory )

	editor.sheet = sheet

	window.add( editor.getSheetEditorWidget() )

	cell = ExampleEditor.sheet.a._f_getCellFromInstance( editor )

	gtk.main()


