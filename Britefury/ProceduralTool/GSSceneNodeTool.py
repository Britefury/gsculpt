##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Util.SignalSlot import ClassSignal, blockSlot, unblockSlot

from Britefury.Painter import Painter
from Britefury.Painter.RequestPainter import RequestPainterObjectSource
from Britefury.Painter.RequestViewportPainter import RequestViewportPainter

from Britefury.Cell.CellEvaluatorWithExternalDependencies import CellEvaluatorWithExternalDependencies

from Britefury.ProceduralCore.GSSceneNode import GSSceneNode

from Britefury.ProceduralTool.GSFunctionTool import *







class SceneNodeInitialProductCellEvaluator (CellEvaluatorWithExternalDependencies):
	def __init__(self, ownerCell):
		super( SceneNodeInitialProductCellEvaluator, self ).__init__( ownerCell )
		self._sceneNodeCell = None
		self._sceneNode = None
		self.errorHandler = None



	def attachSceneNodeCell(self, sceneNodeCell):
		assert self._sceneNodeCell is None, 'node cell already attached'
		self._sceneNodeCell = sceneNodeCell
		self._sceneNodeCell.changedSignal.connect( self._p_onSceneNodeChanged )
		self._sceneNode = None
		self._o_changed()


	def detachSceneNodeCell(self):
		assert self._sceneNodeCell is not None, 'no node cell attached'
		self._sceneNodeCell.changedSignal.disconnect( self._p_onSceneNodeChanged )
		self._sceneNodeCell = None
		self._sceneNode = None
		self._o_changed()



	def evaluate(self, cell):
		unblockSlot( self._p_onInitialProductModified )

		self._p_refreshSceneNode()

		if self._sceneNode is not None:
			return self._sceneNode.getInitialProduct( self.errorHandler )
		else:
			return None


	def _p_onInitialProductModified(self):
		assert self._sceneNode is not None, 'no node attached'

		blockSlot( self._p_onInitialProductModified )
		self._o_changed()


	def _p_onSceneNodeChanged(self):
		if self._sceneNode is not None:
			self._sceneNode.initialProductModifiedSignal.disconnect( self._p_onInitialProductModified )
			self._sceneNode = None


	def _p_refreshSceneNode(self):
		if self._sceneNode is None:
			self._sceneNode = self._sceneNodeCell.getValue()
			if self._sceneNode is not None:
				self._sceneNode.initialProductModifiedSignal.connect( self._p_onInitialProductModified )





class SceneNodeToolInitialProductView (FunctionToolProductView, RequestPainterObjectSource):
	def __init__(self, painterFactory):
		super( SceneNodeToolInitialProductView, self ).__init__()

		self._initialProductCell = Cell()
		self._initialProductEvaluator = SceneNodeInitialProductCellEvaluator( self._initialProductCell )
		self._initialProductCell.evaluator = self._initialProductEvaluator

		self._painter = RequestViewportPainter( painterFactory, False )

		self._view = None
		self._sceneNodeCell = None



	def initialise(self, view, sceneNodeCell):
		self._view = view
		self._sceneNodeCell = sceneNodeCell

		self._painter.painterObjectSource = self
		self._initialProductEvaluator.attachSceneNodeCell( sceneNodeCell )
		self._initialProductCell.changedSignal.connect( self._p_onInitialProductValueChanged )


	def shutdown(self):
		self._initialProductCell.changedSignal.disconnect( self._p_onInitialProductValueChanged )
		self._initialProductEvaluator.detachSceneNodeCell()
		self._view = None
		self._sceneNodeCell = None

		self._painter.painterObjectSource = None



	def installPainters(self, view):
		self._painter.addToView( view )

	def uninstallPainters(self, view):
		self._painter.removeFromView( view )


	def requestObjectToPaint(self):
		return self.objectToPaint


	def _p_onInitialProductValueChanged(self):
		self._view.postRedraw()



	def _p_setErrorHandler(self, errorHandler):
		self._initialProductEvaluator.errorHandler = errorHandler

	def _p_getInitialProductCell(self):
		return self._initialProductCell

	def _p_getDisplayedProduct(self):
		return self._initialProductCell.getImmutableValue()

	def _p_getObjectToPaint(self):
		return self._initialProductCell.getImmutableValue()

	errorHandler = property( None, _p_setErrorHandler )
	initialProductCell = property( _p_getInitialProductCell )
	outputProductCell = property( _p_getInitialProductCell )
	displayedProduct = property( _p_getDisplayedProduct )
	objectToPaint = property( _p_getObjectToPaint )






class GSSceneNodeTool (GSFunctionTool):
	functionClass = GSSceneNode

	def _p_createProductView(self):
		v = SceneNodeToolInitialProductView( self.painterFactory )
		v.initialise( self.view, self.cells.sheet )
		return v


	def _p_shutdownProductView(self, productView):
		productView.shutdown()


	def _p_getSceneNode(self):
		return self.sheet


	sceneNode = property( _p_getSceneNode )






