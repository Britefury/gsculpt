##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO import IOXml

from Britefury.View.BackgroundImage import BackgroundImageList
from Britefury.View.ModelLayer import ModelLayerTable

from Britefury.SheetGraph.SheetGraph import *

from Britefury.GraphView.SheetGraphViewDisplayTable import SheetGraphViewDisplayTable

from Britefury.ProceduralCore.GSSceneNode import GSSceneNode
from Britefury.ProceduralCore import GSSceneCommandTracker





class GSScene (SheetGraph):
	"""Scene - a gSculpt scene
	Contains the list of scene nodes, the graph which controls their connections, the layer table, and the background images table"""
	trackerClass = GSSceneCommandTracker.GSSceneCommandTracker


	class _NameToNodeTable (object):
	 	def __init__(self, scene):
			super( GSScene._NameToNodeTable, self ).__init__()
			self._nameToNode = {}
			self._nodeToName = {}
			self._nodeToResponseFunction = {}
			self._scene = scene
			self._nodesRequiringRefresh = set()
			self._bRefreshRequired = False


		def __iter__(self):
			self._p_refresh()
			return iter( self._nameToNode )

		def __len__(self):
			self._p_refresh()
			return len( self._nameToNode )

		def __contains__(self, node):
			self._p_refresh()
			return node in self._nameToNode

		def __getitem__(self, key):
			self._p_refresh()
			return self._nameToNode[key]

		def keys(self):
			self._p_refresh()
			return self._nameToNode.keys()

		def values(self):
			self._p_refresh()
			return self._nameToNode.values()

		def items(self):
			self._p_refresh()
			return self._nameToNode.items()


		def _f_onAddNode(self, node):
			def onNodeNameChanged():
				try:
					oldName = self._nodeToName[node]
				except KeyError:
					# Not node <-> name not mapped; do nothing
					pass
				else:
					self._p_unmapNameNode( node, oldName )
				self._nodesRequiringRefresh.add( node )
				self._bRefreshRequired = True
			self._p_mapNameNode( node, node.name )
			node.cells.name.changedSignal.connect( onNodeNameChanged )
			self._nodeToResponseFunction[node] = onNodeNameChanged

		def _f_onRemoveNode(self, node):
			oldName = self._nodeToName[node]
			self._p_unmapNameNode( node, oldName )
			onNodeName = self._nodeToResponseFunction[node]
			node.cells.name.changedSignal.disconnect( onNodeName )
			del self._nodeToResponseFunction[node]



		def _p_mapNameNode(self, node, name):
			try:
				nodeList = self._nameToNode[name]
			except KeyError:
				nodeList = []
				self._nameToNode[name] = nodeList
			nodeList.append( node )
			self._nodeToName[node] = name

		def _p_unmapNameNode(self, node, name):
			nodeList = self._nameToNode[name]
			nodeList.remove( node )
			if len( nodeList ) == 0:
				del self._nameToNode[name]
			del self._nodeToName[node]


		def _p_refresh(self):
			if self._bRefreshRequired:
				for node in self._nodesRequiringRefresh:
					self._p_mapNameNode( node, node.name )
				self._nodesRequiringRefresh = set()
				self._bRefreshRequired = False




	def __init__(self):
		super( GSScene, self ).__init__()

		self._nodesByName = GSScene._NameToNodeTable( self )
		self._layerTable = None
		self._backgroundImages = BackgroundImageList()
		self._layerTable = ModelLayerTable( self )
		self._graphViewDisplayTable = SheetGraphViewDisplayTable()



	def getUnusedNumberedNodeName(self, nodeNamePrefix):
		i = 1
		name = '%s%03d'  %  ( nodeNamePrefix, i )
		while name in self.nodesByName:
			i += 1
			name = '%s%03d'  %  ( nodeNamePrefix, i )
		return name




	def __readxml__(self, xmlNode):
		super( GSScene, self ).__readxml__( xmlNode )

		xmlNode.getChild( 'background_images' )  >>  self._backgroundImages
		xmlNode.getChild( 'layer_table' )  >>  self._layerTable
		xmlNode.getChild( 'graph_view_display_table' )  >>  self._graphViewDisplayTable


	def __writexml__(self, xmlNode):
		super( GSScene, self ).__writexml__( xmlNode )

		xmlNode.addChild( 'background_images' )  <<  self._backgroundImages
		xmlNode.addChild( 'layer_table' )  <<  self._layerTable
		xmlNode.addChild( 'graph_view_display_table' )  <<  self._graphViewDisplayTable



	def _p_onNodeAppended(self, node):
		self._nodesByName._f_onAddNode( node )
		super( GSScene, self )._p_onNodeAppended( node )

	def _p_onNodeRemoved(self, node):
		super( GSScene, self )._p_onNodeRemoved( node )
		self._nodesByName._f_onRemoveNode( node )



	def _getNodesByName(self):
		return self._nodesByName

	def _getLayerTable(self):
		return self._layerTable

	def _getBackgroundImages(self):
		return self._backgroundImages


	nodesByName = property( _getNodesByName, doc='Name to node mapping' )
	layerTable = property( _getLayerTable, doc='Layer table' )
	backgroundImages = property( _getBackgroundImages, doc='Background image list' )


IOXml.ioObjectFactoryRegister( 'GSScene', GSScene )


