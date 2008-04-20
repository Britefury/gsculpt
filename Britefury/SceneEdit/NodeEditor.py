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

from Britefury.Util.SignalSlot import blockSlot, unblockSlot

from Britefury.Event.QueuedEvent import queueEvent

from Britefury.UI.UIAction import UIAction, UIActionInstanceSignal
from Britefury.UI.FormLayout import FormLayout
from Britefury.UI.ConfirmDialog import confirmDialog

from Britefury.Cell.Cell import *
from Britefury.Cell.CellEvaluatorWithExternalDependencies import CellEvaluatorWithExternalDependencies

from Britefury.ProceduralTool.GSSceneNodeToolbox import GSSceneNodeToolbox
from Britefury.ProceduralTool.GSFunctionTool import getFunctionToolClass

from Britefury.Painter import Painter
from Britefury.Painter.RequestPainter import RequestPainterObjectSource
from Britefury.Painter.RequestViewportPainter import RequestViewportPainter

from Britefury.SceneEdit.NodePlanAdapter import NodePlanAdapter
from Britefury.SceneEdit.NodePlanViewList import NodePlanViewList
from Britefury.View.FocusTool import FocusTool




def _moveUpButtonContents():
	return gtk.Arrow( gtk.ARROW_UP, gtk.SHADOW_OUT )

def _moveDownButtonContents():
	return gtk.Arrow( gtk.ARROW_DOWN, gtk.SHADOW_OUT )

_modifyAction = UIAction( 'Plan.ModifyAction', ( _( 'Plan' ), _( 'Modify Action' ) ), buttonContents=_( 'Modify' ), accelerator='<control>m', tooltip=_( 'Modify selected action' ) )
_removeAction = UIAction( 'Plan.RemoveAction', ( _( 'Plan' ), _( 'Remove Action' ) ), buttonContents=_( 'Remove' ), accelerator='<control>Delete', tooltip=_( 'Remove selected action' ) )
_moveUpAction = UIAction( 'Plan.MoveActionUp', ( _( 'Plan' ), _( 'Move action up' ) ), buttonContents=_moveUpButtonContents, tooltip=_( 'Move action up' ) )
_moveDownAction = UIAction( 'Plan.MoveActionDown', ( _( 'Plan' ), _( 'Move action down' ) ), buttonContents=_moveDownButtonContents, tooltip=_( 'Move action down' ) )






class NodeViewCellEvaluator (CellEvaluatorWithExternalDependencies):
	def __init__(self, ownerCell):
		super( NodeViewCellEvaluator, self ).__init__( ownerCell )
		self._currentFunction = None
		self._planAdapter = None




	def attachPlanAdapter(self, planAdapter):
		assert self._planAdapter is None, 'plan adapter already attached'
		self._planAdapter = planAdapter
		self._planAdapter.nodeAttachSignal.connect( self._p_onNodeAttachment )
		self._planAdapter.nodeDetachSignal.connect( self._p_onNodeAttachment )
		self._planAdapter.invalidateProductSignal.connect( self._p_onInvalidateProduct )

		if self._planAdapter.node is not None:
			self._p_onNodeAttachment( self._planAdapter.node )



	def detachPlanAdapter(self):
		assert self._planAdapter is not None, 'no plan adapter attached'
		self._planAdapter.nodeAttachSignal.disconnect( self._p_onNodeAttachment )
		self._planAdapter.nodeDetachSignal.disconnect( self._p_onNodeAttachment )
		self._planAdapter.invalidateProductSignal.disconnect( self._p_onInvalidateProduct )
		self._planAdapter = None

		self._p_onNodeAttachment( None )




	def evaluate(self, cell):
		unblockSlot( self._p_onInvalidateProduct )

		result = None

		if self._planAdapter is not None:
			if self._planAdapter.isValid():
				result = self._planAdapter.computePartialProduct( self._currentFunction )

		return result



	def _p_onNodeAttachment(self, node):
		self._o_changed()


	def _p_onInvalidateProduct(self, index):
		if self._currentFunction is not None:
			funcIndex = self._planAdapter.index( self._currentFunction )

			if index <= funcIndex:
				blockSlot( self._p_onInvalidateProduct )
				self._o_changed()



	def _p_setCurrentFunction(self, function):
		if function is not self._currentFunction:
			self._currentFunction = function

			self._o_changed()


	currentFunction = property( None, _p_setCurrentFunction )









class NodeView (RequestPainterObjectSource):
	def __init__(self, view, painterFactory):
		self._currentFunction = None
		self._viewCell = Cell()
		self._viewCellEvaluator = NodeViewCellEvaluator( self._viewCell )
		self._viewCell.evaluator = self._viewCellEvaluator
		self._viewCell.changedSignal.connect( self._p_onViewCellValueChanged )
		self._view = view
		self._painter = RequestViewportPainter( painterFactory, False )
		self._painter.painterObjectSource = self
		self._sceneNode = None
		self._bObjectViewEnabled = False



	def attachPlanAdapter(self, planAdapter):
		self._viewCellEvaluator.attachPlanAdapter( planAdapter )


	def detachPlanAdapter(self):
		self._viewCellEvaluator.detachPlanAdapter()
		self.disableObjectView()
		self._currentFunction = None



	def attachSceneNode(self, sceneNode):
		assert self._sceneNode is None, 'scene node already attached'
		self._sceneNode = sceneNode
		self._sceneNode.nodeViewSettingsModifiedDelayedSignal.connect( self._p_onNodeViewSettingsModified )
		for nodeViewSettings in self._sceneNode.getNodeViewSettings():
			nodeViewSettings.applyToViewSettings( self._view.viewSettings )


	def detachSceneNode(self):
		assert self._sceneNode is not None, 'no scene node attached'
		self._sceneNode.nodeViewSettingsModifiedDelayedSignal.disconnect( self._p_onNodeViewSettingsModified )
		self._sceneNode = None



	def enableObjectView(self):
		if not self._bObjectViewEnabled:
			if self._currentFunction is not None:
				self._painter.addToView( self._view )
				self._view.postRedraw()

			self._bObjectViewEnabled = True


	def disableObjectView(self):
		if self._bObjectViewEnabled:
			if self._currentFunction is not None:
				self._painter.removeFromView( self._view )
				self._view.postRedraw()

			self._bObjectViewEnabled = False



	def requestObjectToPaint(self):
		return self._viewCell.getImmutableValue()



	def _p_getViewCell(self):
		return self._viewCell


	def _p_setCurrentFunction(self, function):
		self._viewCellEvaluator.currentFunction = function

		if function is not self._currentFunction:
			if self._bObjectViewEnabled:
				if self._currentFunction is None  and  function is not None:
					self._painter.addToView( self._view )
				elif self._currentFunction is not None  and  function is None:
					self._painter.removeFromView( self._view )

			self._currentFunction = function
			self._view.postRedraw()


	def _p_getDisplayedProduct(self):
		return self.requestObjectToPaint()


	def _p_onViewCellValueChanged(self):
		self._view.postRedraw()


	def _p_onNodeViewSettingsModified(self, sceneNode):
		for nodeViewSettings in self._sceneNode.getNodeViewSettings():
			nodeViewSettings.applyToViewSettings( self._view.viewSettings )
		self._view.postRedraw()



	currentFunction = property( None, _p_setCurrentFunction )
	viewCell = property( _p_getViewCell )
	displayedProduct = property( _p_getDisplayedProduct )








class NodeToolboxManager (object):
	def __init__(self, view, toolboxTable, nodeEditor, toolPanel, editorSettings, topLevelWindow, commandHistory):
		self._view = view
		self._currentFunction = None
		self._nodeEditor = nodeEditor
		self._toolPanel = toolPanel
		self._editorSettings = editorSettings
		self._topLevelWindow = topLevelWindow
		self._commandHistory = commandHistory
		self._productCell = None
		self._sceneNode = None

		self._planAdapter = None
		self._toolboxTable = toolboxTable
		self._currentToolbox = None
		self._productClass = None




	def attachPlanAdapter(self, planAdapter):
		assert self._planAdapter is None, 'plan adapter already attached'

		self._planAdapter = planAdapter
		self._planAdapter.invalidateProductSignal.connect( self._p_onInvalidateProduct )

		self._p_queueRefresh()



	def detachPlanAdapter(self):
		assert self._planAdapter is not None, 'no plan adapter attached'

		self._p_uninstallCurrentToolbox()

		self._planAdapter.invalidateProductSignal.disconnect( self._p_onInvalidateProduct )

		self._planAdapter = None
		self._currentFunction = None

		self._productClass = None



	def attachProductCell(self, source):
		assert self._productCell is None, 'product cell already attached'

		self._productCell = source

		if self._currentToolbox is not None:
			self._currentToolbox.attachProductCell( source )


	def detachProductCell(self):
		if self._productCell is not None:
			if self._currentToolbox is not None:
				self._currentToolbox.detachProductCell()

			self._productCell = None



	def attachSceneNode(self, sceneNode):
		assert self._sceneNode is None, 'scene node already attached'

		self._sceneNode = sceneNode

		if self._currentToolbox is not None:
			self._currentToolbox.attachSceneNode( self._sceneNode )


	def detachSceneNode(self):
		if self._sceneNode is not None:
			if self._currentToolbox is not None:
				self._currentToolbox.detachSceneNode()

			self._sceneNode = None



	def _p_setCurrentFunction(self, function):
		assert self._planAdapter is not None, 'no plan adapter'

		if function is not self._currentFunction:
			if self._currentFunction is not None  and  function is None:
				self._p_uninstallCurrentToolbox()
				self._productClass = None
			else:
				self._p_queueRefresh()

			self._currentFunction = function



	def _p_onRefresh(self):
		if self._planAdapter is not None:
			if self._currentFunction is not None:
				productClass = self._planAdapter.computePartialProductClass( self._currentFunction )

				if productClass is not self._productClass:
					self._p_uninstallCurrentToolbox()
					self._p_installNewToolbox( productClass )
					self._productClass = productClass

				unblockSlot( self._p_onInvalidateProduct )


	def _p_queueRefresh(self):
		queueEvent( self._p_onRefresh )


	def _p_installNewToolbox(self, productClass):
		toolbox = self._toolboxTable.takeToolbox( productClass )

		if toolbox is not None:
			assert isinstance( toolbox, GSSceneNodeToolbox ), 'all toolboxes must be instances of GSSceneNodeToolbox'

			toolbox.install( self._toolPanel, self._view )

			if self._productCell is not None:
				toolbox.attachProductCell( self._productCell )

			if self._sceneNode is not None:
				toolbox.attachSceneNode( self._sceneNode )

			if toolbox.dndHandler is not None:
				toolbox.dndHandler.install( self._view, Viewport3d, self._nodeEditor )

		self._currentToolbox = toolbox


	def _p_uninstallCurrentToolbox(self):
		if self._currentToolbox is not None:
			self._currentToolbox.uninstall( self._toolPanel, self._view )

			if self._currentToolbox.dndHandler is not None:
				self._currentToolbox.dndHandler.uninstall()

			if self._productCell is not None:
				self._currentToolbox.detachProductCell()

			if self._sceneNode is not None:
				self._currentToolbox.detachSceneNode()

			self._toolboxTable.returnToolbox( self._currentToolbox )

			self._currentToolbox = None


	def _p_onInvalidateProduct(self, index):
		if self._currentFunction is not None:
			functionIndex = self._planAdapter.index( self._currentFunction )

			if index <= functionIndex:
				self._p_queueRefresh()

				blockSlot( self._p_onInvalidateProduct )



	currentFunction = property( None, _p_setCurrentFunction )








class NodeEditor (object):
	NODE_EDITOR_WIDTH = 180

	def __init__(self, view, editorSettings, painterFactory, toolboxTable, toolPanel, topLevelWindow, commandHistory):
		self._nodeView = NodeView( view, painterFactory )
		self._toolboxManager = NodeToolboxManager( view, toolboxTable, self, toolPanel, editorSettings, topLevelWindow, commandHistory )
		self._view = view
		self._editorSettings = editorSettings
		self._painterFactory = painterFactory
		self._toolPanel = toolPanel
		self._topLevelWindow = topLevelWindow
		self._commandHistory = commandHistory
		self._currentTool = None

		self._planAdapter = NodePlanAdapter()
		self._node = None

		self._planView = NodePlanViewList()



		# Title label
		self._titleLabel = gtk.Label()
		self._titleLabel.set_size_request( NodeEditor.NODE_EDITOR_WIDTH, -1 )
		self._titleLabel.show()


		# Plan label
		planLabel = gtk.Label()
		planLabel.set_markup( _( '<b><span foreground="#000040">Plan</span></b>' ) )
		planLabel.show()


		# Plan view
		self._planView.getWidget().show()


		# Actions / buttons
		self._modifyAction = _modifyAction.instance( UIActionInstanceSignal )
		self._modifyAction.actionSignal.connect( self._p_onModify )

		self._removeAction = _removeAction.instance( UIActionInstanceSignal )
		self._removeAction.actionSignal.connect( self._p_onRemove )

		self._moveUpAction = _moveUpAction.instance( UIActionInstanceSignal )
		self._moveUpAction.actionSignal.connect( self._p_onMoveUp )

		self._moveDownAction = _moveDownAction.instance( UIActionInstanceSignal )
		self._moveDownAction.actionSignal.connect( self._p_onMoveDown )


		buttonLayout = FormLayout()
		buttonLayout.row()  <<  self._modifyAction.button()  <<  self._removeAction.button()  <<  self._moveUpAction.button()
		buttonLayout.row()  <<  FormLayout.space( 2 )  <<  self._moveDownAction.button()
		buttonLayout.finalise()
		buttonLayout.getWidget().show()


		# Pack into box
		self._editorBox = gtk.VBox()
		self._editorBox.pack_start( self._titleLabel, False, False, 3 )
		self._editorBox.pack_start( planLabel, False, False, 1 )
		self._editorBox.pack_start( self._planView.getWidget(), True, True, 0 )
		self._editorBox.pack_start( buttonLayout.getWidget(), False, False, 0 )





	def attachNode(self, node):
		assert self._node is None, 'node already attached'

		self._node = node
		self._planAdapter.node = node
		self._planAdapter.removeSignal.connect( self._p_onPlanRemove )

		self._node.bEditing = True

		self._planView.attachPlanAdapter( self._planAdapter )
		self._nodeView.attachPlanAdapter( self._planAdapter )
		self._toolboxManager.attachPlanAdapter( self._planAdapter )

		self._planView.selectionSignal.connect( self._p_onSelect )

		self._nodeView.enableObjectView()
		self._planView.selectFunction( self._planAdapter[-1] )

		self._toolboxManager.attachProductCell( self._nodeView.viewCell )

		self._toolboxManager.attachSceneNode( self._node )
		self._nodeView.attachSceneNode( self._node )

		self._titleLabel.set_markup( _( '<b><span foreground="#004000">Node: </span></b>' ) + self._node.name )

		self._node.cells.name.changedSignal.connect( self._p_onNodeNameChanged )



	def detachNode(self):
		assert self._node is not None, 'node already attached'

		self._node.cells.name.changedSignal.disconnect( self._p_onNodeNameChanged )

		self._p_uninstallCurrentTool()

		self._titleLabel.set_text( "" )

		self._nodeView.detachSceneNode()
		self._toolboxManager.detachSceneNode()

		self._toolboxManager.detachProductCell()

		self._planView.selectionSignal.disconnect( self._p_onSelect )

		self._planView.detachPlanAdapter()

		self._nodeView.disableObjectView()
		self._nodeView.detachPlanAdapter()

		self._toolboxManager.detachPlanAdapter()


		self._node.bEditing = False

		self._planAdapter.removeSignal.disconnect( self._p_onPlanRemove )

		self._node = None




	def getWidget(self):
		return self._editorBox




	def installAccelerators(self, acceleratorManager):
		self._modifyAction.installAccel( acceleratorManager )
		self._removeAction.installAccel( acceleratorManager )


	def uninstallAccelerators(self, acceleratorManager):
		self._modifyAction.installAccel( acceleratorManager )
		self._removeAction.installAccel( acceleratorManager )




	def addProcedureSelectAndEdit(self, proc):
		assert self._node is not None, 'no node attached'
		self.addProcedureAndSelect( proc )
		self.modifyCurrentSelection()
		return self._currentTool


	def addProcedureAndSelect(self, proc):
		assert self._node is not None, 'no node attached'
		if self._planView.selectedFunction is not None:
			self._planAdapter.insertAfter( self._planView.selectedFunction, proc )
		else:
			self._planAdapter.append( proc )

		self._planView.selectFunction( proc )


	def editCurrentFunction(self):
		assert self._node is not None, 'no node attached'
		self.modifyCurrentSelection()



	def modifyCurrentSelection(self):
		if self._planView.selectedFunction is not None  and  self._currentTool is None:
			toolClass = getFunctionToolClass( self._planView.selectedFunction )

			assert toolClass is not None, 'failed to create tool for function'

			self._currentTool = toolClass( self._view, self._editorSettings, self._topLevelWindow, self._commandHistory, self._painterFactory )
			self._currentTool.sheet = self._planView.selectedFunction

			assert self._currentTool is not None, 'failed to create tool for function'


			self._toolboxManager.detachProductCell()
			self._nodeView.disableObjectView()

			self._currentTool.install( self._toolPanel, self._topLevelWindow )
			self._currentTool.doneListener = self._p_onToolDone

			self._toolboxManager.attachProductCell( self._currentTool.outputProductCell )



	def _p_uninstallCurrentTool(self):
		if self._currentTool is not None:
			# The tool provides the object source that is used to generate the object to draw
			# The tool kit manager may need this during the detachProductCell() function.
			# Since uninstalling the tool would destroy this object source, detach it from the
			# tool kit manager first.
			self._toolboxManager.detachProductCell()

			self._currentTool.uninstall()
			self._currentTool.sheet = None

			self._nodeView.enableObjectView()
			self._toolboxManager.attachProductCell( self._nodeView.viewCell )

			self._currentTool.doneListener = None
			self._currentTool.shutdownTool()
			self._currentTool = None



	def _p_onPlanRemove(self, index, proc):
		assert self._node is not None, 'no node attached'
		if proc is self._planView.selectedFunction:
			self._planView.selectFunction( index - 1 )



	def _p_onModify(self, action):
		self.modifyCurrentSelection()



	def _p_onRemove(self, action):
		if self._planView.selectedFunction is not None  and  self._planView.selectedFunction is not self._node:
			self._planAdapter.remove( self._planView.selectedFunction )


	def _p_onMoveUp(self, action):
		if self._planView.selectedFunction is not None:
			funcIndex = self._planAdapter.index( self._planView.selectedFunction )

			if funcIndex >= 2:
				self._planAdapter.swap( funcIndex, funcIndex - 1 )


	def _p_onMoveDown(self, action):
		if self._planView.selectedFunction is not None:
			funcIndex = self._planAdapter.index( self._planView.selectedFunction )

			if funcIndex >= 1  and  funcIndex <  ( len( self._planAdapter ) - 1 ):
				self._planAdapter.swap( funcIndex, funcIndex + 1 )


	def _p_onSelect(self, function):
		self._nodeView.currentFunction = function
		self._toolboxManager.currentFunction = function

		self._p_uninstallCurrentTool()



	def _p_onToolDone(self, tool):
		assert tool is self._currentTool, 'received \'done\' notification from tool that is not the current tool'

		self._p_uninstallCurrentTool()



	def _p_onNodeNameChanged(self):
		queueEvent( self._p_refreshNodeName )


	def _p_refreshNodeName(self):
		if self._node is not None:
			self._titleLabel.set_markup( _( '<b><span foreground="#004000">Node: </span></b>' ) + self._node.name )
		else:
			self._titleLabel.set_markup( _( '<b><span foreground="#004000">No node</span></b>' ) )




	def _p_getCurrentFunction(self):
		assert self._node is not None, 'no node attached'
		return self._planView.selectedFunction

	def _p_setCurrentFunction(self, function):
		assert self._node is not None, 'no node attached'
		self._planView.selectFunction( function )


	def _p_getCommandHistory(self):
		return self._commandHistory


	def _p_getDisplayedProduct(self):
		if self._currentTool is not None:
			return self._currentTool.displayedProduct
		else:
			return self._nodeView.displayedProduct


	currentFunction = property( _p_getCurrentFunction, _p_setCurrentFunction )
	displayedProduct = property( _p_getDisplayedProduct )
	commandHistory = property( _p_getCommandHistory )


