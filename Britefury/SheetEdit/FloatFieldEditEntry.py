##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.FloatCellEditEntry import FloatCellEditEntry

from Britefury.SheetEdit.SheetEdit import BasicSingleFieldWidgetEditor



class FloatFieldEditEntry (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'entry' ]

	def __init__(self, field, minValue, maxValue, step, page, numDigits):
		super( FloatFieldEditEntry, self ).__init__( field )
		self._minValue = minValue
		self._maxValue = maxValue
		self._step = step
		self._page = page
		self._numDigits = numDigits

	def createElement(self, sheetEditContext):
		return FloatCellEditEntry( self._minValue, self._maxValue, self._step, self._page, self._numDigits )




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
		a = Field( float, 0.0 )


	class ExampleEditor (SheetEditor):
		sheet = SheetRefField( ExampleSheet )

		aLabel = FieldLabelWithFn( sheet.a, 'A' )
		aEdit = FloatFieldEditEntry( sheet.a, -1024.0, 1024.0, 1.0, 10.0, 4 )

		layout = FormLayoutFactory()
		layout.row()  <<  aLabel.label << aEdit.entry
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


