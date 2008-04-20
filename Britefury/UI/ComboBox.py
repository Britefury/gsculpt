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


class ComboBox (UIController):
	choiceSignal = ClassSignal()


	def __init__(self, columnSpecs = [ str ], choices = []):
		super( ComboBox, self ).__init__()
		self._store = gtk.ListStore( *columnSpecs )
		self._combo = gtk.ComboBox( self._store )

		self._choices = []
		self._choice = None

		for columnIndex, columnType in enumerate( columnSpecs ):
			self._p_createCellRenderer( columnIndex, columnType )

		for choice in choices:
			self._choices.append( choice[0] )
			self._store.append( choice[1:] )

		self._combo.connect_after( 'changed', self._p_onChanged )


	def getWidget(self):
		return self._combo


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
			self._combo.set_active( row )
			self.applicationEventsEnd()


	def _p_onChanged(self, combo, data=None):
		# This fixes an issue on the Win32 platform whereby the viewport mode selection combo 'locks' (its popup
		# refuses to disappear), rendering the application unusable. This is due to the combo being de-parented
		# in response to the 'changed' signal. Calling GtkComboBox.popdown() forces the popup to disappear.
		combo.popdown()

		rowIndex = combo.get_active()
		if rowIndex == -1:
			self._choice = None
		else:
			self._choice = self._choices[rowIndex]
		self.choiceSignal.emit( self.uiEvent(), self._choice )


	def _p_createCellRenderer(self, columnIndex, columnType):
		renderer = None
		columnString = ''
		if columnType is str:
			renderer, columnString = gtk.CellRendererText(), 'text'
		elif columnType is gtk.gdk.Pixbuf:
			renderer, columnString = gtk.CellRendererPixbuf(), 'pixbuf'
		else:
			raise ColumnTypeError

		kwargs = { columnString : columnIndex }

		self._combo.pack_start( renderer, False )
		self._combo.set_attributes( renderer, **kwargs )


	choice = property( _p_getChoice, _p_setChoice )




