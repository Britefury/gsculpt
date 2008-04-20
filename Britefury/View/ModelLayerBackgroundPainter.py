##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy

from Britefury.Util.SignalSlot import blockSlot, unblockSlot

from Britefury.WorkArea.Viewport3d import Viewport3dPainter

from Britefury.Model.Model import GSProductModel

from Britefury.View.ModelLayer import ModelLayerTable
from Britefury.View.DrawingTemplate import DrawingTemplate




class ModelLayerBackgroundPainter (Viewport3dPainter, DrawingTemplate):
	class Entry (object):
	 	def __init__(self, painterFactory):
			super( ModelLayerBackgroundPainter.Entry, self ).__init__()
			self.node = None
			self.product = None
			self.painter = None
			self.bRefreshRequired = False
			self.painterFactory = painterFactory


		def attachNode(self, node):
			assert self.node is None, 'node already attached'
			self.node = node
			self.node.out.productModifiedSignal.connect( self._p_onProductModified )
			self.node.nodeViewSettingsModifiedDelayedSignal.connect( self._p_onNodeViewSettingsModified )

		def detachNode(self):
			assert self.node is not None, 'no node attached'
			self.node.nodeViewSettingsModifiedDelayedSignal.disconnect( self._p_onNodeViewSettingsModified )
			self.node.out.productModifiedSignal.disconnect( self._p_onProductModified )
			self.node = None



		def paint3d(self, settings, paintLayer):
			self._p_refresh()
			if self.painter is not None:
				self.painter.paintObject3d( settings, paintLayer, True, True )

		def templateDrawingPoint3d(self, ray, bBackfaceCulling):
			self._p_refreshProduct()
			if self.product is not None  and  isinstance( self.product, GSProductModel ):
				return self.product.raytraceClipRay( ray, bBackfaceCulling )
			else:
				return False, None, None, None

		def choose3d(self, ray, bBackfaceCulling, nodeClass, productClass):
			if nodeClass is None  or  isinstance( self.node, nodeClass ):
				self._p_refreshProduct()
				if productClass is None  or  isinstance( self.product, productClass ):
					if self.product is not None  and  isinstance( self.product, GSProductModel ):
						return self.product.raytraceClipRay( ray, bBackfaceCulling )

			return False, None, None, None


		def forceRefresh(self):
			if self.product is None  or  self.bRefreshRequired:
				self.product = self.node.out.getResultProduct()
			if ( self.painter is None  or  self.bRefreshRequired )  and  self.product is not None:
				self.painter = self._p_createPainterForProduct( self.product )
			self.bRefreshRequired = False
			unblockSlot( self._p_onProductModified )


		def _p_refreshProduct(self):
			# Only rebuild product and painter if they are not built already
			# (NOTE: we do *not* need to check if @self.bRefreshRequired is true before attempting to rebuild)
			if self.product is None:
				self.product = self.node.out.getResultProduct()
			unblockSlot( self._p_onProductModified )

		def _p_refresh(self):
			# Only rebuild product and painter if they are not built already
			# (NOTE: we do *not* need to check if @self.bRefreshRequired is true before attempting to rebuild)
			self._p_refreshProduct()
			if self.painter is None  and  self.product is not None:
				self.painter = self._p_createPainterForProduct( self.product )



		def _p_createPainterForProduct(self, product):
			if product is not None:
				painter = self.painterFactory.createPainterForObject( product )
				if painter is not None:
					painter.setProduct( product )
				for viewSettings in self.node.getNodeViewSettings():
					viewSettings.applyToPainter( painter )
				return painter
			else:
				return None

		def _p_onProductModified(self):
			self.bRefreshRequired = True
			blockSlot( self._p_onProductModified )


		def _p_onNodeViewSettingsModified(self, sceneNode):
			if self.painter is not None:
				for viewSettings in self.node.getNodeViewSettings():
					viewSettings.applyToPainter( self.painter )



	def __init__(self, painterFactory, view):
		super( ModelLayerBackgroundPainter, self ).__init__()

		self._painterFactory = painterFactory
		self._view = view

		self._layerTable = None

		self._nodes = []
		self._entries = []

		self._hiddenNodes = None
		self._hiddenIndices = None



	def attachLayerTable(self, layerTable):
		assert self._layerTable is None, 'layer table already attached'
		self._hiddenNodes = set()
		self._layerTable = layerTable
		self._layerTable.visibleNodesSignal.connect( self._p_onVisibleNodeListChanged )
		self._p_onVisibleNodeListChanged( self._layerTable )


	def detachLayerTable(self):
		assert self._layerTable is not None, 'no layer table attached'
		self._layerTable.visibleNodesSignal.disconnect( self._p_onVisibleNodeListChanged )
		self._layerTable = None
		self._hiddenNodes = None


	def showNode(self, node):
		self._p_refreshNodeList()
		self._hiddenNodes.remove( node )
		self._hiddenIndices = None
		self._p_refreshHiddenIndices()
		self._view.postRedraw()

	def hideNode(self, node):
		self._p_refreshNodeList()
		self._hiddenNodes.add( node )
		self._hiddenIndices = None
		self._p_refreshHiddenIndices()
		self._view.postRedraw()


	def forceRefreshLayers(self):
		if self._entries is not None:
			for entry in self._entries:
				entry.forceRefresh()
		self._view.postRedraw()



	def evPaint3d(self, viewport, paintLayer):
		if self._layerTable is not None:
			self._p_refreshEntryList()
			settings = viewport.viewportSettings
			for i, entry in enumerate( self._entries ):
				if i not in self._hiddenIndices:
					entry.paint3d( settings, paintLayer )


	def templateDrawingPoint3d(self, ray, bBackfaceCulling):
		"""Template drawing point - 3D
		@ray - the ray to raytrace 		[Segment3d]
		@bBackfaceCulling - cull back faces
		Returns: a tuple of:
			bSuccess		 [bool]						Successful; did we hit anything?
			t			[float] (or None if no hit)			The intersection parameter
			intersection	[Point3] (or None if no hit)			The intersection point
			clippedRay	[Segment3] (or None if no hit)		The clipped ray"""
		bSuccess = False
		t = None
		intersection = None
		clippedRay = None

		if self._layerTable is not None:
			self._p_refreshEntryList()
			for i, entry in enumerate( self._entries ):
				if i not in self._hiddenIndices:
					_bSuccess, _t, _intersection, _clippedRay = entry.templateDrawingPoint3d( ray, bBackfaceCulling )
					if _bSuccess:
						bSuccess = True
						t = _t
						intersection = _intersection
						clippedRay = _clippedRay
						ray = _clippedRay

		return bSuccess, t, intersection, clippedRay



	def chooseNode(self, ray, bBackfaceCulling, nodeClass=None, productClass=None):
		"""Template drawing point - 3D
		@ray - the ray to raytrace 		[Segment3d]
		@bBackfaceCulling - cull back faces
		Returns: a tuple of:
			node			a scene node (or None if no hit)	The node that was hit
			t			[float] (or None if no hit)			The intersection parameter
			intersection	[Point3] (or None if no hit)			The intersection point
			clippedRay	[Segment3] (or None if no hit)		The clipped ray"""
		node = None
		t = None
		intersection = None
		clippedRay = None

		if self._layerTable is not None:
			self._p_refreshEntryList()
			for i, entry in enumerate( self._entries ):
				if i not in self._hiddenIndices:
					_bSuccess, _t, _intersection, _clippedRay = entry.choose3d( ray, bBackfaceCulling, nodeClass, productClass )
					if _bSuccess:
						node = entry.node
						t = _t
						intersection = _intersection
						clippedRay = _clippedRay
						ray = _clippedRay

		return node, t, intersection, clippedRay


	def _p_onVisibleNodeListChanged(self, layerTable):
		if self._entries is not None:
			for entry in self._entries:
				entry.detachNode()
			self._entries = None
		self._nodes = None
		self._hiddenIndices = None
		self._view.postRedraw()



	def _p_refreshNodeList(self):
		if self._layerTable is not None:
			if self._nodes is None:
				self._nodes = copy( self._layerTable._f_getVisibleNodes() )

	def _p_refreshEntryList(self):
		self._p_refreshNodeList()
		self._p_refreshHiddenIndices()
		if self._layerTable is not None:
			if self._entries is None:
				self._entries = [ ModelLayerBackgroundPainter.Entry( self._painterFactory )  for node in self._nodes ]
				for node, entry in zip( self._nodes, self._entries ):
					entry.attachNode( node )


	def _p_refreshHiddenIndices(self):
		if self._hiddenIndices is None:
			self._hiddenIndices = [ self._nodes.index( n )   for n in self._hiddenNodes  if n in self._nodes ]



