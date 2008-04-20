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
from Britefury.UI.ConfirmDialog import confirmDialog

from Britefury.MainApp.Plugins.PluginInfo import *
from Britefury.MainApp.Plugins.PluginPath import *
from Britefury.MainApp.Plugins.PluginTable import *



class PluginInfoView (SheetEditor):
	sheet = SheetRefField( PluginInfo )


	nameLabel = StringFieldViewLabel( sheet.name )
	versionLabel = StringFieldViewLabel( sheet.version )
	vendorLabel = StringFieldViewLabel( sheet.vendor )



	layout = FormLayoutFactory()
	layout.row()  <<  _( 'Name:' )  <<  nameLabel.label
	layout.row()  <<  _( 'Version:' )  <<  versionLabel.label
	layout.row()  <<  _( 'Vendor:' )  <<  vendorLabel.label
	layout.finalise()






class PluginTableViewList (object):
	COLUMN_ITEM = 0
	COLUMN_NAME = 1

	selectionSignal = ClassSignal()



	def __init__(self, pluginTable):
		super( PluginTableViewList, self ).__init__()

		self._pluginTable = pluginTable
		self._pluginTable.appendedSignal.connect( self._p_onPluginPathAppended )
		self._pluginTable.removeSignal.connect( self._p_onPluginPathRemove )
		self._pluginTable.clearSignal.connect( self._p_onClear )

		self.selectedItems = []

		self._store = gtk.ListStore( object, str )
		self._sorted = gtk.TreeModelSort( self._store )
		self._sorted.set_sort_column_id( self.COLUMN_NAME, gtk.SORT_ASCENDING )
		self._treeView = gtk.TreeView( self._sorted )
		self._selection = self._treeView.get_selection()
		self._selection.set_mode( gtk.SELECTION_SINGLE )
		self._selection.connect( 'changed', self._p_onSelectionChanged )

		nameCellRenderer = gtk.CellRendererText()
		nameCellRenderer.set_property( 'editable', False )
		nameColumn = gtk.TreeViewColumn( _( 'Plugin' ), nameCellRenderer, text=self.COLUMN_NAME )
		nameColumn.set_expand( True )
		self._treeView.append_column( nameColumn )

		self._treeView.show()

		self._scroll = gtk.ScrolledWindow()
		self._scroll.set_policy( gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC )
		self._scroll.add( self._treeView )


		self._p_populate()



	def getWidget(self):
		return self._scroll





	def _p_populate(self):
		for pluginPath in self._pluginTable:
			self._p_onPluginPathAppended( pluginPath )



	def _p_onPluginPathAppended(self, pluginPath):
		self._store.append( ( pluginPath, pluginPath.pluginInfo.name ) )

	def _p_onPluginPathRemove(self, pluginPath):
		pluginPathIndex = self._pluginTable.index( pluginPath )
		rowPath = ( pluginPathIndex, )
		del self._store[rowPath]

	def _p_onClear(self):
		self._store.clear()



	def _p_onSelectionChanged(self, selection):
		selectedPaths = selection.get_selected_rows()[1]
		self.selectedItems = [ self._sorted[path][self.COLUMN_ITEM]  for path in selectedPaths ]
		self.selectionSignal.emit()






class PluginManagerWindow (object):
	closeSignal = ClassSignal()


	def __init__(self, pluginTable, parentWindow):
		self._pluginTable = pluginTable
		self._parentWindow = parentWindow


		titleLabel = gtk.Label()
		titleLabel.set_markup( '<span foreground="#000080"><b>'  +  _( 'Plugins' )  +  '</b></span>' )
		titleLabel.show()

		self._tableView = PluginTableViewList( pluginTable )
		self._tableView.selectionSignal.connect( self._p_onSelection )
		self._tableView.getWidget().show()



		self._pluginInfoView = PluginInfoView( None, None, self._parentWindow, None )

		self._pluginInfoView.getSheetEditorWidget().show()


		pluginBox = gtk.VBox( spacing=30 )
		pluginBox.pack_start( self._pluginInfoView.getSheetEditorWidget(), False, False )
		pluginBox.show()


		hBox = gtk.HBox( spacing=20, homogeneous=True )
		hBox.pack_start( self._tableView.getWidget(), True, True )
		hBox.pack_start( pluginBox, True, True )
		hBox.show()



		buttonBoxSep = gtk.HSeparator()
		buttonBoxSep.show()

		addButton = gtk.Button( stock=gtk.STOCK_ADD )
		addButton.connect( 'clicked', self._p_onAdd )
		addButton.show()

		removeButton = gtk.Button( stock=gtk.STOCK_REMOVE )
		removeButton.connect( 'clicked', self._p_onRemove )
		removeButton.show()

		closeButton = gtk.Button( stock=gtk.STOCK_OK )
		closeButton.connect( 'clicked', self._p_onClose )
		closeButton.show()

		buttonBox = gtk.HBox( spacing=10 )
		buttonBox.pack_end( closeButton, False, False )
		buttonBox.pack_end( removeButton, False, False )
		buttonBox.pack_end( addButton, False, False )
		buttonBox.show()


		vBox = gtk.VBox( spacing=20 )
		vBox.pack_start( titleLabel, False, False )
		vBox.pack_start( hBox, True, True )
		vBox.pack_start( buttonBoxSep, False, False )
		vBox.pack_start( buttonBox, False, False )
		vBox.show()


		self._window = gtk.Window( gtk.WINDOW_TOPLEVEL )
		self._window.set_title( _( 'Plugins' ) )
		self._window.set_transient_for( parentWindow )
		self._window.connect( 'delete-event', self._p_onWindowDelete )
		self._window.add( vBox )
		self._window.set_border_width( 20 )
		self._window.set_size_request( 600, 350 )

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

		self._currentItem = selectedItem
		if self._currentItem is not None:
			self._pluginInfoView.sheet = self._currentItem.pluginInfo
		else:
			self._pluginInfoView.sheet = None





	def _p_onAdd(self, button):
		gspluginFilter = gtk.FileFilter()
		gspluginFilter.set_name( _( 'gSculpt Plugins (*.gsplugin)' ) )
		gspluginFilter.add_pattern( '*.gsplugin' )

		openDialog = gtk.FileChooserDialog( _( 'Load plugin' ), self._window, gtk.FILE_CHOOSER_ACTION_OPEN,
										( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
		openDialog.set_current_folder( os.getcwd() )
		openDialog.add_filter( gspluginFilter )
		openDialog.show()
		response = openDialog.run()
		filename = openDialog.get_filename()
		openDialog.destroy()
		if response == gtk.RESPONSE_OK:
			if filename is not None:
				if confirmDialog( _( 'Confirm' ), _( 'Are you sure you wish to load this plugin?' ), gtk.STOCK_YES, gtk.STOCK_NO, 'y', 'n', True, self._window ):
					pluginPath = PluginPath()
					pluginPath.path = filename
					pluginInfo = pluginPath.pluginInfo
					if pluginPath.bLoadedSuccessfully:
						self._pluginTable.append( pluginPath )
					else:
						messageDialog( _( 'Could not load plugin' ), self._window, _( 'Could not load \'%s\'' )  %  ( filename, ) )



	def _p_onRemove(self, button):
		if self._currentItem is not None:
			pluginName = self._currentItem.pluginInfo.name
			if confirmDialog( _( 'Confirm' ), _( 'Are you sure you wish to remove the plugin \'%s\'?' )  %  ( pluginName, ), gtk.STOCK_YES, gtk.STOCK_NO, 'y', 'n', True, self._window ):
				self._pluginTable.remove( self._currentItem )


	def _p_onClose(self, button):
		self.hide()
		self.closeSignal.emit()



	def _p_onWindowDelete(self, window, event):
		self.hide()
		self.closeSignal.emit()
		# Don't allow the window to be deleted
		return True


