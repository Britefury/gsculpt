##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Util.SignalSlot import ClassSignal

from Britefury.WorkArea.Viewport import ViewportEventHandler
from Britefury.WorkArea.Viewport3d import Viewport3dPainter

from Britefury.Mesh import GSProductMesh




class MeshPickHelper (ViewportEventHandler, Viewport3dPainter):
	manipulateSignal = ClassSignal()

	def __init__(self):
		super( MeshPickHelper, self ).__init__()
		self._meshPickHelperView = None
		self._meshCell = None
		self._pick = None
		self._bPickValid = False
		self._bDirty = True
		self._currentViewport = None
		self._bSensitive = True


	def attachView(self, view):
		assert self._meshPickHelperView is None, 'view already attached'
		self._meshPickHelperView = view

	def detachView(self):
		assert self._meshPickHelperView is not None, 'no view attached'
		self._meshPickHelperView = None


	def attachMeshCell(self, cell):
		assert self._meshCell is None, 'mesh cell already attached'
		self._meshCell = cell
		self._meshCell.changedSignal.connect( self._p_onMeshCellValueChanged )
		self._bDirty = True

	def detachMeshCell(self):
		assert self._meshCell is not None, 'no mesh cell attached'
		self._meshCell.changedSignal.disconnect( self._p_onMeshCellValueChanged )
		self._meshCell = None
		self._bDirty = False
		self._bPickValid = False


	def _o_setMeshPickHelperSensitivity(self, bSensitive):
		bSensitivityChanged = bSensitive != self._bSensitive
		self._bSensitive = bSensitive
		if self._meshPickHelperView is not None  and  bSensitivityChanged:
			self._meshPickHelperView.postRedraw()



	def evPrimaryButtonDown(self, viewport):
		super( MeshPickHelper, self ).evPrimaryButtonDown( viewport )
		if self._bSensitive:
			self._currentViewport = viewport

			self._bDirty = False
			self._bPickValid = False
			self._pick = None
			mesh = self._p_getMesh()

			if mesh is not None:
				self._p_refreshPick( viewport )
				self._bPickValid = self._p_refreshHit( viewport, mesh, self._pick )
				if self._bPickValid:
					self._p_pickConfirmed( viewport, self._pick )
				if self._meshPickHelperView is not None:
					self._meshPickHelperView.forceRedraw()


	def evMotion(self, viewport, point):
		super( MeshPickHelper, self ).evMotion( viewport, point )
		if self._bSensitive:
			self._currentViewport = viewport
			self._p_motion( viewport )


	def evEnter(self, viewport, point):
		super( MeshPickHelper, self ).evEnter( viewport, point )
		if self._bSensitive:
			self._currentViewport = viewport
			self._p_motion( viewport )


	def _p_motion(self, viewport):
		self._bDirty = False
		bWasPickValid = self._bPickValid
		self._bPickValid = False

		if not viewport.pointerStatus.bAltKey:
			mesh = self._p_getMesh()
			if mesh is not None:
				self._p_refreshPick( viewport )
				self._bPickValid = self._p_refreshHit( viewport, mesh, self._pick )
				if self._meshPickHelperView is not None:
					self._meshPickHelperView.forceRedraw()

		if self._bPickValid != bWasPickValid:
			if self._meshPickHelperView is not None:
				self._meshPickHelperView.postRedraw()


	def evLeave(self, viewport, point):
		super( MeshPickHelper, self ).evLeave( viewport, point )
		if self._bSensitive:
			self._currentViewport = None
			self._bDirty = False
			if self._bPickValid:
				self._bPickValid = False
				self._pick = None
				if self._meshPickHelperView is not None:
					self._meshPickHelperView.postRedraw()


	def evPaint3d(self, viewport, paintLayer):
		super( MeshPickHelper, self ).evPaint3d( viewport, paintLayer )
		if self._bSensitive:
			if self._pick is not None:
				self._p_refresh( viewport )
				if self._bPickValid:
					self._p_paintHighlight( viewport, paintLayer )


	def _p_refreshPick(self, viewport):
		"Can be overloaded - refresh the target"
		self._pick = viewport.createPick()

	def _p_refreshHit(self, viewport, mesh, pick):
		"To be overloaded - refresh the pick target"
		assert False, 'abstract'

	def _p_paintHighlight(self, viewport, paintLayer):
		"To be overloaded - paint the pick target highlight"
		assert False, 'abstract'

	def _p_pickConfirmed(self, viewport, pick):
		"To be overloaded - pick confirmed; add to pick list"
		assert False, 'abstract'

	def _o_resetHighlight(self):
		"Can be overloaded - reset the highlight"
		pass


	def _o_requestHighlightRefresh(self):
		self._bDirty = True
		if self._meshPickHelperView is not None:
			self._meshPickHelperView.postRedraw()


	def _p_refresh(self, viewport):
		if self._bDirty  and  self._currentViewport is not None:
			self._bPickValid = False
			mesh = self._p_getMesh()
			if mesh is not None:
				self._bPickValid = self._p_refreshHit( viewport, mesh, self._pick )
			self._bDirty = False



	def _p_getMesh(self):
		if self._meshCell is not None:
			mesh = self._meshCell.getValue()
			if isinstance( mesh, GSProductMesh ):
				return mesh
		return None


	def _p_onMeshCellValueChanged(self):
		if self._meshPickHelperView is not None:
			self._o_resetHighlight()
			self._o_requestHighlightRefresh()





class MeshPickSeededEdgeListHelper (MeshPickHelper):
	def __init__(self):
		super( MeshPickSeededEdgeListHelper, self ).__init__()
		self._currentSeedEdgeIndex = -1
		self._currentSeedPickPoint = None
		self._bCurrentTargetValid = False
		self._currentSeedEdgeSegment = None
		self._currentSegmentList = None
		self._seedIndexToSegmentListTable = {}
		self._targetType = None


	def _p_refreshHit(self, viewport, mesh, pick):
		pickedEdgeIndex, self._currentSeedPickPoint, thruFaceIndex = mesh.pickEdge( pick.toMPick() )

		targetType = self._p_getTargetType( viewport )

		if pickedEdgeIndex != self._currentSeedEdgeIndex  or  targetType != self._targetType:
			# The seed edge index has changed
			self._currentSeedEdgeIndex = pickedEdgeIndex
			self._targetType = targetType

			self._bCurrentTargetValid = True

			if self._currentSeedEdgeIndex != -1:
				self._currentSeedEdgeSegment = mesh.getEdgeSegment( self._currentSeedEdgeIndex )

				# An edge was chosen
				try:
					self._currentSegmentList = self._seedIndexToSegmentListTable[ ( self._currentSeedEdgeIndex, self._targetType ) ]
				except KeyError:
					# No edge list for this seed

					# Discover the list using the seed
					bDiscoverySuccessful, discoveredEdgeIndices, discoveredEdgeSegments = self._p_discoverEdgeList( mesh, self._currentSeedEdgeIndex, self._targetType )

					if bDiscoverySuccessful:
						self._currentSegmentList = discoveredEdgeSegments
						for edgeIndex in discoveredEdgeIndices:
							self._seedIndexToSegmentListTable[ ( edgeIndex, self._targetType ) ] = discoveredEdgeSegments
					else:
						self._bCurrentTargetValid = False
						self._currentSegmentList = None

				self._p_seedChosen( mesh, self._currentSeedEdgeIndex, self._targetType )
				return True
			else:
				self._currentSegmentList = None
				self._currentSeedEdgeSegment = None
				return False
		else:
			return self._currentSegmentList is not None  or  not self._bCurrentTargetValid



	def _p_paintHighlight(self, viewport, paintLayer):
		"Paint the pick target highlight"
		if self._bCurrentTargetValid:
			self._p_paintValidHighlight( viewport, paintLayer, self._currentSegmentList, self._currentSeedEdgeSegment )
		elif self._currentSeedEdgeSegment is not None:
			self._p_paintInvalidHighlight( viewport, paintLayer, self._currentSeedEdgeSegment )


	def _o_resetHighlight(self):
		self._currentSeedEdgeIndex = -1
		self._currentSegmentList = None
		self._bCurrentTargetValid = False
		self._seedIndexToSegmentListTable = {}
		self._currentSeedEdgeSegment = None



	def _p_seedChosen(self, mesh, seedEdgeIndex, targetType):
		"Can be overloaded - seed chosen"
		pass

	def _p_paintValidHighlight(self, viewport, paintLayer, segmentList, seedSegment):
		"To be overloaded - paint highlight for valid pick"
		assert False, 'abstract'

	def _p_paintInvalidHighlight(self, viewport, paintLayer, seedSegment):
		"Can be overloaded - paint highlight for invalid pick"
		pass

	def _p_discoverEdgeList(self, mesh, seedEdgeIndex):
		"""To be overloaded - discover the edge list given a seed
		Returns:
		bDiscoverySuccessful, discoveredEdgeIndices, discoveredEdgeSegments"""
		assert False, 'abstract'

	def _p_getTargetType(self, viewport):
		return None


	def _o_edgeIndexListToSegmentList(self, mesh, edgeIndices):
		"Protected - convert an edge index list to an edge segment list"
		return [ mesh.getEdgeSegment( edgeIndex )   for edgeIndex in edgeIndices ]


	def _o_getPickPointOnSeedEdge(self):
		"Protected - get the seed pick point"
		return self._currentSeedPickPoint



