##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy

from Britefury.CommandHistory import CommandHistory

from Britefury.CommandHistory.CommandTracker import CommandTracker

from Britefury.Sheet.SheetCommandTracker import SheetCommandTracker

import ModelLayer



class ModelLayerAppendSceneNodeCommand (CommandHistory.Command):
	def __init__(self, layer, sceneNode):
		super( ModelLayerAppendSceneNodeCommand, self ).__init__()
		self._layer = layer
		self._sceneNode = sceneNode


	def execute(self):
		self._layer._f_commandTrackerAppend( self._sceneNode )

	def unexecute(self):
		self._layer._f_commandTrackerRemove( self._sceneNode )



class ModelLayerInsertSceneNodeCommand (CommandHistory.Command):
	def __init__(self, layer, index, sceneNode):
		super( ModelLayerInsertSceneNodeCommand, self ).__init__()
		self._layer = layer
		self._index = index
		self._sceneNode = sceneNode


	def execute(self):
		self._layer._f_commandTrackerInsert( self._index, self._sceneNode )

	def unexecute(self):
		self._layer._f_commandTrackerRemove( self._sceneNode )



class ModelLayerRemoveSceneNodeCommand (CommandHistory.Command):
	def __init__(self, layer, index, sceneNode):
		super( ModelLayerRemoveSceneNodeCommand, self ).__init__()
		self._layer = layer
		self._index = index
		self._sceneNode = sceneNode


	def execute(self):
		self._layer._f_commandTrackerRemove( self._sceneNode )

	def unexecute(self):
		self._layer._f_commandTrackerInsert( self._index, self._sceneNode )



class ModelLayerClearSceneNodesCommand (CommandHistory.Command):
	def __init__(self, layer, sceneNodes):
		super( ModelLayerClearSceneNodesCommand, self ).__init__()
		self._layer = layer
		self._sceneNodes = sceneNodes


	def execute(self):
		self._layer.clear()

	def unexecute(self):
		for sceneNode in self._sceneNodes:
			self._layer._f_commandTrackerAppend( sceneNode )




class ModelLayerCommandTracker (SheetCommandTracker):
	def track(self, layer):
		super( ModelLayerCommandTracker, self ).track( layer )
		assert isinstance( layer, ModelLayer.ModelLayer )


	def stopTracking(self, layer):
		assert isinstance( layer, ModelLayer.ModelLayer )

		super( ModelLayerCommandTracker, self ).stopTracking( layer )



	def _f_onLayerNodeAppended(self, layer, sceneNode):
		self._commandHistory.addCommand( ModelLayerAppendSceneNodeCommand( layer, sceneNode ) )

	def _f_onLayerNodeInserted(self, layer, index, sceneNode):
		self._commandHistory.addCommand( ModelLayerInsertSceneNodeCommand( layer, index, sceneNode ) )

	def _f_onLayerRemoveNode(self, layer, sceneNode):
		index = layer.index( sceneNode )
		self._commandHistory.addCommand( ModelLayerRemoveSceneNodeCommand( layer, index, sceneNode ) )

	def _f_onLayerClear(self, layer):
		self._commandHistory.addCommand( ModelLayerClearSceneNodesCommand( layer, copy( layer.sceneNodes ) ) )







class ModelLayerTableAppendLayerCommand (CommandHistory.Command):
	def __init__(self, layerTable, layer):
		super( ModelLayerTableAppendLayerCommand, self ).__init__()
		self._layerTable = layerTable
		self._layer = layer


	def execute(self):
		self._layerTable.append( self._layer )

	def unexecute(self):
		self._layerTable.remove( self._layer )



class ModelLayerTableInsertLayerCommand (CommandHistory.Command):
	def __init__(self, layerTable, index, layer):
		super( ModelLayerTableInsertLayerCommand, self ).__init__()
		self._layerTable = layerTable
		self._index = index
		self._layer = layer


	def execute(self):
		self._layerTable.insert( self._index, self._layer )

	def unexecute(self):
		self._layerTable.remove( self._layer )



class ModelLayerTableRemoveLayerCommand (CommandHistory.Command):
	def __init__(self, layerTable, index, layer):
		super( ModelLayerTableRemoveLayerCommand, self ).__init__()
		self._layerTable = layerTable
		self._index = index
		self._layer = layer


	def execute(self):
		self._layerTable.remove( self._m_layer )

	def unexecute(self):
		self._layerTable.insert( self._index, self._layer )



class ModelLayerTableClearLayersCommand (CommandHistory.Command):
	def __init__(self, layerTable, layer):
		super( ModelLayerTableClearLayersCommand, self ).__init__()
		self._layerTable = layerTable
		self._layers = layer


	def execute(self):
		self._layerTable.clear()

	def unexecute(self):
		for Layer in self._layers:
			self._layerTable.append( Layer )




class ModelLayerTableCommandTracker (CommandTracker):
	def track(self, layerTable):
		super( ModelLayerTableCommandTracker, self ).track( layerTable )
		assert isinstance( layerTable, ModelLayer.ModelLayerTable )

		for layer in layerTable:
			self._commandHistory.track( layer )


	def stopTracking(self, layerTable):
		assert isinstance( layerTable, ModelLayer.ModelLayerTable )

		for layer in layerTable:
			self._commandHistory.stopTracking( layer )

		super( ModelLayerTableCommandTracker, self ).stopTracking( layerTable )



	def _f_onLayerTableLayerAppended(self, layerTable, layer):
		self._commandHistory.track( layer )
		self._commandHistory.addCommand( ModelLayerTableAppendLayerCommand( layerTable, layer ) )

	def _f_onLayerTableLayerInserted(self, layerTable, index, layer):
		self._commandHistory.track( layer )
		self._commandHistory.addCommand( ModelLayerTableInsertLayerCommand( layerTable, index, layer ) )

	def _f_onLayerTableLayerRemoveed(self, layerTable, layer):
		index = layerTable.index( layer )
		self._commandHistory.addCommand( ModelLayerTableRemoveLayerCommand( layerTable, index, layer ) )
		self._commandHistory.stopTracking( layer )

	def _f_onLayerTableCleared(self, layerTable):
		self._commandHistory.addCommand( ModelLayerTableClearLayersCommand( layerTable, copy( layerTable.layers ) ) )
		for layer in layerTable.layers:
			self._commandHistory.stopTracking( layer )



