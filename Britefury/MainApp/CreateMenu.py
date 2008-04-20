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


from Britefury.SceneNodes.SceneModelNode import SceneModelNode

from Britefury.PrimitiveTools.ToolBoxPrimitive import ProcBoxPrimitive
from Britefury.PrimitiveTools.ToolPlanePrimitive import ProcPlanePrimitive
from Britefury.PrimitiveTools.ToolSpherePrimitive import ProcSpherePrimitive
from Britefury.PrimitiveTools.ToolCylinderPrimitive import ProcCylinderPrimitive
from Britefury.PrimitiveTools.ToolConePrimitive import ProcConePrimitive
from Britefury.PrimitiveTools.ToolTorusPrimitive import ProcTorusPrimitive
from Britefury.PrimitiveTools.ToolTubePrimitive import ProcTubePrimitive
from Britefury.MeshTools.MeshEdit.ToolMeshDrawFace import ProcMeshDrawFace
from Britefury.MeshTools.MeshEdit.ToolMeshDrawQuads import ProcMeshDrawQuads

from Britefury.GroupTools.CreateGroupTool import CreateGroupTool



class CreateMenu (object):
	def __init__(self, sceneEditor, toolPanel, backgroundLayerPainter, view, parentWindow, commandHistory):
		self._scene = None
		self._sceneEditor = sceneEditor
		self._toolPanel = toolPanel
		self._backgroundLayerPainter = backgroundLayerPainter
		self._view = view
		self._parentWindow = parentWindow
		self._commandHistory = commandHistory

		self.menu = gtk.Menu()
		self._itemToInitFunction = {}
		self._itemToPrefix = {}
		self._items = []

		self._p_addCreatePrimitiveItem( _( 'Box' ), _( 'Box' ), self._p_initSceneNodeBox )
		self._p_addCreatePrimitiveItem( _( 'Plane' ), _( 'Plane' ), self._p_initSceneNodePlane )
		self._p_addCreatePrimitiveItem( _( 'Sphere' ), _( 'Sphere' ), self._p_initSceneNodeSphere )
		self._p_addCreatePrimitiveItem( _( 'Cylinder' ), _( 'Cylinder' ), self._p_initSceneNodeCylinder )
		self._p_addCreatePrimitiveItem( _( 'Cone' ), _( 'Cone' ), self._p_initSceneNodeCone )
		self._p_addCreatePrimitiveItem( _( 'Torus' ), _( 'Torus' ), self._p_initSceneNodeTorus )
		self._p_addCreatePrimitiveItem( _( 'Tube' ), _( 'Tube' ), self._p_initSceneNodeTube )
		self._p_addCreatePrimitiveItem( _( 'Draw poly face' ), _( 'Mesh' ), self._p_initSceneNodeDrawFace )
		self._p_addCreatePrimitiveItem( _( 'Draw quad mesh' ), _( 'Quad mesh' ), self._p_initSceneNodeDrawQuadMesh )
		sepItem = gtk.SeparatorMenuItem()
		sepItem.show()
		self.menu.append( sepItem )
		self._p_addItem( _( 'Group' ), self._p_createGroup )



	def attachScene(self, scene):
		assert self._scene is None, 'scene already attached'
		self._scene = scene
		for item in self._items:
			item.set_sensitive( True )


	def detachScene(self):
		assert self._scene is not None, 'no scene attached'
		self._scene = None
		for item in self._items:
			item.set_sensitive( False )



	def _p_addItem(self, itemText, itemFunction):
		item = gtk.MenuItem( itemText )
		item.connect( 'activate', itemFunction )
		self.menu.append( item )
		self._items.append( item )
		if self._scene is None:
			item.set_sensitive( False )


	def _p_addCreatePrimitiveItem(self, itemText, nodeNamePrefix, initFunction):
		item = gtk.MenuItem( itemText )
		item.connect( 'activate', self._p_onCreatePrimitiveItem )
		self.menu.append( item )
		self._itemToInitFunction[item] = initFunction
		self._itemToPrefix[item] = nodeNamePrefix
		self._items.append( item )
		if self._scene is None:
			item.set_sensitive( False )


	def _p_onCreatePrimitiveItem(self, item):
		# Get the init function, and name prefix
		initFunction = self._itemToInitFunction[item]
		nodeNamePrefix = self._itemToPrefix[item]

		# Work out the node name
		name = self._scene.getUnusedNumberedNodeName( nodeNamePrefix )

		# Begin command
		self._commandHistory.freeze()

		# Create and name the node
		node = SceneModelNode()
		node.name = name

		# Initialise it
		initFunction( node )

		# Add the node to the scene
		self._scene.nodes.append( node )

		# Make it the current node
		self._sceneEditor.currentNode = node
		self._sceneEditor.nodeEditor.currentFunction = node.plan[-1]
		self._sceneEditor.nodeEditor.modifyCurrentSelection()

		# End command
		self._commandHistory.thaw()



	def _p_initSceneNodeBox(self, node):
		node.plan.append( ProcBoxPrimitive() )

	def _p_initSceneNodePlane(self, node):
		node.plan.append( ProcPlanePrimitive() )

	def _p_initSceneNodeSphere(self, node):
		node.plan.append( ProcSpherePrimitive() )

	def _p_initSceneNodeCylinder(self, node):
		node.plan.append( ProcCylinderPrimitive() )

	def _p_initSceneNodeCone(self, node):
		node.plan.append( ProcConePrimitive() )

	def _p_initSceneNodeTorus(self, node):
		node.plan.append( ProcTorusPrimitive() )

	def _p_initSceneNodeTube(self, node):
		node.plan.append( ProcTubePrimitive() )

	def _p_initSceneNodeDrawFace(self, node):
		node.plan.append( ProcMeshDrawFace() )

	def _p_initSceneNodeDrawQuadMesh(self, node):
		node.plan.append( ProcMeshDrawQuads() )



	def _p_createGroup(self, item):
		if self._scene is not None:
			groupTool = CreateGroupTool( self._sceneEditor, self._scene, self._backgroundLayerPainter, self._view, self._parentWindow, self._commandHistory )
			groupTool.install( self._toolPanel, self._parentWindow )
			groupTool.doneListener = self._p_onCreateGroupToolDone

	def _p_onCreateGroupToolDone(self, tool):
		tool.uninstall()
		tool.shutdownTool()
