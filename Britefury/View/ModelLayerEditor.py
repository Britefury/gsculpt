##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy

import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.Util.SignalSlot import *

from Britefury.UI.UIController import UIController
from Britefury.UI.AcceleratorManager import AcceleratorManager
from Britefury.UI.ComboBox import ComboBox
from Britefury.UI.NameDialog import nameDialog
from Britefury.UI.FormLayout import FormLayout
from Britefury.UI.UIAction import UIAction, UIActionInstanceSignal

from Britefury.Event.QueuedEvent import queueEvent

from Britefury.ProceduralCore.GSSceneNode import GSSceneNode

from Britefury.CellEdit.ChoiceCellEditComboBox import ChoiceCellEditComboBox

from Britefury.View.ModelLayer import ModelLayer, ModelLayerTable

from Britefury.MainApp import GeneralActions




_closeAction = UIAction( 'ModelLayerWindow.Close', ( _( 'Model layer window' ), _( 'Close' ) ), accelerator='Escape', tooltip=_( 'Close model layer window' ) )



class ModelLayerTableView (object):
	COLUMN_ITEM = 0
	COLUMN_NAME = 1
	COLUMN_VISIBLE = 2

	selectionSignal = ClassSignal()

	def __init__(self, commandHistory):
		super( ModelLayerTableView, self ).__init__()

		self._layerTable = None
		self._commandHistory = commandHistory

		self._store = gtk.TreeStore( object, str, bool )
		self._sorted = gtk.TreeModelSort( self._store )
		self._sorted.set_sort_column_id( ModelLayerTableView.COLUMN_NAME, gtk.SORT_ASCENDING )
		self._treeView = gtk.TreeView( self._sorted )
		self._selection = self._treeView.get_selection()
		self._selection.set_mode( gtk.SELECTION_MULTIPLE )
		self._selection.connect( 'changed', self._p_onSelectionChanged )

		nameCellRenderer = gtk.CellRendererText()
		nameCellRenderer.set_property( 'editable', True )
		nameCellRenderer.connect( 'edited', self._p_onNameEdited )
		nameColumn = gtk.TreeViewColumn( _( 'Name' ), nameCellRenderer, text=ModelLayerTableView.COLUMN_NAME )
		nameColumn.set_expand( True )
		self._treeView.append_column( nameColumn )

		visibleCellRenderer = gtk.CellRendererToggle()
		visibleCellRenderer.set_property( 'activatable', True )
		visibleCellRenderer.connect( 'toggled', self._p_onVisibleToggled )
		visibleColumn = gtk.TreeViewColumn( _( 'Vs.' ), visibleCellRenderer, active=ModelLayerTableView.COLUMN_VISIBLE )
		self._treeView.append_column( visibleColumn )

		self._sceneNodeToListeners = {}
		self._layerToListeners = {}
		self.selectedItems = []
		self.selectedLayers = []
		self.selectedNodes = []



	def getWidget(self):
		return self._treeView


	def attachLayerTable(self, layers):
		assert self._layerTable is None, 'model layer table already attached'
		self._layerTable = layers
		self._layerTable.layerAppendedSignal.connect( self._p_onLayerAppended )
		self._layerTable.removeLayerSignal.connect( self._p_onRemoveLayer )
		self._layerTable.layerInsertedSignal.connect( self._p_onLayerInserted )
		self._layerTable.clearLayersSignal.connect( self._p_onClearLayers )
		for layer in self._layerTable:
			self._p_onLayerAppended( self._layerTable, layer  )



	def detachLayerTable(self):
		assert self._layerTable is not None, 'no model layer table attached'
		self._p_onClearLayers( self._layerTable )
		self._layerTable.layerAppendedSignal.disconnect( self._p_onLayerAppended )
		self._layerTable.removeLayerSignal.disconnect( self._p_onRemoveLayer )
		self._layerTable.layerInsertedSignal.disconnect( self._p_onLayerInserted )
		self._layerTable.clearLayersSignal.disconnect( self._p_onClearLayers )
		self._layerTable = None


	def _p_onLayerAppended(self, layerTable, layer):
		self._store.append( None, self._p_getRowDataForLayer( layer ) )
		self._p_attachLayer( layer )

	def _p_onRemoveLayer(self, layerTable, layer):
		self._p_detachLayer( layer )
		del self._store[ self._p_getRowPathForLayer( layer ) ]

	def _p_onLayerInserted(self, layerTable, index, layer):
		# gtk.TreeStore does not like inserting at the end, using insert_before(), so detect this case, and handle it using an append operation
		if index == len( layerTable ) - 1:
			self._store.append( None, self._p_getRowDataForLayer( layer ) )
		else:
			self._store.insert_before( None, self._store.get_iter( ( index ) ), self._p_getRowDataForLayer( layer ) )
		self._p_attachLayer( layer )

	def _p_onClearLayers(self, layerTable):
		for layer in reversed( self._layerTable ):
			self._p_onRemoveLayer( self._layerTable, layer )




	def _p_attachLayer(self, layer):
		def onNameChanged():
			def refreshName():
				try:
					rowPath = self._p_getRowPathForLayer( layer )
				except ValueError:
					pass
				else:
					self._store[rowPath][ModelLayerTableView.COLUMN_NAME] = layer.name
			queueEvent( refreshName )
		def onVisibilityChanged():
			def refreshVisibility():
				try:
					rowPath = self._p_getRowPathForLayer( layer )
				except ValueError:
					pass
				else:
					self._store[rowPath][ModelLayerTableView.COLUMN_VISIBLE] = layer.bVisible
			queueEvent( refreshVisibility )
		layer.cells.name.changedSignal.connect( onNameChanged )
		layer.cells.bVisible.changedSignal.connect( onVisibilityChanged )
		self._layerToListeners[layer] = ( onNameChanged, onVisibilityChanged )
		layer.nodeAppendedSignal.connect( self._p_onNodeAppended )
		layer.removeNodeSignal.connect( self._p_onRemoveNode )
		layer.nodeInsertedSignal.connect( self._p_onNodeInserted )
		layer.clearNodesSignal.connect( self._p_onClearNodes )
		for sceneNode in layer:
			self._p_onNodeAppended( layer, sceneNode )

	def _p_detachLayer(self, layer):
		self._p_onClearNodes( layer )
		layer.nodeAppendedSignal.disconnect( self._p_onNodeAppended )
		layer.removeNodeSignal.disconnect( self._p_onRemoveNode )
		layer.nodeInsertedSignal.disconnect( self._p_onNodeInserted )
		layer.clearNodesSignal.disconnect( self._p_onClearNodes )
		onNameChanged, onVisibleChanged = self._layerToListeners[layer]
		layer.cells.name.changedSignal.disconnect( onNameChanged )
		layer.cells.bVisible.changedSignal.disconnect( onVisibleChanged )
		del self._layerToListeners[layer]





	def _p_onNodeAppended(self, layer, sceneNode):
		layerPath = self._p_getRowPathForLayer( layer )
		self._store.append( self._store.get_iter( layerPath ), self._p_getRowDataForSceneNode( sceneNode ) )
		self._p_attachSceneNode( sceneNode )

	def _p_onRemoveNode(self, layer, sceneNode):
		self._p_detachSceneNode( sceneNode )
		del self._store[ self._p_getRowPathForSceneNode( sceneNode ) ]

	def _p_onNodeInserted(self, layer, index, sceneNode):
		layerIndex = self._layerTable.index( layer )
		# gtk.TreeStore does not like inserting at the end, using insert_before(), so detect this case, and handle it using an append operation
		if index == len( layer ) - 1:
			self._store.append( self._store.get_iter( ( layerIndex, ) ), self._p_getRowDataForSceneNode( sceneNode ) )
		else:
			self._store.insert_before( self._store.get_iter( ( layerIndex, ) ), self._store.get_iter( ( layerIndex, index ) ), self._p_getRowDataForSceneNode( sceneNode ) )
		self._p_attachSceneNode( sceneNode )

	def _p_onClearNodes(self, layer):
		for sceneNode in reversed( layer ):
			self._p_onRemoveNode( layer, sceneNode )



	def _p_attachSceneNode(self, sceneNode):
		def onNameChanged():
			def refreshName():
				try:
					rowPath = self._p_getRowPathForSceneNode( sceneNode )
				except ValueError:
					pass
				else:
					self._store[rowPath][ModelLayerTableView.COLUMN_NAME] = sceneNode.name
			queueEvent( refreshName )
		def onVisibilityChanged():
			def refreshVisibility():
				try:
					rowPath = self._p_getRowPathForSceneNode( sceneNode )
				except ValueError:
					pass
				else:
					self._store[rowPath][ModelLayerTableView.COLUMN_VISIBLE] = sceneNode.bVisible
			queueEvent( refreshVisibility )
		sceneNode.cells.name.changedSignal.connect( onNameChanged )
		sceneNode.cells.bVisible.changedSignal.connect( onVisibilityChanged )
		self._sceneNodeToListeners[sceneNode] = ( onNameChanged, onVisibilityChanged )

	def _p_detachSceneNode(self, sceneNode):
		onNameChanged, onVisibilityChanged = self._sceneNodeToListeners[sceneNode]
		sceneNode.cells.name.changedSignal.disconnect( onNameChanged )
		sceneNode.cells.bVisible.changedSignal.disconnect( onVisibilityChanged )
		del self._sceneNodeToListeners[sceneNode]




	def _p_getRowPathForLayer(self, layer):
		layerIndex = self._layerTable.index( layer )
		return ( layerIndex, )

	def _p_getRowPathForSceneNode(self, sceneNode):
		layer = self._layerTable.getLayerForNode( sceneNode )
		layerIndex = self._layerTable.index( layer )
		nodeIndex = layer.index( sceneNode )
		return layerIndex, nodeIndex


	def _p_getRowDataForLayer(self, layer):
		return ( layer, layer.name, layer.bVisible )

	def _p_getRowDataForSceneNode(self, sceneNode):
		return ( sceneNode, sceneNode.name, sceneNode.bVisible )


	def _p_getItem(self, path):
		if len( path ) == 1:
			return self._layerTable[ path[0] ]
		elif len( path ) == 2:
			return self._layerTable[ path[0] ][ path[1] ]
		else:
			raise ValueError, 'invalid path: %s' % ( path, )


	def _p_onSelectionChanged(self, selection):
		selectedPaths = selection.get_selected_rows()[1]
		self.selectedItems = [ self._sorted[path][ModelLayerTableView.COLUMN_ITEM]  for path in selectedPaths ]
		self.selectedLayers = [ item  for item in self.selectedItems  if isinstance( item, ModelLayer ) ]
		self.selectedNodes = [ item  for item in self.selectedItems  if isinstance( item, GSSceneNode ) ]
		self.selectionSignal.emit()


	def _p_onNameEdited(self, renderer, pathString, newText):
		item = self._sorted[pathString][ModelLayerTableView.COLUMN_ITEM]
		if isinstance( item, ModelLayer )  and  item is not self._layerTable.defaultLayer:
			item.name = newText
			self._commandHistory.finishCommand()


	def _p_onVisibleToggled(self, renderer, pathString):
		item = self._sorted[pathString][ModelLayerTableView.COLUMN_ITEM]
		item.bVisible = not item.bVisible
		self._commandHistory.finishCommand()







class ModelLayerEditorWindow (object):
	closeSignal = ClassSignal()

	def __init__(self, parentWindow, commandHistory):
		super( ModelLayerEditorWindow, self ).__init__()

		self._parentWindow = parentWindow
		self._commandHistory = commandHistory
		self._layerTable = None

		self._windowAcceleratorManager = AcceleratorManager()


		self._addLayerButton = gtk.Button( _( 'New' ) )
		self._addLayerButton.connect( 'clicked', self._p_onAddLayer )

		self._removeLayerButton = gtk.Button( _( 'Remove' ) )
		self._removeLayerButton.connect( 'clicked', self._p_onRemoveLayer )
		self._removeLayerButton.set_sensitive( False )

		self._renameLayerButton = gtk.Button( _( 'Rename' ) )
		self._renameLayerButton.connect( 'clicked', self._p_onRenameLayer )
		self._renameLayerButton.set_sensitive( False )

		self._removeNodeButton = gtk.Button( _( 'Remove' ) )
		self._removeNodeButton.connect( 'clicked', self._p_onRemoveNode )
		self._removeNodeButton.set_sensitive( False )

		self._moveNodeButton = gtk.Button( _( 'Move' ) )
		self._moveNodeButton.connect( 'clicked', self._p_onMoveNode )
		self._moveNodeButton.set_sensitive( False )

		closeButton = gtk.Button( stock=gtk.STOCK_CLOSE )
		closeButton.connect( 'clicked', self._p_onClose )
		closeButton.show()

		buttonLayout = FormLayout()
		buttonLayout.row()  <<  _( 'Layers' )
		buttonLayout.row()  <<  self._addLayerButton
		buttonLayout.row()  <<  self._removeLayerButton
		buttonLayout.row()  <<  self._renameLayerButton
		buttonLayout.row()
		buttonLayout.row()  <<  _( 'Nodes' )
		buttonLayout.row()  <<  self._removeNodeButton
		buttonLayout.row()  <<  self._moveNodeButton
		buttonLayout.finalise()
		buttonLayout.getWidget().show_all()

		buttonBox = gtk.VBox()
		buttonBox.pack_start( buttonLayout.getWidget(), expand=False, fill=False )
		buttonBox.pack_end( closeButton, expand=False, fill=False )
		buttonBox.show()

		self._layerTableView = ModelLayerTableView( self._commandHistory )
		self._layerTableView.selectionSignal.connect( self._p_onSelection )
		self._layerTableView.getWidget().show()

		self._viewScrolledWindow = gtk.ScrolledWindow()
		self._viewScrolledWindow.set_policy( gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC )
		self._viewScrolledWindow.add( self._layerTableView.getWidget() )
		self._viewScrolledWindow.show()

		windowBox = gtk.HBox( spacing=5 )
		windowBox.pack_start( buttonBox, expand=False, fill=False )
		windowBox.pack_start( self._viewScrolledWindow, expand=True, fill=True )
		windowBox.show()

		self._window = gtk.Window( gtk.WINDOW_TOPLEVEL )
		self._window.set_title( _( 'Layers' ) )
		self._window.set_default_size( 300, 400 )
		self._window.set_transient_for( self._parentWindow )
		self._window.add( windowBox )
		self._window.connect( 'delete-event', self._p_onWindowDelete )

		self._windowAcceleratorManager.attachToWindow( self._window )

		self._undoAction = GeneralActions.undoAction.instance( UIActionInstanceSignal )
		self._undoAction.actionSignal.connect( self._p_onUndoAction )
		self._undoAction.installAccel( self._windowAcceleratorManager )

		self._redoAction = GeneralActions.redoAction.instance( UIActionInstanceSignal )
		self._redoAction.actionSignal.connect( self._p_onRedoAction )
		self._redoAction.installAccel( self._windowAcceleratorManager )

		self._closeAction = _closeAction.instance( UIActionInstanceSignal )
		self._closeAction.actionSignal.connect( self._p_onCloseAction )
		self._closeAction.installAccel( self._windowAcceleratorManager )



	def getWidget(self):
		return self._window



	def attachLayerTable(self, layerTable):
		assert self._layerTable is None, 'layer table already attached'
		self._layerTableView.attachLayerTable( layerTable )
		self._layerTable = layerTable


	def detachLayerTable(self):
		assert self._layerTable is not None, 'no layer table attached'
		self._layerTableView.detachLayerTable()
		self._layerTable = None



	def show(self):
		self._window.show()

	def hide(self):
		self._window.hide()


	def getWindow(self):
		return self._window



	def _p_onWindowDelete(self, window, event):
		self.hide()
		self.closeSignal.emit()

		# Don't allow the window to be deleted
		return True


	def _p_onUndoAction(self, action):
		if self._commandHistory.canUndo():
			self._commandHistory.undo()


	def _p_onRedoAction(self, action):
		if self._commandHistory.canRedo():
			self._commandHistory.redo()


	def _p_onCloseAction(self, action):
		self.hide()
		self.closeSignal.emit()


	def _p_onAddLayer(self, widget):
		name = nameDialog( _( 'New Layer' ), _( 'Please enter a name for the new layer' ), _( 'Name:' ), parentWindow = self._window )
		if name is not None:
			self._commandHistory.freeze()
			layer = ModelLayer()
			layer.name = name
			self._layerTable.append( layer )
			self._commandHistory.thaw()

	def _p_onRemoveLayer(self, widget):
		self._commandHistory.freeze()
		for layer in self._layerTableView.selectedLayers:
			if layer is not self._layerTable.defaultLayer:
				self._layerTable.remove( layer )
		self._commandHistory.thaw()

	def _p_onRenameLayer(self, widget):
		if len( self._layerTableView.selectedLayers ) > 0:
			layer = self._layerTableView.selectedLayers[0]
			if layer is not self._layerTable.defaultLayer:
				name = nameDialog( _( 'Rename Layer' ), _( 'Please enter the new name for the layer' ), _( 'Name:' ), layer.name, parentWindow = self._window )
				if name is not None:
					self._commandHistory.freeze()
					layer.name = name
					self._commandHistory.thaw()


	def _p_onRemoveNode(self, widget):
		self._commandHistory.freeze()
		for sceneNode in self._layerTableView.selectedNodes:
			self._layerTable.removeSceneNode( sceneNode )
		self._commandHistory.thaw()

	def _p_onMoveNode(self, widget):
		menu = ModelLayerMenu( self._layerTable )
		menu.popup()
		menu.choiceSignal.connect( self._p_onMoveNodeMenuChoice )

	def _p_onMoveNodeMenuChoice(self, event, layer):
		self._commandHistory.freeze()
		for sceneNode in self._layerTableView.selectedNodes:
			self._layerTable.addSceneNodeToLayer( sceneNode, layer )
		self._commandHistory.thaw()


	def _p_onClose(self, event):
		self.hide()
		self.closeSignal.emit()


	def _p_onSelection(self):
		bRemoveLayerOk = len( self._layerTableView.selectedLayers ) > 0  and  ( len( self._layerTableView.selectedLayers ) > 1  or  self._layerTableView.selectedLayers[0] is not self._layerTable.defaultLayer )
		bRenameLayerOk = len( self._layerTableView.selectedLayers ) > 0  and  self._layerTableView.selectedLayers[0] is not self._layerTable.defaultLayer
		bNodesSelecged = len( self._layerTableView.selectedNodes ) > 0
		self._removeLayerButton.set_sensitive( bRemoveLayerOk )
		self._renameLayerButton.set_sensitive( bRenameLayerOk )
		self._removeNodeButton.set_sensitive( bNodesSelecged )
		self._moveNodeButton.set_sensitive( bNodesSelecged )






class ModelLayerMenu (UIController):
	choiceSignal = ClassSignal()

	def __init__(self, layerTable):
		super( ModelLayerMenu, self ).__init__()
		self._layerTable = layerTable
		self._menu = gtk.Menu()
		self._itemToLayer = {}

		for layer in layerTable:
			item = gtk.MenuItem( label=layer.name )
			item.show()
			item.connect( 'activate', self._p_onItemActivate )
			self._menu.append( item )
			self._itemToLayer[item] = layer


	def popup(self):
		self._menu.popup( None, None, None, 1, 0 )


	def _p_onItemActivate(self, item):
		layer = self._itemToLayer[item]
		if layer in self._layerTable:
			self.choiceSignal.emit( self.uiEvent(), layer )



class _ModelLayerCombo (object):
	def __init__(self):
		super( _ModelLayerCombo, self ).__init__()
		self._layerTable = None


	def attachLayerTable(self, layerTable):
		assert self._layerTable is None, 'layer table already attached'
		self._layerTable = layerTable
		self._layerTable.layerAppendedSignal.connect( self._p_onLayerAppended )
		self._layerTable.layerInsertedSignal.connect( self._p_onLayerInserted )
		self._layerTable.removeLayerSignal.connect( self._p_onRemoveLayer )
		self._layerTable.clearLayersSignal.connect( self._p_onClearLayers )
		if self._bAllowNone:
			self.append( None, _( '[None]' ) )
		for layer in self._layerTable:
			self.append( layer, layer.name )

	def detachLayerTable(self):
		assert self._layerTable is not None, 'no layer table attached'
		self.clear()
		self._layerTable.layerAppendedSignal.disconnect( self._p_onLayerAppended )
		self._layerTable.layerInsertedSignal.disconnect( self._p_onLayerInserted )
		self._layerTable.removeLayerSignal.disconnect( self._p_onRemoveLayer )
		self._layerTable.clearLayersSignal.disconnect( self._p_onClearLayers )
		self._layerTable = None


	def _p_onLayerAppended(self, layerTable, layer):
		self.append( layer, layer.name )

	def _p_onLayerInserted(self, layerTable, index, layer):
		self.insert( layer, index, layer.name )

	def _p_onRemoveLayer(self, layerTable, layer):
		self.remove( layer )

	def _p_onClearLayers(self, layerTable):
		self.clear()
		if self._bAllowNone:
			self.append( None, _( '[None]' ) )



class ModelLayerComboBox (ComboBox, _ModelLayerCombo):
	def __init__(self, bAllowNone=True):
		choices = []
		super( ModelLayerComboBox, self ).__init__( [ str ], choices )
		self._layerTable = None
		self._bAllowNone = bAllowNone


class ModelLayerCellEditComboBox (ChoiceCellEditComboBox, _ModelLayerCombo):
	def __init__(self, bAllowNone=True):
		choices = []
		super( ModelLayerCellEditComboBox, self ).__init__( [ str ], choices )
		self._layerTable = None
		self._bAllowNone = bAllowNone


