##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.BoolCellEditCheck import BoolCellEditCheck

from Britefury.SheetEdit.SheetEdit import BasicSingleFieldWidgetEditor



class BoolFieldEditCheck (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'check' ]

	def __init__(self, field, labelText):
		super( BoolFieldEditCheck, self ).__init__( field )
		self._labelText = labelText

	def createElement(self, sheetEditContext):
		return BoolCellEditCheck( self._labelText )




if __name__ == '__main__':
	import pygtk
	pygtk.require( '2.0' )
	import gtk


	from Britefury.Sheet.Sheet import *
	from Britefury.SheetEdit.SheetEdit import SheetEditor
	from Britefury.UI.FormLayout import FormLayoutFactory
	from Britefury.UI.TestWindow import *
	from Britefury.CommandHistory.CommandHistory import CommandHistory


	class ExampleSheet (Sheet):
		a = Field( bool, False )


	class ExampleEditor (SheetEditor):
		sheet = SheetRefField( ExampleSheet )

		aEdit = BoolFieldEditCheck( sheet.a, 'A' )

		layout = FormLayoutFactory()
		layout.row()  <<  'Test' << aEdit.check
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


