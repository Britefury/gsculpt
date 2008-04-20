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


from Britefury.Cell.CellEvaluator import CellEvaluator
from Britefury.Cell.CellEvaluatorPythonExpression import CellEvaluatorPythonExpression, CellEvaluatorPythonExpressionCompilationError

from Britefury.CellEdit.WidgetSingleCellEdit import WidgetSingleCellEdit

from Britefury.UI.GSImage import *
from Britefury.UI.MessageDialog import messageDialog



_FUNCTION_BUTTON_IMAGE_FILENAME = 'function_button.png'



__bShowFunctionButtons__ = False






class CellEditFunctionButton (WidgetSingleCellEdit):
	def __init__(self):
		super( CellEditFunctionButton, self ).__init__()
		self.button = gtk.Button()
		functionLabel = gtk.Label()
		functionLabel.set_markup( '<span foreground="#c00000"><i><b>f</b>( )</i></span>' )
		functionLabel.show()
		self.button.add( functionLabel )
		self.button.connect( 'clicked', self._p_onButtonClicked )
		self._o_setUISensitivity( False )


	def show(self):
		if __bShowFunctionButtons__:
			self.button.show()

	def hide(self):
		self.button.hide()


	def _o_setUISensitivity(self, bSensitive):
		self.button.set_sensitive( bSensitive )



	def _p_onButtonClicked(self, button):
		fixItem = gtk.MenuItem( _( 'Fixed value' ) )
		fixItem.show()
		fixItem.connect( 'activate', self._p_onFixedValue )

		pythonExpressionItem = gtk.MenuItem( _( 'Python expression' ) )
		pythonExpressionItem.show()
		pythonExpressionItem.connect( 'activate', self._p_onPythonExpression )

		menu = gtk.Menu()
		menu.append( fixItem )
		menu.append( pythonExpressionItem )
		menu.show()
		menu.popup( None, None, None, 1, 0 )



	def _p_onFixedValue(self, item):
		if self._cell.bValid:
			if isinstance( self._cell.evaluator, CellEvaluator ):
				self._cell.literalValue = self._cell.value


	def _p_onPythonExpression(self, item):
		if self._cell.bValid:
			def onEntryActivate(entry):
				dialog.response( gtk.RESPONSE_OK )

			#dialogFlags = gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT
			dialogFlags = gtk.DIALOG_MODAL

			dialog = gtk.Dialog( _( 'Enter python expression' ), None, dialogFlags, buttons = ( gtk.STOCK_OK, gtk.RESPONSE_OK, gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL ) )

			promptLabel = gtk.Label( _( 'Expression:' ) )

			entry = gtk.Entry()
			entry.connect( 'activate', onEntryActivate )

			contentsVBox = gtk.VBox( spacing=20 )
			contentsVBox.pack_start( promptLabel )
			contentsVBox.pack_start( entry )

			paddingHBox = gtk.HBox()
			paddingHBox.pack_start( contentsVBox, padding=30 )

			dialog.vbox.pack_start( paddingHBox, padding=30 )

			dialog.show_all()
			dialog.grab_focus()
			response = dialog.run()
			dialog.destroy()

			if response == gtk.RESPONSE_OK:
				if self._cell.bValid:
					try:
						self._cell.evaluator = CellEvaluatorPythonExpression( entry.get_text() )
					except CellEvaluatorPythonExpressionCompilationError:
						messageDialog( 'Compilation error', None, 'Could not compile expression \'%s\''  %  ( entry.get_text(), ) )
			else:
				return None






if __name__ == '__main__':
	from Britefury.Cell.Cell import *
	from Britefury.UI.TestWindow import *
	from Britefury.I18n import i18n

	i18n.initialise()


	button = CellEditFunctionButton()
	button.button.show()

	runTestWindow( button.button )
