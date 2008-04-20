##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.SheetEdit.BoolFieldEditCheck import *
from Britefury.SheetEdit.BoolFieldEditCheckWithFn import *
from Britefury.SheetEdit.FieldLabelWithFn import *
from Britefury.SheetEdit.IntFieldEditEntry import *
from Britefury.SheetEdit.IntFieldEditSlider import *
from Britefury.SheetEdit.FloatFieldEditEntry import *
from Britefury.SheetEdit.FloatFieldEditSlider import *
from Britefury.SheetEdit.StringFieldEditEntry import *
from Britefury.SheetEdit.StringFieldViewLabel import *
from Britefury.SheetEdit.StringFieldEditAccelEntry import *
from Britefury.SheetEdit.AxisFieldEditRadioGroup import *
from Britefury.SheetEdit.Vector2FieldEditEntry import *
from Britefury.SheetEdit.Point2FieldEditEntry import *
from Britefury.SheetEdit.Vector3FieldEditEntry import *
from Britefury.SheetEdit.Point3FieldEditEntry import *
from Britefury.SheetEdit.QuaternionFieldEditEntry import *
from Britefury.SheetEdit.QuaternionFieldEditDirectionPopup import *
from Britefury.SheetEdit.Colour3fFieldEditColourButton import *
from Britefury.SheetEdit.SheetEditorButton import *



if __name__ == '__main__':
	import pygtk
	pygtk.require( '2.0' )
	import gtk


	from Britefury.Sheet.Sheet import *
	from Britefury.SheetEdit.SheetEdit import SheetEditor
	from Britefury.UI.FormLayout import FormLayoutFactory
	from Britefury.UI.TestWindow import *
	from Britefury.CommandHistory.CommandHistory import CommandHistory
	from Britefury.I18n import i18n
	from Britefury.extlibs.pyconsole.pyconsole import testScriptingWindow
	i18n.initialise()


	class ExampleSheet (Sheet):
		a = Field( int, 0 )
		b = Field( str, 'Hi' )


	class ExampleEditor (SheetEditor):
		sheet = SheetRefField( ExampleSheet )

		aLabel = FieldLabelWithFn( sheet.a, 'A' )
		aEdit = IntFieldEditEntry( sheet.a, 0, 1024, 1, 10 )
		bLabel = FieldLabelWithFn( sheet.b, 'B' )
		bEdit = StringFieldEditEntry( sheet.b )

		layout = FormLayoutFactory()
		layout.row()  <<  aLabel.label << aEdit.entry
		layout.row()  <<  bLabel.label << bEdit.entry
		layout.row()
		layout.finalise()



	window = makeEmptyTestWindow()

	commandHistory = CommandHistory()


	sheet = ExampleSheet()
	editor = ExampleEditor( None, None, window, commandHistory )

	editor.sheet = sheet

	window.add( editor.getSheetEditorWidget() )

	scriptWindow, scriptConsole = testScriptingWindow( 'Test scripting window', 'Script', window, False, locals() )

	gtk.main()
