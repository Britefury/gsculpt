##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import traceback

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.FileIO.IOXml import ioObjectFactoryRegister

from Britefury.Cell.Cell import *

from Britefury.Sheet.Sheet import *

from Britefury.View import ModelLayerCommandTracker




class ModelLayer (Sheet):
	trackerClass = ModelLayerCommandTracker.ModelLayerCommandTracker

	name = Field( str, 'Layer' )
	bVisible = Field( bool, True )

	nodeAppendedSignal = ClassSignal()
	nodeInsertedSignal = ClassSignal()
	removeNodeSignal = ClassSignal()
	clearNodesSignal = ClassSignal()


	def __init__(self):
		super( ModelLayer, self ).__init__()
		self.sceneNodes = []
		self._layerTable = None

		self._visibleNodes = []
		self._nodeToListeners = {}

		self._nodesToRefresh = set()
		self._bVisibleListRequiresRefresh = False

		self._commandTracker_ = None



	def __len__(self):
		return len( self.sceneNodes )

	def __contains__(self, node):
		return node in self.sceneNodes

	def __iter__(self):
		return iter( self.sceneNodes )

	def __getitem__(self, index):
		return self.sceneNodes[index]

	def index(self, node):
		return self.sceneNodes.index( node )


	def append(self, node):
		self.sceneNodes.append( node )
		self._p_attachSceneNode( node )
		if self._layerTable is not None:
			self._layerTable._f_onLayerNodeAppended( self, node )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onLayerNodeAppended( self, node )
		self.nodeAppendedSignal.emit( self, node )

	def insert(self, index, node):
		self.sceneNodes.insert( index, node )
		self._p_attachSceneNode( node )
		if self._layerTable is not None:
			self._layerTable._f_onLayerNodeInserted( self, index, node )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onLayerNodeInserted( self, index, node )
		self.nodeInsertedSignal.emit( self, index, node )

	def remove(self, node):
		self.removeNodeSignal.emit( self, node )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onLayerRemoveNode( self, node )
		self._p_detachSceneNode( node )
		if self._layerTable is not None:
			self._layerTable._f_onLayerRemoveNode( self, node )
		self.sceneNodes.remove( node )

	def clear(self):
		self.clearNodesSignal.emit( self )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onLayerClear( self )
		self._f_clear()


	def _f_clear(self):
		for node in self.sceneNodes:
			self._p_detachSceneNode( node )
		if self._layerTable is not None:
			self._layerTable._f_onLayerClearNodes( self )
		self.sceneNodes = []


	def _f_commandTrackerAppend(self, node):
		self._layerTable.removeSceneNode( node )
		self.append( node )

	def _f_commandTrackerInsert(self, index, node):
		self._layerTable.removeSceneNode( node )
		self.insert( index, node )

	def _f_commandTrackerRemove(self, node):
		assert node in self.sceneNodes
		self.remove( node )


	def _p_attachSceneNode(self, sceneNode):
		def onVisibleChanged():
			if sceneNode in self._visibleNodes:
				self._visibleNodes.remove( sceneNode )
			self._bVisibleListRequiresRefresh = True
			self._nodesToRefresh.add( sceneNode )
			if self._layerTable is not None:
				self._layerTable._f_onVisibleNodesChanged()
		sceneNode.cells.bVisible.changedSignal.connect( onVisibleChanged )
		self._nodeToListeners[sceneNode] = ( onVisibleChanged, )
		if sceneNode.bVisible:
			self._visibleNodes.append( sceneNode )
			if self._layerTable is not None:
				self._layerTable._f_onVisibleNodesChanged()

	def _p_detachSceneNode(self, sceneNode):
		if sceneNode in self._visibleNodes:
			self._visibleNodes.remove( sceneNode )
			if self._layerTable is not None:
				self._layerTable._f_onVisibleNodesChanged()
		onVisibleChanged = self._nodeToListeners[sceneNode][0]
		sceneNode.cells.bVisible.changedSignal.disconnect( onVisibleChanged )



	def _f_getVisibleNodes(self):
		if self._bVisibleListRequiresRefresh:
			for sceneNode in self._nodesToRefresh:
				if sceneNode.bVisible:
					self._visibleNodes.append( sceneNode )
			self._nodesToRefresh = set()
			self._bVisibleListRequiresRefresh = False
		return self._visibleNodes






	def __readxml__(self, xmlNode):
		super( ModelLayer, self ).__readxml__( xmlNode )

		self.clear()

		nodeListXml = xmlNode.getChild( 'scene_nodes' )
		if nodeListXml.isValid():
			for nodeXml in nodeListXml.childrenNamed( 'node' ):
				if nodeXml.isValid():
					self.append( nodeXml.readObject() )


	def __writexml__(self, xmlNode):
		super( ModelLayer, self ).__writexml__( xmlNode )

		nodeListXml = xmlNode.addChild( 'scene_nodes' )
		if nodeListXml.isValid():
			for node in self.sceneNodes:
				nodeListXml.addChild( 'node' ).writeObject( node )







class ModelLayerCell (RefCell):
	valueClass = ModelLayer
	bAllowNone = True





class ModelLayerTable (object):
	trackerClass = ModelLayerCommandTracker.ModelLayerTableCommandTracker

	visibleNodesSignal = ClassSignal()

	layerAppendedSignal = ClassSignal()
	layerInsertedSignal = ClassSignal()
	removeLayerSignal = ClassSignal()
	clearLayersSignal = ClassSignal()


	def __init__(self, scene):
		super( ModelLayerTable, self ).__init__()
		self._scene = scene
		self._scene.nodeAddedSignal.connect( self._p_onSceneNodeAdded )
		self._scene.removeNodeSignal.connect( self._p_onSceneRemoveNode )
		self.layers = []
		self._nodeToLayer = {}

		self._visibleNodes = None

		self._commandTracker_ = None

		self.defaultLayer = ModelLayer()
		self.defaultLayer.name = 'Default'
		self.append( self.defaultLayer )

		self._newNodeLayer = ModelLayerCell()
		self._newNodeLayer.literalValue = self.defaultLayer



	def addSceneNodeToLayer(self, sceneNode, layer):
		try:
			oldLayer = self._nodeToLayer[sceneNode]
		except KeyError:
			pass
		else:
			oldLayer.remove( sceneNode )

		layer.append( sceneNode )


	def removeSceneNode(self, sceneNode):
		try:
			layer = self._nodeToLayer[sceneNode]
		except KeyError:
			pass
		else:
			layer.remove( sceneNode )


	def getLayerForNode(self, sceneNode):
		try:
			return self._nodeToLayer[sceneNode]
		except KeyError:
			raise ValueError, 'node is not in this layer table'



	def __len__(self):
		return len( self.layers )

	def __contains__(self, layer):
		return layer in self.layers

	def __iter__(self):
		return iter( self.layers )

	def __getitem__(self, index):
		return self.layers[index]

	def index(self, layer):
		return self.layers.index( layer )


	def append(self, layer):
		self.layers.append( layer )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onLayerTableLayerAppended( self, layer )
		self.layerAppendedSignal.emit( self, layer )
		self._p_attachLayer( layer )

	def insert(self, index, layer):
		self.layers.insert( index, layer )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onLayerTableLayerInserted( self, index, layer )
		self.layerInsertedSignal.emit( self, index, layer )
		self._p_attachLayer( layer )

	def remove(self, layer):
		self.removeLayerSignal.emit( self, layer )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onLayerTableLayerRemoveed( self, layer )
		self.layers.remove( layer )
		self._p_detachLayer( layer )

	def clear(self):
		self._p_clear()
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onLayerTableCleared( self )
		self.defaultLayer = ModelLayer()
		self.defaultLayer.name = 'Default'
		self.append( self.defaultLayer )



	def _p_attachLayer(self, layer):
		layer._layerTable = self
		layer.cells.bVisible.changedSignal.connect( self._f_onVisibleNodesChanged )
		for sceneNode in layer:
			self._nodeToLayer[sceneNode] = layer

	def _p_detachLayer(self, layer):
		for sceneNode in layer:
			del self._nodeToLayer[sceneNode]
		layer._layerTable = None
		layer.cells.bVisible.changedSignal.disconnect( self._f_onVisibleNodesChanged )



	def _f_onLayerNodeAppended(self, layer, sceneNode):
		self._nodeToLayer[sceneNode] = layer

	def _f_onLayerNodeInserted(self, layer, index, sceneNode):
		self._nodeToLayer[sceneNode] = layer

	def _f_onLayerRemoveNode(self, layer, sceneNode):
		del self._nodeToLayer[sceneNode]

	def _f_onLayerClearNodes(self, layer):
		for sceneNode in layer:
			del self._nodeToLayer[sceneNode]


	def _f_onVisibleNodesChanged(self):
		self._visibleNodes = None
		self.visibleNodesSignal.emit( self )



	def _f_getVisibleNodes(self):
		if self._visibleNodes is None:
			# Refresh
			self._visibleNodes = []
			for layer in self.layers:
				if layer.bVisible:
					self._visibleNodes += layer._f_getVisibleNodes()
		return self._visibleNodes






	def _p_clear(self):
		for layer in self.layers:
			self._p_detachLayer( layer )
			layer._f_clear()
		self.clearLayersSignal.emit( self )
		self.layers = []
		self._nodeToLayer = {}
		self.defaultLayer = None


	def __readxml__(self, xmlNode):
		self._p_clear()

		layerListXml = xmlNode.getChild( 'layers' )
		if layerListXml.isValid():
			for layerXml in layerListXml.childrenNamed( 'layer' ):
				if layerXml.isValid():
					layer = ModelLayer()
					layerXml  >>  layer
					self.append( layer )
		self.defaultLayer = xmlNode.getChild( 'default_layer' ).readObject()

		if self.defaultLayer is None:
			if len( self.layers ) == 0:
				self.defaultLayer = ModelLayer()
				self.defaultLayer.name = 'Default'
				self.append( self.defaultLayer )
			else:
				self.defaultLayer == self.layers[0]

		self._newNodeLayer.literalValue = xmlNode.getChild( 'new_node_layer' ).readObject()



	def __writexml__(self, xmlNode):
		layerListXml = xmlNode.addChild( 'layers' )
		if layerListXml.isValid():
			for layer in self.layers:
				layerListXml.addChild( 'layer' )  <<  layer
		xmlNode.addChild( 'default_layer' ).writeObject( self.defaultLayer )
		xmlNode.addChild( 'new_node_layer' ).writeObject( self._newNodeLayer.value )



	def _p_onSceneNodeAdded(self, scene, sceneNode):
		if self._newNodeLayer.value is not None:
			self._newNodeLayer.value.append( sceneNode )

	def _p_onSceneRemoveNode(self, scene, sceneNode):
		self.removeSceneNode( sceneNode )



	def _p_getNewNodeLayer(self):
		return self._newNodeLayer

	newNodeLayer = property( _p_getNewNodeLayer )


ioObjectFactoryRegister( 'ModelLayerTable', ModelLayerTable )


