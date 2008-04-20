##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Colour3f

from Britefury.CellEdit.Colour3fCellEditColourButton import Colour3fCellEditColourButton

from Britefury.SheetEdit.SheetEdit import BasicSingleFieldWidgetEditor



class Colour3fFieldEditColourButton (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'colourButton' ]

	def createElement(self, sheetEditContext):
		return Colour3fCellEditColourButton()




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
	from Britefury.I18n import i18n
	i18n.initialise()


	class ExampleSheet (Sheet):
		a = Field( Colour3f, Colour3f( 1.0, 0.0, 0.0 ) )


	class ExampleEditor (SheetEditor):
		sheet = SheetRefField( ExampleSheet )

		aLabel = FieldLabelWithFn( sheet.a, 'A' )
		aEdit = Colour3fFieldEditColourButton( sheet.a )

		layout = FormLayoutFactory()
		layout.row()  <<  aLabel.label << aEdit.colourButton
		layout.row()
		layout.finalise()



	window = makeEmptyTestWindow()

	commandHistory = CommandHistory()


	sheet = ExampleSheet()
	editor = ExampleEditor( None, None, window, commandHistory )

	editor.sheet = sheet

	window.add( editor.getSheetEditorWidget() )

	cell = ExampleEditor.sheet.a._f_getCellFromInstance( editor )
	#print cell, cell.value

	gtk.main()


