##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.CellEdit.CellEdit import CellEdit

from Britefury.SheetEdit.SheetEdit import *



class SheetEditorButton (BasicFieldWidgetEditor):
	__componentnames__ = [ 'button' ]

	class _ButtonElement (CellEdit):
		def __init__(self, labelText, instance, onClickedMethod):
			super( SheetEditorButton._ButtonElement, self ).__init__()
			self.button = gtk.Button( labelText )
			self.button.connect( 'clicked', self._p_onClicked )
			self._o_setUISensitivity( True )
			self._instance = instance
			self._onClickedMethod = onClickedMethod


		def show(self):
			self.button.show()

		def hide(self):
			self.button.hide()


		def _o_isValid(self):
			return True


		def _o_setUISensitivity(self, bSensitive):
			self.button.set_sensitive( bSensitive )


		def _p_onClicked(self, check):
			self._onClickedMethod( self._instance )



	def __init__(self, labelText, onClicked):
		super( SheetEditorButton, self ).__init__()
		self._labelText = labelText
		self._onClicked = onClicked


	def createElement(self, sheetEditContext):
		return SheetEditorButton._ButtonElement( self._labelText, sheetEditContext.sheetEditor, self._onClicked )







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
		pass


	class ExampleEditor (SheetEditor):
		def onClick(self):
			print 'clicked', self

		sheet = SheetRefField( ExampleSheet )

		button = SheetEditorButton( 'Click me', onClick )

		layout = FormLayoutFactory()
		layout.row()  <<  button.button
		layout.row()
		layout.finalise()



	window = makeEmptyTestWindow()

	commandHistory = CommandHistory()


	sheet = ExampleSheet()
	editor = ExampleEditor( None, None, window, commandHistory )

	editor.sheet = sheet

	window.add( editor.getSheetEditorWidget() )

	gtk.main()


