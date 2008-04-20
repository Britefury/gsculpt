##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.Point3CellEditEntry import Point3CellEditEntry

from Britefury.SheetEdit.SheetEdit import BasicFieldWidgetEditor



class Point3FieldEditEntry (BasicFieldWidgetEditor):
	__componentnames__ = [ 'x', 'y', 'z' ]

	def __init__(self, field, minValue, maxValue, step, page, numDigits):
		super( Point3FieldEditEntry, self ).__init__()
		self._field = field
		self._minValue = minValue
		self._maxValue = maxValue
		self._step = step
		self._page = page
		self._numDigits = numDigits


	def createElement(self, sheetEditContext):
		return Point3CellEditEntry( self._minValue, self._maxValue, self._step, self._page, self._numDigits )

	def initialiseElement(self, element, sheetEditContext):
		super( Point3FieldEditEntry, self ).initialiseElement( element, sheetEditContext )
		element.attachCell( self._p_getCell( self._field, sheetEditContext ) )

	def shutdownElement(self, element, sheetEditContext):
		super( Point3FieldEditEntry, self ).shutdownElement( element, sheetEditContext )
		element.detachCell()




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
		a = Field( Point3, Point3() )


	class ExampleEditor (SheetEditor):
		sheet = SheetRefField( ExampleSheet )

		aLabel = FieldLabelWithFn( sheet.a, 'A' )
		aEdit = Point3FieldEditEntry( sheet.a, 0, 1024, 1, 10, 4 )

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


