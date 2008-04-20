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
import gobject

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.UI.UIController import UIController



class ColumnTypeError: pass


class OptionList (UIController):
	choiceSignal = ClassSignal()


	def __init__(self, columnSpecs, choices = []):
		"""Constructor
		@columnSpecs: list of tuples: ( columnTitle, columnType )
		@choices: initial list of choices; list of tuples, where the first member of the tuple is the choice, and each subsequent member is the contents of the corresponding column"""
		super( OptionList, self ).__init__()
		storeColumnSpecs = [ spec[1]  for spec in columnSpecs ]
		self._store = gtk.ListStore( *storeColumnSpecs )
		self._view = gtk.TreeView( self._store )
		self._view.show()
		self._scrolledWindow = gtk.ScrolledWindow()
		self._scrolledWindow.set_policy( gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC )
		self._scrolledWindow.add( self._view )
		self._selection = self._view.get_selection()
		self._selection.set_mode( gtk.SELECTION_SINGLE )

		self._choices = []
		self._choice = None

		for columnIndex, columnSpec in enumerate( columnSpecs ):
			self._p_createCellRenderer( columnIndex, columnSpec )

		for choice in choices:
			self._choices.append( choice[0] )
			self._store.append( choice[1:] )

		self._selection.connect( 'changed', self._p_onChanged )


	def getWidget(self):
		return self._scrolledWindow


	def append(self, choice, *row):
		self._choices.append( choice )
		self._store.append( row )

	def insert(self, index, choice, *row):
		self._choices.insert( index, choice )
		self._store.insert( index, row )

	def remove(self, choice):
		index = self._choices.index( choice )
		del self._choices[index]
		del self._store[index]

	def clear(self):
		self._choices = []
		self._store.clear()


	def __setitem__(self, choice, row):
		index = self._choices.index( choice )
		self._store[index] = row

	def __len__(self):
		return len( self._choices )


	def _p_getChoice(self):
		return self._choice

	def _p_setChoice(self, choice):
		if choice != self._choice:
			try:
				row = self._choices.index( choice )
			except ValueError:
				if choice is None:
					row = -1
				else:
					raise
			self.applicationEventsBegin()
			self._selection.select_path( ( row, ) )
			self.applicationEventsEnd()


	def _p_onChanged(self, combo, data=None):
		model, treePaths = self._selection.get_selected_rows()
		if len( treePaths ) > 0:
			rowIndex = treePaths[0][0]
			self._choice = self._choices[rowIndex]
		else:
			self._choice = None
		self.choiceSignal.emit( self.uiEvent(), self._choice )


	def _p_createCellRenderer(self, columnIndex, columnSpec):
		renderer = None
		columnTitle, columnType = columnSpec

		columnString = ''
		if columnType is str:
			renderer, columnString = gtk.CellRendererText(), 'text'
		elif columnType is gtk.gdk.Pixbuf:
			renderer, columnString = gtk.CellRendererPixbuf(), 'pixbuf'
		else:
			raise ColumnTypeError

		kwargs = { columnString : columnIndex }

		viewColumn = gtk.TreeViewColumn( columnTitle, renderer, **kwargs )
		self._view.append_column( viewColumn )


	choice = property( _p_getChoice, _p_setChoice )




if __name__ == '__main__':
	def onDelete(widget, event):
		gtk.main_quit()

	def onChoice(event, choice):
		print optionList.index( choice ), choice


	optionList = OptionList( [ ( 'A', str ), ( 'B', str ) ] )
	optionList.getWidget().show()
	optionList.choiceSignal.connect( onChoice )

	window = gtk.Window( gtk.WINDOW_TOPLEVEL )
	window.connect( 'delete-event', onDelete )
	window.add( optionList.getWidget() )
	window.show()

	optionList.append( 'P', 'p', '1' )
	optionList.append( 'Q', 'q', '2' )
	optionList.append( 'R', 'r', '3' )

	gtk.main()


