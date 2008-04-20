##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.FormLayout import FormLayout

from Britefury.Tool.Tool import Tool

from Britefury.WorkArea.Viewport import ViewportEventHandler, ViewportContextButtonListener
from Britefury.WorkArea.Viewport3d import Viewport3d

from Britefury.Model.Model import GSProductModel
from Britefury.SceneNodes.SceneGroupNode import SceneGroupNode





class CreateGroupTool (Tool, ViewportEventHandler, ViewportContextButtonListener):
	def __init__(self, sceneEditorTools, scene, backgroundLayerPainter, view, parentWindow, commandHistory):
		super( CreateGroupTool, self ).__init__( view, parentWindow )
		self._sceneEditorTools = sceneEditorTools
		self._originalCurrentNode = self._sceneEditorTools.currentNode
		self._scene = scene
		self._scene.nodeAddedSignal.connect( self._p_onSceneNodeAdded )
		self._scene.removeNodeSignal.connect( self._p_onSceneRemoveNode )
		self._backgroundLayerPainter = backgroundLayerPainter
		self._view = view
		self._commandHistory = commandHistory
		self.setTitleWidgetText( _( 'Create Group' ) )
		self._pageTitle = _( 'CreateGrp' )

		layout = FormLayout()
		layout.row()
		layout.textBlock( _( 'Click on a model in a viewport to add it to the group.\nUse undo to remove a model from the group.' ) )
		layout.row()
		layout.finalise()
		layout.getWidget().show()
		self._o_packContents( layout.getWidget() )

		self._group = None



	def shutdownTool(self):
		super( CreateGroupTool, self ).shutdownTool()
		self._scene.nodeAddedSignal.disconnect( self._p_onSceneNodeAdded )
		self._scene.removeNodeSignal.disconnect( self._p_onSceneRemoveNode )


	def _o_installEventHandlers(self):
		self._view.addEventHandler( self, Viewport3d )
		self._view.addContextButtonListener( self, Viewport3d )


	def _o_uninstallEventHandlers(self):
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removeContextButtonListener( self, Viewport3d )




	def evPrimaryButtonDown(self, viewport):
		ray = viewport.computeRay()
		node, t, intersection, _clippedRay = self._backgroundLayerPainter.chooseNode( ray, False )		# HACK: get proper value for backface culling

		if node is not None:
			self._commandHistory.freeze()

			# Ensure the group has been created
			self._p_ensureGroupExists()

			# Ensure that it is in the scene
			if self._group not in self._scene.nodes:
				self._scene.nodes.append( self._group )

			# Add the node to the group
			self._group.models.append( node.out )

			# Hide the original node
			node.bVisible = False

			self._commandHistory.thaw()


	def evContextButton(self, viewport):
		self._o_done()



	def _p_ensureGroupExists(self):
		if self._group is None:
			self._group = SceneGroupNode()
			nodeName = self._scene.getUnusedNumberedNodeName( 'Group' )
			self._group.name = nodeName


	def _p_onSceneNodeAdded(self, scene, node):
		if node is self._group:
			self._sceneEditorTools.currentNode = self._group

	def _p_onSceneRemoveNode(self, scene, node):
		if node is self._group:
			self._sceneEditorTools.currentNode = self._originalCurrentNode


