##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Util.SignalSlot import *

from Britefury.Event.QueuedEvent import *

from Britefury.FileIO import IOXml

from Britefury.Sheet.Sheet import *

from Britefury.SheetGraph.SheetGraph import *

from Britefury.ProceduralCore.GSFunction import *

from Britefury.ProceduralCore.GSPlan import GSPlan, GSPlanInitialProductSource
from Britefury.ProceduralCore import GSSceneNodeCommandTracker







class GSSceneNodeSource (SheetGraphNodeSource):
	"""Scene node source (output) pin"""
	productModifiedSignal = ClassSignal()



	def getResultProduct(self):
		"""Get the result product from this pin"""
		assert False, 'abstract'

	def getResultProductClass(self):
		"""Get the class of the result product from this pin"""
		assert False, 'abstract'

IOXml.ioObjectFactoryRegister( 'GSSceneNodeSource', GSSceneNodeSource )




class GSSceneNodeSourcePlanResult (GSSceneNodeSource):
	"""The scene node plan result source pin"""
	def getResultProduct(self):
		"""Get the result product from this pin"""
		return self._node.getResultProduct()

	def getResultProductClass(self):
		"""Get the class of the result product from this pin"""
		return self._node.getResultProductClass()

IOXml.ioObjectFactoryRegister( 'GSSceneNodeSourcePlanResult', GSSceneNodeSourcePlanResult )



class GSSceneNodeSinkBase (object):
	"""Scene node sink pin base class"""
	def getInputProduct(self, index):
		"""Get the input product from the input indexed by @index"""
		if isinstance( index, slice ):
			return [ self._getResultProductFromSource( source )   for source in self[index] ]
		else:
			return self._getResultProductFromSource( self[index] )

	def getInputProducts(self):
		"""Get the list of input products"""
		return [ self._getResultProductFromSource( source )   for source in self ]

	def inputProducts(self):
		"""The the input products from the inputs into this sink pin (returns a list)"""
		for source in self:
			yield self._getResultProductFromSource( source )

	def getInputProductClass(self, index):
		"""Get the class of the input product from the input indexed by @index"""
		if isinstance( index, slice ):
			return [ self._getResultProductClassFromSource( source )   for source in self[index] ]
		else:
			return self._getResultProductClassFromSource( self[index] )

	def getInputProductClasses(self):
		"""Get the list of input product classes"""
		return [ self._getResultProductClassFromSource( source )   for source in self ]

	def inputProductClasses(self):
		"""The the class of the input products from the inputs into this sink pin (returns a list)"""
		for source in self:
			yield self._getResultProductClassFromSource( source )


	def _onAddInput(self, graphSink, graphSource, graphLink):
		self._node._p_onInputLinkModified()

	def _onRemoveInput(self, graphSink, graphSource, graphLink):
		self._node._p_onInputLinkModified()


	@staticmethod
	def _getResultProductFromSource(source):
		if source is not None:
			return source.getResultProduct()
		else:
			return None

	@staticmethod
	def _getResultProductClassFromSource(source):
		if source is not None:
			return source.getResultProductClass()
		else:
			return None



class GSSceneNodeSinkSingle (GSSceneNodeSinkBase, SheetGraphNodeSinkSingle):
	"""Scene node sink that can take a single input"""
	pass

IOXml.ioObjectFactoryRegister( 'GSSceneNodeSinkSingle', GSSceneNodeSinkSingle )



class GSSceneNodeSinkMultiple (GSSceneNodeSinkBase, SheetGraphNodeSinkMultiple):
	"""Scene node sink that can take multiple inputs"""
	pass

IOXml.ioObjectFactoryRegister( 'GSSceneNodeSinkMultiple', GSSceneNodeSinkMultiple )





class SceneNodeSinkSingleField (SheetGraphSinkSingleField):
	"""Node single input sink field"""
	def _o_createSink(self, instance, name):
		return GSSceneNodeSinkSingle( instance, name )


class SceneNodeSinkMultipleField (SheetGraphSinkMultipleField):
	"""Node multiple input sink field"""
	def _o_createSink(self, instance, name):
		return GSSceneNodeSinkMultiple( instance, name )







class SceneNodeSourceField (SheetGraphSourceField):
	def _o_createSource(self, instance, name):
		return GSSceneNodeSource( instance, name )






class GSSceneNodeClass (SheetGraphNodeClass, GSFunctionClass):
	def __init__(cls, name, bases, clsDict):
		super( GSSceneNodeClass, cls ).__init__( name, bases, clsDict )





class GSSceneNode (SheetGraphNode, GSFunction, GSPlanInitialProductSource):
	__metaclass__ = GSSceneNodeClass

	"""Scene Node base class"""
	class NodePlanResultSourceField (SceneNodeSourceField):
		"""Scene node plan result source field"""
		def _o_createSource(self, instance, name):
			return GSSceneNodeSourcePlanResult( instance, name )


	trackerClass = GSSceneNodeCommandTracker.GSSceneNodeCommandTracker
	description = _( 'Abstract scene node' )

	out = NodePlanResultSourceField( 'out', _( 'Default output source' ) )

	name = Field( str, '' )
	bVisible = Field( bool, True )


	nodeViewSettingsAddedSignal = ClassSignal()
	removeNodeViewSettingsSignal = ClassSignal()
	nodeViewSettingsModifiedDelayedSignal = ClassSignal()


	def __init__(self, name = None, src=None):
		"""Constructor:
		@name - name of node
		@src - source object to copy"""
		SheetGraphNode.__init__( self, src )
		GSPlanInitialProductSource.__init__( self )


		self._plan = GSPlan( self )

		self._initialProduct = None
		self._initialProductClass = None
		self._bInitialProductModified = True
		self._bInitialProductClassModified = True

		self._plan.productModifiedSignal.chainConnect( self.out.productModifiedSignal )
		self._plan.productModifiedSignal.connect( self._p_onPlanProductModified )


		self._resultProduct = None
		self._resultProductClass = None
		self._bResultProductModified = True
		self._bResultProductClassModified = True

		self._nodeViewSettings = []





	def getInitialProduct(self, errorHandler):
		"""Get the initial product (the product that is processed by the plan)"""
		if self._bInitialProductModified:
			self._initialProduct = self.computeInitialProduct( None )

			if self._initialProduct is not None:
				self._initialProductClass = type( self._initialProduct )
			else:
				self._initialProductClass = None

			self._bInitialProductModified = False
			self._bInitialProductClassModified = False
			unblockSlot( self._p_onInputModified )
		return self._initialProduct


	def getInitialProductClass(self):
		"""Get the class of the initial product (the product that is processed by the plan)"""
		if self._bInitialProductClassModified:
			self._initialProductClass = self.computeInitialProductClass()

			self._bInitialProductClassModified = False
			unblockSlot( self._p_onInputModified )
		return self._initialProductClass


	def computeInitialProduct(self, errorHandler):
		assert False, 'abstract'

	def computeInitialProductClass(self):
		assert False, 'abstract'




	def getResultProduct(self):
		"""Get the result product"""
		self._p_updateResult()
		return self._resultProduct


	def getResultProductClass(self):
		"""Get the class of the result product"""
		self._p_updateResultClass()
		return self._resultProductClass



	def getNodeViewSettingsForClass(self, settingsClass):
		"""Get the node view settings whose class is @settingsClass"""
		for settings in self._nodeViewSettings:
			if settings.__class__ is settingsClass:
				return settings
		return None

	def getCompatibleNodeViewSettings(self, settingsClass):
		"""Get a list of node view settings objects that are instances of @settingsClass"""
		return [ settings   for settings in self._nodeViewSettings   if isinstance( settings, settingsClass ) ]

	def getNodeViewSettings(self):
		"""Get a list of node view settings"""
		return self._nodeViewSettings

	def setNodeViewSettings(self, settings):
		"""Set node view settings; replaces and existing entry that is of the same class"""
		for s in self._nodeViewSettings:
			if s.__class__ is settings.__class__:
				self._p_removeNodeViewSettings( s )
		self._p_addNodeViewSettings( settings )



	def _p_addNodeViewSettings(self, settings):
		self._nodeViewSettings.append( settings )
		settings.cellModifiedSignal.connect( self._p_onNodeViewSettingsCellModified )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onSceneNodeViewSettingsAdded( self, settings )
		self.nodeViewSettingsAddedSignal.emit( self, settings )
		self.nodeViewSettingsModifiedDelayedSignal.emit( self )


	def _p_removeNodeViewSettings(self, settings):
		self.removeNodeViewSettingsSignal.emit( self, settings )
		settings.cellModifiedSignal.disconnect( self._p_onNodeViewSettingsCellModified )
		self._nodeViewSettings.remove( settings )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onSceneNodeViewSettingsRemoved( self, settings )




	def __readxml__(self, xmlNode):
		super( GSSceneNode, self ).__readxml__( xmlNode )

		xmlNode.getChild( 'plan' )  >>  self._plan

		viewSettingsListXmlNode = xmlNode.getChild( 'node_view_settings' )
		if viewSettingsListXmlNode.isValid():
			for settingsXmlNode  in viewSettingsListXmlNode.childrenNamed( 'view_settings' ):
				settings = settingsXmlNode.readObject()
				if settings is not None:
					self._nodeViewSettings.append( settings )
					settings.cellModifiedSignal.connect( self._p_onNodeViewSettingsCellModified )
					self.nodeViewSettingsAddedSignal.emit( self, settings )
		self.nodeViewSettingsModifiedDelayedSignal.emit( self )


	def __writexml__(self, xmlNode):
		super( GSSceneNode, self ).__writexml__( xmlNode )

		xmlNode.addChild( 'plan' )  <<  self._plan

		viewSettingsListXmlNode = xmlNode.addChild( 'node_view_settings' )
		for settings in self._nodeViewSettings:
			viewSettingsListXmlNode.addChild( 'view_settings' ).writeObject( settings )




	def _p_updateResult(self):
		if self._bResultProductModified:
			if self._plan.isConsistent():
				self._resultProduct = self._plan.computeProduct()
				self._resultProductClass = type( self._resultProduct )
			else:
				self._resultProduct = None
				self._resultProductClass = None

			self._bResultProductModified = False
			self._bResultProductClassModified = False
			unblockSlot( self._p_onPlanProductModified )

	def _p_updateResultClass(self):
		if self._bResultProductClassModified:
			if self._plan.isConsistent():
				self._resultProductClass = self._plan.computeProductClass()
			else:
				self._resultProductClass = None

			self._bResultProductClassModified = False
			unblockSlot( self._p_onPlanProductModified )




	def _f_onCellModified(self):
		"Notify of field value change"
		super( GSSceneNode, self )._f_onCellModified()
		self._initialProduct = None
		self._initialProductClass = None
		self._bInitialProductModified = True
		self._bInitialProductClassModified = True
		self.initialProductModifiedSignal.emit()


	def _p_onPlanProductModified(self):
		"Notify of plan product change"
		self._resultProduct = None
		self._resultProductClass = None
		self._bResultProductModified = True
		self._bResultProductClassModified = True
		blockSlot( self._p_onPlanProductModified )



	def _p_onInputModified(self):
		"Private - Notify of input product change"
		# Don't need to emit @self.initialProductModifiedSignal as this is done by a chain connection
		self._initialProduct = None
		self._initialProductClass = None
		self._bInitialProductModified = True
		self._bInitialProductClassModified = True
		blockSlot( self._p_onInputModified )


	def _o_onAddInputSource(self, source):
		super( GSSceneNode, self )._o_onAddInputSource( source )
		source.productModifiedSignal.connect( self._p_onInputModified )
		source.productModifiedSignal.chainConnect( self.initialProductModifiedSignal )
		self._p_onInputModified()
		self.initialProductModifiedSignal.emit()


	def _o_onRemoveInputSource(self, source):
		super( GSSceneNode, self )._o_onRemoveInputSource( source )
		source.productModifiedSignal.disconnect( self._p_onInputModified )
		source.productModifiedSignal.chainDisconnect( self.initialProductModifiedSignal )
		self._p_onInputModified()
		self.initialProductModifiedSignal.emit()


	def _p_onInputLinkModified(self):
		self._p_onInputModified()
		self.initialProductModifiedSignal.emit()


	def _p_onNodeViewSettingsCellModified(self, viewSettings):
		queueEvent( self._p_sendViewSettingsModified )


	def _p_sendViewSettingsModified(self):
		self.nodeViewSettingsModifiedDelayedSignal.emit( self )



	def _getPlan(self):
		return self._plan


	plan = property( _getPlan, doc='node plan' )




