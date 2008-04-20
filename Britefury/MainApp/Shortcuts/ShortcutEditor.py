##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os

import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Sheet.Sheet import *

from Britefury.SheetEdit.SheetEdit import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.UI.UIAction import UIAction
from Britefury.UI.FormLayout import FormLayout
from Britefury.UI.MessageDialog import messageDialog
from Britefury.UI.ConfirmOverwriteFileDialog import confirmOverwriteFileDialog

from Britefury.MainApp.Shortcuts.Shortcut import *



class ShortcutEditor (SheetEditor):
	sheet = SheetRefField( Shortcut )


	def _p_onReset(self):
		if self.sheet is not None:
			self.sheet.reset()


	def _p_humanReadablePath(self):
		if self.sheet is not None:
			action = self.sheet.action

			if action is not None:
				return '.'.join( action.humanReadablePathTuple )
		return ''

	def _p_hasSheet(self):
		return self.sheet is not None


	humanReadablePath = FunctionField( _p_humanReadablePath )
	_hasSheet = FunctionField( _p_hasSheet )

	pathView = StringFieldViewLabel( humanReadablePath )
	accelLabel = FieldLabelWithFn( sheet.accelString, _( 'Key:' ) )
	accelEntry = StringFieldEditAccelEntry( sheet.accelString )
	resetButton = SheetEditorButton( _( 'Reset to default' ), _p_onReset )

	resetButton.sensitivityField = _hasSheet







	layout = FormLayoutFactory()
	layout.row()  <<  _( 'Shortcut:' )
	layout.row()  <<  pathView.label
	layout.row()  <<  accelLabel.label  <<  accelEntry.entry
	layout.row()
	layout.row()  <<  resetButton.button
	layout.finalise()






class ShortcutTableViewTree (object):
	COLUMN_ITEM = 0
	COLUMN_PATH = 1
	COLUMN_ACCEL = 2

	selectionSignal = ClassSignal()



	def __init__(self, shortcutTable):
		super( ShortcutTableViewTree, self ).__init__()

		self._shortcutTable = shortcutTable

		self.selectedItems = []

		self._store = gtk.TreeStore( object, str, str )
		self._sorted = gtk.TreeModelSort( self._store )
		self._sorted.set_sort_column_id( ShortcutTableViewTree.COLUMN_PATH, gtk.SORT_ASCENDING )
		self._treeView = gtk.TreeView( self._sorted )
		self._selection = self._treeView.get_selection()
		self._selection.set_mode( gtk.SELECTION_SINGLE )
		self._selection.connect( 'changed', self._p_onSelectionChanged )

		pathCellRenderer = gtk.CellRendererText()
		pathCellRenderer.set_property( 'editable', False )
		pathColumn = gtk.TreeViewColumn( _( 'Shortcut' ), pathCellRenderer, text=ShortcutTableViewTree.COLUMN_PATH )
		pathColumn.set_expand( True )
		self._treeView.append_column( pathColumn )

		accelCellRenderer = gtk.CellRendererText()
		accelCellRenderer.set_property( 'editable', False )
		accelColumn = gtk.TreeViewColumn( _( 'Key' ), accelCellRenderer, text=ShortcutTableViewTree.COLUMN_ACCEL )
		self._treeView.append_column( accelColumn )

		self._treeView.show()

		self._scroll = gtk.ScrolledWindow()
		self._scroll.set_policy( gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC )
		self._scroll.add( self._treeView )


		self._shortcutMap = {}


		self._p_populate()



	def getWidget(self):
		return self._scroll




	def _p_buildMapEntry(self, fullHumPath, humPath, mapTable, shortcut):
		if len( humPath ) == 0:
			return
		elif len( humPath ) == 1:
			if not isinstance( mapTable, dict ):
				if isinstance( mapTable, Shortcut ):
					print 'Error: human readable paths are not terminal; human readable path: \'%s\', shortcut path: \'%s\', existing shortcut path: \'%s\''  %  ( '.'.join( fullHumPath ), shortcut.path, mapTable.path )
				else:
					print 'Error: human readable paths are not terminal; human readable path: \'%s\', shortcut path: \'%s\', existing map table entry is not a shortcut'  % ( '.'.join( fullHumPath ), shortcut.path )
			else:
				try:
					entry = mapTable[humPath[0]]
				except KeyError:
					mapTable[humPath[0]] = shortcut
				else:
					if isinstance( entry, Shortcut ):
						print 'Warning: human readable paths are not unique; human readable path: \'%s\', shortcut path: \'%s\', existing shortcut path: \'%s\''  %  ( '.'.join( fullHumPath ), shortcut.path, entry.path )
					else:
						print 'Warning: human readable paths are not unique; human readable path: \'%s\', shortcut path: \'%s\', existing map table entry is not a shortcut'  %  ( '.'.join( fullHumPath ), shortcut.path )

					bFoundValidPath = False
					testIndex = 2
					while not bFoundValidPath:
						pathToTest = '%s (%d)'  %  ( humPath[0], testIndex )
						try:
							entry = mapTable[pathToTest]
						except KeyError:
							mapTable[pathToTest] = shortcut
							bFoundValidPath = True
						testIndex += 2
		else:
			subMap = mapTable.setdefault( humPath[0], {} )
			self._p_buildMapEntry( fullHumPath, humPath[1:], subMap, shortcut )


	def _p_addMapEntry(self, path, shortcut):
		try:
			action = UIAction._pathToActionTable[path]
		except KeyError:
			raise ValueError, 'could not get action'

		humPath = action.humanReadablePathTuple
		self._p_buildMapEntry( humPath, humPath, self._shortcutMap, shortcut )


	def _p_populateMap(self):
		for path, shortcut in self._shortcutTable._pathToShortcut.items():
			self._p_addMapEntry( path, shortcut )



	def _p_populateViewSubtree(self, parentIter, mapTable):
		for humPathKey, shortcutOrSubtree in mapTable.items():
			if isinstance( shortcutOrSubtree, dict ):
				itemIter = self._store.append( parentIter, ( None, humPathKey, '' ) )
				self._p_populateViewSubtree( itemIter, shortcutOrSubtree )
			else:
				def makeOnAccelLabel(shortcut, itemPath):
					def refreshAccelLabel():
						self._store[itemPath][ShortcutTableViewTree.COLUMN_ACCEL] = shortcut.accelLabel
					def queueRefreshAccelLabel():
						queueEvent( refreshAccelLabel )
					return queueRefreshAccelLabel
				itemIter = self._store.append( parentIter, ( shortcutOrSubtree, humPathKey, shortcutOrSubtree.accelLabel ) )
				itemPath = self._store.get_path( itemIter )
				shortcutOrSubtree.cells.accelLabel.changedSignal.connect( makeOnAccelLabel( shortcutOrSubtree, itemPath ) )



	def _p_populate(self):
		self._p_populateMap()
		self._p_populateViewSubtree( None, self._shortcutMap )



	def _p_onSelectionChanged(self, selection):
		selectedPaths = selection.get_selected_rows()[1]
		self.selectedItems = [ self._sorted[path][ShortcutTableViewTree.COLUMN_ITEM]  for path in selectedPaths ]
		self.selectedItems = [ item   for item in self.selectedItems   if isinstance( item, Shortcut ) ]
		self.selectionSignal.emit()






class ShortcutTableEditorWindow (object):
	closeSignal = ClassSignal()


	def __init__(self, shortcutTable, parentWindow):
		self._shortcutTable = shortcutTable
		self._parentWindow = parentWindow


		titleLabel = gtk.Label()
		titleLabel.set_markup( _( '<span foreground="#000080"><b>Shortcuts</b></span>' ) )
		titleLabel.show()

		self._tableView = ShortcutTableViewTree( shortcutTable )
		self._tableView.selectionSignal.connect( self._p_onSelection )
		self._tableView.getWidget().show()



		self._shortcutEditor = ShortcutEditor( None, None, self._parentWindow, None )

		self._shortcutEditor.getSheetEditorWidget().show()


		collisionsLabel = gtk.Label( _( 'This key is also used by:' ) )
		collisionsLabel.show()

		self._collisions = gtk.TextView()
		self._collisions.set_editable( False )
		self._collisions.show()

		collisionsScrolledWin = gtk.ScrolledWindow()
		collisionsScrolledWin.add( self._collisions )
		collisionsScrolledWin.set_policy( gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC )
		collisionsScrolledWin.show()

		collisionsBox = gtk.VBox()
		collisionsBox.pack_start( collisionsLabel, False, False )
		collisionsBox.pack_start( collisionsScrolledWin, True, True )
		collisionsBox.show()



		shortcutBox = gtk.VBox( spacing=30, homogeneous=True )
		shortcutBox.pack_start( self._shortcutEditor.getSheetEditorWidget(), False, False )
		shortcutBox.pack_start( collisionsBox, True, True )
		shortcutBox.show()


		hBox = gtk.HBox( spacing=20, homogeneous=True )
		hBox.pack_start( self._tableView.getWidget(), True, True )
		hBox.pack_start( shortcutBox, True, True )
		hBox.show()



		buttonBoxSep = gtk.HSeparator()
		buttonBoxSep.show()

		loadButton = gtk.Button( stock=gtk.STOCK_OPEN )
		loadButton.connect( 'clicked', self._p_onOpen )
		loadButton.show()

		saveButton = gtk.Button( stock=gtk.STOCK_SAVE )
		saveButton.connect( 'clicked', self._p_onSave )
		saveButton.show()

		closeButton = gtk.Button( stock=gtk.STOCK_OK )
		closeButton.connect( 'clicked', self._p_onClose )
		closeButton.show()

		buttonBox = gtk.HBox( spacing=10 )
		buttonBox.pack_start( loadButton, False, False )
		buttonBox.pack_start( saveButton, False, False )
		buttonBox.pack_end( closeButton, False, False )
		buttonBox.show()


		vBox = gtk.VBox( spacing=20 )
		vBox.pack_start( titleLabel, False, False )
		vBox.pack_start( hBox, True, True )
		vBox.pack_start( buttonBoxSep, False, False )
		vBox.pack_start( buttonBox, False, False )
		vBox.show()


		self._window = gtk.Window( gtk.WINDOW_TOPLEVEL )
		self._window.set_title( _( 'Shortcuts' ) )
		self._window.set_transient_for( parentWindow )
		self._window.connect( 'delete-event', self._p_onWindowDelete )
		self._window.add( vBox )
		self._window.set_border_width( 20 )
		self._window.set_size_request( -1, 600 )

		self._currentItem = None



	def show(self):
		self._window.show()


	def hide(self):
		self._window.hide()


	def _p_onSelection(self):
		assert len( self._tableView.selectedItems ) <= 1

		if len( self._tableView.selectedItems ) == 0:
			selectedItem = None
		else:
			selectedItem = self._tableView.selectedItems[0]

		if self._currentItem is not None:
			self._currentItem.cells.accelString.changedSignal.disconnect( self._p_onAccelStringChanged )

		self._currentItem = selectedItem
		self._shortcutEditor.sheet = self._currentItem

		if self._currentItem is not None:
			self._currentItem.cells.accelString.changedSignal.connect( self._p_onAccelStringChanged )

		self._p_updateCollisions()



	def _p_onAccelStringChanged(self):
		queueEvent( self._p_updateCollisions )


	def _p_updateCollisions(self):
		# Start with empty string
		text = ''

		# If there is a current shortcut selected
		if self._currentItem is not None:
			# Get the key combo for it
			currentItemKey = gtk.accelerator_parse( self._currentItem.accelString )

			# Check that a shortcut key has been assigned
			if currentItemKey != ( 0, 0 ):
				# Get all shortcuts
				allShortcuts = self._shortcutTable.getShortcuts()

				# For each shortcut....
				for shortcut in allShortcuts:
					# ... that is not the current shortcut, and has the same shortcut key assigned:
					if shortcut is not self._currentItem  and  gtk.accelerator_parse( shortcut.accelString )  ==  currentItemKey:
						# Get the action
						action = None

						try:
							action = UIAction._pathToActionTable[shortcut.path]
						except KeyError:
							pass
						else:
							# Append a line of text identifying it to the text buffer
							text += '.'.join( action.humanReadablePathTuple ) + '\n'

		textBuffer = self._collisions.get_buffer()
		textBuffer.set_text( text )



	def _p_onOpen(self, button):
		gsshortcutsFilter = gtk.FileFilter()
		gsshortcutsFilter.set_name( _( 'gSculpt Shortcuts (*.gsshortcuts)' ) )
		gsshortcutsFilter.add_pattern( '*.gsshortcuts' )

		openDialog = gtk.FileChooserDialog( _( 'Open' ), self._window, gtk.FILE_CHOOSER_ACTION_OPEN,
										( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
		openDialog.set_current_folder( os.getcwd() )
		openDialog.add_filter( gsshortcutsFilter )
		openDialog.show()
		response = openDialog.run()
		filename = openDialog.get_filename()
		openDialog.destroy()
		if response == gtk.RESPONSE_OK:
			if filename is not None:
				try:
					readShortcuts( self._shortcutTable, filename )
				except IOError:
					messageDialog( _( 'Could not read file' ), self._window, _( 'Could not read \'%s\'' )  %  ( filename, ) )


	def _p_onSave(self, button):
		gsshortcutsFilter = gtk.FileFilter()
		gsshortcutsFilter.set_name( _( 'gSculpt Shortcuts (*.gsshortcuts)' ) )
		gsshortcutsFilter.add_pattern( '*.gsshortcuts' )

		saveDialog = gtk.FileChooserDialog( _( 'Save' ), self._window, gtk.FILE_CHOOSER_ACTION_SAVE,
									( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
		saveDialog.add_filter( gsshortcutsFilter )
		saveDialog.set_current_folder( os.getcwd() )
		saveDialog.show()
		response = saveDialog.run()
		filenameFromDialog = saveDialog.get_filename()
		saveDialog.destroy()
		filename = None
		if response == gtk.RESPONSE_OK:
			if filenameFromDialog is not None:
				if os.path.exists( filenameFromDialog ):
					if confirmOverwriteFileDialog( filenameFromDialog, self._window ):
						filename = filenameFromDialog
				else:
					filename = filenameFromDialog
		if filename is not None:
			try:
				writeShortcuts( self._shortcutTable, filename )
			except IOError:
				messageDialog( _( 'Could not write file' ), self._window, _( 'Could not write \'%s\'' )  %  ( filename, ) )


	def _p_onClose(self, button):
		self.hide()
		self.closeSignal.emit()



	def _p_onWindowDelete(self, window, event):
		self.hide()
		self.closeSignal.emit()
		# Don't allow the window to be deleted
		return True




if __name__ == '__main__':
	def onDelete(window, event, data=None):
		gtk.main_quit()

	view = ShortcutTableViewTree( ShortcutTable() )
	view.getWidget().show()


	window = gtk.Window( gtk.WINDOW_TOPLEVEL )
	window.connect( 'delete-event', onDelete )
	window.add( view.getWidget() )
	window.set_border_width( 20 )
	window.show()

	gtk.main()
