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

from Britefury.Mesh.Mesh import GSProductMesh

from Britefury.SceneNodes.SceneGroupNode import SceneGroupNode

from Britefury.GroupTools.ToolGroupMergeContents import ProcGroupMergeContents





class MeshAttachTool (Tool, ViewportEventHandler, ViewportContextButtonListener):
	def __init__(self, sceneEditorTools, nodeEditor, scene, backgroundLayerPainter, view, parentWindow, commandHistory):
		super( MeshAttachTool, self ).__init__( view, parentWindow )
		self._sceneEditorTools = sceneEditorTools
		self._nodeEditor = nodeEditor
		self._originalCurrentNode = None
		self._originalCurrentFunction = None
		self._scene = scene
		self._scene.nodeAddedSignal.connect( self._p_onSceneNodeAdded )
		self._scene.removeNodeSignal.connect( self._p_onSceneRemoveNode )
		self._backgroundLayerPainter = backgroundLayerPainter
		self._view = view
		self._commandHistory = commandHistory
		self.setTitleWidgetText( _( 'Attach Mesh' ) )
		self._pageTitle = _( 'AttMsh' )

		layout = FormLayout()
		layout.row()
		layout.textBlock( _( 'Click on a mesh in a viewport to attach it to the existing mesh.\n'  'Use undo to remove a mesh.\n'			\
						'This will create a group and merge its contents. Your original meshes can be accessed through the Scene Graph.\n'  'Any actions that come after the current function will be moved into the new node.\n' ) )
		layout.row()
		layout.finalise()
		layout.getWidget().show()
		self._o_packContents( layout.getWidget() )

		self._group = None
		self._mergeProcedure = None



	def shutdownTool(self):
		super( MeshAttachTool, self ).shutdownTool()
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
		node, t, intersection, _clippedRay = self._backgroundLayerPainter.chooseNode( ray, False, productClass=GSProductMesh )		# HACK: get proper value for backface culling

		if node is not None:
			self._commandHistory.freeze()

			# Ensure the group has been created
			self._p_ensureGroupExists()

			# Add the node to the group
			self._group.models.append( node.out )

			# Hide the original node
			node.bVisible = False

			self._commandHistory.thaw()


	def evContextButton(self, viewport):
		self._o_done()



	def _p_ensureGroupExists(self):
		if self._group is None:
			# Take a copy of @self._sceneEditorTools.currentNode, as adding the group node will cause self._o_onSceneNodeAdded to be called, which will change @self._sceneEditorTools.currentNode
			self._originalCurrentNode = self._sceneEditorTools.currentNode
			self._originalCurrentFunction = self._nodeEditor.currentFunction
			currentNode = self._sceneEditorTools.currentNode
			currentFunction = self._nodeEditor.currentFunction

			group = SceneGroupNode()

			if currentNode is not None:
				group.name = currentNode.name
				currentNode.name = self._scene.getUnusedNumberedNodeName( currentNode.name + ' pre attach' )
				currentNode.bVisible = False
			else:
				group.name = self._scene.getUnusedNumberedNodeName( 'Attached' )

			# Add it to the scene
			self._scene.nodes.append( group )

			# Add a merge procedure to the group's plan
			self._mergeProcedure = ProcGroupMergeContents()
			group.plan.append( self._mergeProcedure )

			# Take the procedures after @current function
			if currentFunction is not None  and  currentFunction is not currentNode:
				index = currentNode.plan.index( currentFunction ) + 1
				procsToMove = currentNode.plan[index:]
				del currentNode.plan[index:]
				group.plan.extend( procsToMove )

			# Add the current node to the group
			if currentNode is not None:
				group.models.append( currentNode.out )

			self._group = group
			self._sceneEditorTools.currentNode = self._group
			self._nodeEditor.currentFunction = self._mergeProcedure





	def _p_onSceneNodeAdded(self, scene, node):
		if node is self._group:
			self._sceneEditorTools.currentNode = self._group
			self._nodeEditor.currentFunction = self._mergeProcedure

	def _p_onSceneRemoveNode(self, scene, node):
		if node is self._group:
			self._sceneEditorTools.currentNode = self._originalCurrentNode
			self._nodeEditor.currentFunction = self._originalCurrentFunction
			self._group = None
			self._mergeProcedure = None
			self._o_done()


