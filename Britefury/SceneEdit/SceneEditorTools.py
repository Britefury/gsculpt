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

from Britefury.Tool.ToolPanel import ToolPanelPage

from Britefury.UI.AcceleratorManager import AcceleratorManager
from Britefury.UI.UIAction import UIAction, UIActionInstanceSignal
from Britefury.UI.ConfirmDialog import confirmDialog
from Britefury.UI.NameDialog import nameDialog
from Britefury.UI.ToggleButton import ToggleButton
from Britefury.UI.FormLayout import FormLayout
from Britefury.UI.ButtonPopupMenu import ButtonPopupMenu

from Britefury.Event.QueuedEvent import queueEvent

from Britefury.GraphView.SheetGraphView import SheetGraphView, SheetGraphViewNode

from Britefury.View.ModelLayerEditor import ModelLayerMenu, ModelLayerCellEditComboBox

from Britefury.SceneEdit.NodeEditor import NodeEditor
from Britefury.SceneEdit.SelectNodeTool import SelectNodeTool

from Britefury.MainApp import GeneralActions





_selectModelAction = UIAction( 'General.SelectModel', ( _( 'General' ), _( 'Select model' ) ), accelerator='<ctrl>space', tooltip=_( 'Select model' ) )
_sceneAction = UIAction( 'General.ShowSceneGraph', ( _( 'General' ), _( 'Show scene graph' ) ), accelerator='<ctrl><shift>s', tooltip=_( 'Show scene graph window' ) )


_closeAction = UIAction( 'SceneGraph.Close', ( _( 'Scene graph window' ), _( 'Close' ) ), accelerator='Escape', tooltip=_( 'Close scene graph window' ) )





class SceneGraphViewNode (SheetGraphViewNode):
	def attachGraphNode(self, graphNode):
		super( SceneGraphViewNode, self ).attachGraphNode( graphNode )
		self.setText( graphNode.name, graphNode.name )
		self._graphNode.cells.name.changedSignal.connect( self._p_onNameChanged )

	def detachGraphNode(self):
		self.setText( '', '' )
		self._graphNode.cells.name.changedSignal.disconnect( self._p_onNameChanged )
		super( SceneGraphViewNode, self ).detachGraphNode()



	def _p_onNameChanged(self):
		queueEvent( self._p_refreshName )


	def _p_refreshName(self):
		name = self._graphNode.name
		self.setText( name, name )



class SceneGraphView (SheetGraphView):
	def attachGraph(self, sceneGraph):
		super( SceneGraphView, self ).attachGraph( sceneGraph, sceneGraph._graphViewDisplayTable )


	def _p_createViewNode(self):
		return SceneGraphViewNode( self )



class SceneEditorTools (ToolPanelPage):
	FOCUS_ORIGIN = 0
	ZOOM_1TO1 = 1
	ZOOM_EXTENTS_ALL = 2
	ZOOM_EXTENTS_SELECTED = 3


	newNodeSignal = ClassSignal()


	def __init__(self, toolPanel, painterFactory, view, backgroundLayerPainter, editorSettings, toolboxTable, acceleratorManager, parentWindow, commandHistory):
		super( SceneEditorTools, self ).__init__()

		self._view = view
		self._backgroundLayerPainter = backgroundLayerPainter
		self._acceleratorManager = acceleratorManager
		self._parentWindow = parentWindow
		self._commandHistory = commandHistory
		self._painterFactory = painterFactory
		self._layerTable = None
		self._scene = None
		self._currentNode = None
		self._bDeleteExistingLinks = False
		self._sceneWindowTooltips = gtk.Tooltips()
		self._sceneWindowAcceleratorManager = AcceleratorManager()
		self._selectNodeTool = SelectNodeTool( self, backgroundLayerPainter, view, parentWindow )
		self._selectNodeTool.doneListener = self._p_onSelectModelToolDone


		self._nodeEditor = NodeEditor( view, editorSettings, painterFactory, toolboxTable, toolPanel, parentWindow, commandHistory )
		self._nodeEditor.getWidget().show()

		self._graphView = SceneGraphView( self._p_createLink, self._p_eraseLink )
		self._graphView.show()


		tooltips = gtk.Tooltips()


		# Settings bar
		self._defaultLayerCombo = ModelLayerCellEditComboBox()

		# Zoom tool box
		self._zoomMenu = ButtonPopupMenu( gsImageFilename='graphview_zoom.png' )
		self._zoomMenu.addMenuItem( self.FOCUS_ORIGIN, gsImageFilename='graphview_origin.png' )
		self._zoomMenu.addMenuItem( self.ZOOM_1TO1, gsImageFilename='graphview_1to1.png' )
		self._zoomMenu.addMenuItem( self.ZOOM_EXTENTS_ALL, gsImageFilename='graphview_zoomextentsall.png' )
		self._zoomMenu.addMenuItem( self.ZOOM_EXTENTS_SELECTED, gsImageFilename='graphview_zoomextentsselected.png' )
		self._zoomMenu.menuSignal.connect( self._p_onZoomMenu )
		self._zoomMenu.getWidget().show()

		settingsBar = gtk.HBox()
		settingsBar.pack_start( gtk.Label( _( 'Default layer:' ) ), False, False, 5 )
		settingsBar.pack_start( self._defaultLayerCombo.comboBox.getWidget(), False, False )
		settingsBar.pack_start( gtk.VSeparator(), False, False, 10 )
		settingsBar.pack_start( self._zoomMenu.getWidget(), False, False )
		settingsBar.show_all()



		# Vertical tool box
		self._newNodeButton = gtk.Button( _( 'New' ) )
		self._newNodeButton.connect( 'clicked', self._p_onNew )

		self._editNodeButton = gtk.Button( _( 'Edit' ) )
		self._editNodeButton.connect( 'clicked', self._p_onEdit )

		self._renameNodeButton = gtk.Button( _( 'Rename' ) )
		self._renameNodeButton.connect( 'clicked', self._p_onRename )

		self._deleteNodeButton = gtk.Button( _( 'Delete' ) )
		self._deleteNodeButton.connect( 'clicked', self._p_onDelete )

		self._addNodeToLayerButton = gtk.Button( _( 'Add to' ) )
		self._addNodeToLayerButton.connect( 'clicked', self._p_onAddNodeToLayer )

		self._removeNodeFromLayerButton = gtk.Button( _( 'Remove from' ) )
		self._removeNodeFromLayerButton.connect( 'clicked', self._p_onRemoveNodeFromLayer )



		closeButton = gtk.Button( stock=gtk.STOCK_CLOSE )
		closeButton.connect( 'clicked', self._p_onClose )




		buttonLayout = FormLayout()
		buttonLayout.row()  <<  _( 'Nodes' )
		buttonLayout.row()  <<  self._newNodeButton
		buttonLayout.row()  <<  self._editNodeButton
		buttonLayout.row()  <<  self._renameNodeButton
		buttonLayout.row()  <<  self._deleteNodeButton
		buttonLayout.row()
		buttonLayout.row()  <<  _( 'Layers' )
		buttonLayout.row()  <<  self._addNodeToLayerButton
		buttonLayout.row()  <<  self._removeNodeFromLayerButton
		buttonLayout.finalise()
		buttonLayout.getWidget().show()

		sceneToolbox = gtk.VBox()
		sceneToolbox.pack_start( buttonLayout.getWidget(), False, False )
		sceneToolbox.pack_end( closeButton, False, False )
		sceneToolbox.show_all()


		sceneWindowTable = gtk.Table( 2, 2 )
		sceneWindowTable.attach( settingsBar, 1, 2, 0, 1, gtk.EXPAND | gtk.FILL, 0 )
		sceneWindowTable.attach( sceneToolbox, 0, 1, 1, 2, 0, gtk.EXPAND | gtk.FILL )
		sceneWindowTable.attach( self._graphView, 1, 2, 1, 2, gtk.EXPAND | gtk.FILL, gtk.EXPAND | gtk.FILL )
		sceneWindowTable.show()


		self._sceneWindow = gtk.Window( gtk.WINDOW_TOPLEVEL )
		self._sceneWindow.set_title( _( 'Scene' ) )
		self._sceneWindow.set_default_size( 800, 600 )
		self._sceneWindow.set_transient_for( self._parentWindow )
		self._sceneWindow.add( sceneWindowTable )
		self._sceneWindow.connect( 'delete-event', self._p_onWindowDelete )

		self._sceneWindowAcceleratorManager.attachToWindow( self._sceneWindow )


		self._undoAction = GeneralActions.undoAction.instance( UIActionInstanceSignal )
		self._undoAction.actionSignal.connect( self._p_onUndoAction )
		self._undoAction.installAccel( self._sceneWindowAcceleratorManager )

		self._redoAction = GeneralActions.redoAction.instance( UIActionInstanceSignal )
		self._redoAction.actionSignal.connect( self._p_onRedoAction )
		self._redoAction.installAccel( self._sceneWindowAcceleratorManager )

		self._closeAction = _closeAction.instance( UIActionInstanceSignal )
		self._closeAction.actionSignal.connect( self._p_onCloseAction )
		self._closeAction.installAccel( self._sceneWindowAcceleratorManager )




		# TODO: new object menu



		# TOOLPANEL CONTENTS

		self._selectModelButton = ToggleButton( _( 'Choose model' ) )
		self._selectModelButton.getWidget().show()
		self._selectModelButton.toggleSignal.connect( self._p_onSelectModelButton )
		tooltips.set_tip( self._selectModelButton.getWidget(), _( 'Choose model' ), _( 'Choose model' ) )

		self._selectModelAction = _selectModelAction.instance( UIActionInstanceSignal )
		self._selectModelAction.actionSignal.connect( self._p_onSelectModel )


		self._sceneButton = gtk.ToggleButton( _( 'Scene graph' ) )
		self._sceneButton.connect( 'clicked', self._p_onSceneButton )

		self._sceneAction = _sceneAction.instance( UIActionInstanceSignal )
		self._sceneAction.actionSignal.connect( self._p_onScene )


		self._sceneHBox = gtk.HBox( True )
		self._sceneHBox.pack_start( self._selectModelButton.getWidget(), True, True, 10 )
		self._sceneHBox.pack_start( self._sceneButton, True, True, 10 )


		self._verticalBox = gtk.VBox()
		self._verticalBox.pack_start( gtk.Label( _( '---Scene---' ) ), False, False, 8 )
		self._verticalBox.pack_start( self._sceneHBox, False, False )
		self._verticalBox.pack_start( gtk.HSeparator(), False, False, 15 )
		self._verticalBox.pack_start( self._nodeEditor.getWidget(), True, True )
		self._verticalBox.show_all()


		self._graphView.nodeDoubleClickedSignal.connect( self._p_onNodeDoubleClicked )
		self._graphView.dragNodeBeginSignal.connect( self._p_onDragNodeBegin )
		self._graphView.dragNodeEndSignal.connect( self._p_onDragNodeEnd )





	def attachScene(self, scene):
		assert self._scene is None, 'scene already attached'

		self._scene = scene
		self._layerTable = scene.layerTable
		self._scene.removeNodeSignal.connect( self._p_onRemoveSceneNode )
		self._graphView.attachGraph( self._scene )
		self._defaultLayerCombo.attachLayerTable( self._scene.layerTable )
		self._defaultLayerCombo.attachCell( self._scene.layerTable.newNodeLayer )


	def detachScene(self):
		assert self._scene is not None, 'no scene attached'

		if self._currentNode is not None:
			self._nodeEditor.detachNode()
		self._currentNode = None

		self._defaultLayerCombo.detachCell()
		self._defaultLayerCombo.detachLayerTable()
		self._graphView.detachGraph()
		self._scene.removeNodeSignal.disconnect( self._p_onRemoveSceneNode )

		self._layerTable = None
		self._scene = None



	def getWidget(self):
		return self._verticalBox



	def installAccelerators(self):
		self._nodeEditor.installAccelerators( self._acceleratorManager )
		self._sceneAction.installAccel( self._acceleratorManager )
		self._selectModelAction.installAccel( self._acceleratorManager )


	def uninstallAccelerators(self):
		self._nodeEditor.uninstallAccelerators( self._acceleratorManager )
		self._sceneAction.uninstallAccel( self._acceleratorManager )
		self._selectModelAction.uninstallAccel( self._acceleratorManager )



	def onPageSwitchIn(self):
		self.installAccelerators()


	def onDetachToolPanel(self, toolPanel):
		self.uninstallAccelerators()




	def _p_setCurrentNode(self, node):
		if node is not self._currentNode:
			if self._currentNode is not None:
				self._nodeEditor.detachNode()
				self._backgroundLayerPainter.showNode( self._currentNode )

			self._currentNode = node

			if self._currentNode is not None:
				self._nodeEditor.attachNode( self._currentNode )
				self._backgroundLayerPainter.hideNode( self._currentNode )
		self._backgroundLayerPainter.forceRefreshLayers()


	def _p_currentNodeDelete(self):
		self._p_setCurrentNode( None )



	def _p_onSelectModelButton(self, event, bState):
		if bState:
			self._selectNodeTool.install( self._toolPanel, self._parentWindow )
		else:
			self._selectNodeTool.uninstall()


	def _p_onSelectModel(self, action):
		self._selectModelButton.state = True


	def _p_onSelectModelToolDone(self, tool):
		tool.uninstall()
		self._selectModelButton.state = False


	def _p_onSceneButton(self, widget):
		bState = widget.get_active()

		if bState:
			x, y, modifiers = gtk.gdk.get_default_root_window().get_pointer()

			self._sceneWindow.move( x, y )
			self._sceneWindow.show()
		else:
			self._sceneWindow.hide()


	def _p_onScene(self, action):
		self._sceneButton.set_active( not self._sceneButton.get_active() )


	def _p_onWindowDelete(self, window, event):
		self._p_hideWindow()

		# Don't allow the window to be deleted
		return True


	def _p_hideWindow(self):
		self._sceneButton.set_active( False )



	def _p_onRemoveSceneNode(self, scene, sceneNode):
		if sceneNode is self._currentNode  and  self._currentNode is not None:
			self._p_currentNodeDelete()




	def _p_getSelectedSceneNode(self):
		currentGraphNode = self._graphView.selection.currentNode
		#return GSFunctionGraphNode.getFunctionGraphNodeForGSGraphNode( currentGraphNode )
		return currentGraphNode




	def _p_onNew(self, widget):
		self.newNodeSignal.emit()


	def _p_onEdit(self, widget):
		self._p_setCurrentNode( self._p_getSelectedSceneNode() )


	def _p_onRename(self, widget):
		nodeToRename = self._p_getSelectedSceneNode()

		if nodeToRename is not None:
			# TODO: i18n
			newName = nameDialog( _( 'Rename node' ), _( 'Rename the node currently called "%s"' ) % ( nodeToRename.name, ), _( 'Name:' ), nodeToRename.name, True, self._sceneWindow )
			if newName is not None:
				nodeToRename.name = newName


	def _p_onDelete(self, widget):
		if len( self._graphView.selection ) > 0:
			# TODO: i18n
			if confirmDialog( _( 'Confirm delete' ), _( 'Are you sure you wish to delete the %d selected node(s)?' ) % ( len( self._graphView.selection ), ),
							gtk.STOCK_YES, gtk.STOCK_NO, 'y', 'n', True, self._sceneWindow ):
				selectedNodes = list( self._graphView.selection )
				self._commandHistory.freeze()
				for sceneNode in selectedNodes:
					#sceneNode = GSFunctionGraphNode.getFunctionGraphNodeForGSGraphNode( graphNode )
					if sceneNode is self._currentNode:
						self._p_currentNodeDelete()
					self._scene.nodes.remove( sceneNode )
				self._commandHistory.thaw()


	def _p_onAddNodeToLayer(self, widget):
		menu = ModelLayerMenu( self._scene.layerTable )
		menu.choiceSignal.connect( self._p_onAddNodeToLayerChoice )
		menu.popup()


	def _p_onRemoveNodeFromLayer(self, widget):
		self._commandHistory.freeze()
		selectedNodes = list( self._graphView.selection )
		for sceneNode in selectedNodes:
			#sceneNode = GSFunctionGraphNode.getFunctionGraphNodeForGSGraphNode( graphNode )
			self._scene.layerTable.removeSceneNode( sceneNode )
		self._commandHistory.thaw()



	def _p_onAddNodeToLayerChoice(self, event, layer):
		self._commandHistory.freeze()
		selectedNodes = list( self._graphView.selection )
		for sceneNode in selectedNodes:
			#sceneNode = GSFunctionGraphNode.getFunctionGraphNodeForGSGraphNode( graphNode )
			self._scene.layerTable.addSceneNodeToLayer( sceneNode, layer )
		self._commandHistory.thaw()



	def _p_onZoomMenu(self, event, choice):
		if choice == self.FOCUS_ORIGIN:
			self._graphView.focusOnOrigin()
		elif choice == self.ZOOM_1TO1:
			self._graphView.zoom1To1()
		elif choice == self.ZOOM_EXTENTS_ALL:
			self._graphView.zoomExtentsAll()
		elif choice == self.ZOOM_EXTENTS_SELECTED:
			self._graphView.zoomExtentsSelected()



	def _p_onClose(self, widget):
		self._p_hideWindow()




	def _p_onUndoAction(self, action):
		if self._commandHistory.canUndo():
			self._commandHistory.undo()


	def _p_onRedoAction(self, action):
		if self._commandHistory.canRedo():
			self._commandHistory.redo()


	def _p_onCloseAction(self, action):
		self._p_hideWindow()





	def _p_onNodeDoubleClicked(self, graphNode):
		self._p_setCurrentNode( self._p_getSelectedSceneNode() )

	def _p_onDragNodeBegin(self):
		self._commandHistory.freeze()

	def _p_onDragNodeEnd(self):
		self._commandHistory.thaw()


	def _p_createLink(self, sourcePin, sinkPin):
		self._commandHistory.freeze()
		sinkPin.append( sourcePin )
		self._commandHistory.thaw()

	def _p_eraseLink(self, sourcePin, sinkPin):
		self._commandHistory.freeze()
		sourcePin.remove( sinkPin )
		self._commandHistory.thaw()




	def _p_getNodeEditor(self):
		return self._nodeEditor

	def _p_getCurrentNode(self):
		return self._currentNode

	def _p_getScene(self):
		return self._scene


	nodeEditor = property( _p_getNodeEditor )
	currentNode = property( _p_getCurrentNode, _p_setCurrentNode )
	scene = property( _p_getScene )



