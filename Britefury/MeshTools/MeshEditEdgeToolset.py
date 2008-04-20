##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Colour3f

from Britefury.Tool.Toolset import TOOLSET_EVENTHANDLER_PRIORITY

from Britefury.Model import ModelDraw

from Britefury.Mesh.MeshPickHelper import MeshPickSeededEdgeListHelper

from Britefury.MeshTools.MeshToolsetWithPicking import MeshToolsetWithPicking
from Britefury.MeshTools.MeshEditEdgeToolGroup import MeshEditEdgeToolGroup

from Britefury.MeshTools.Mark.ToolMeshMarkPickEdges import *




class MeshEditEdgeToolset (MeshToolsetWithPicking):
	class EdgePick (MeshPickSeededEdgeListHelper):
		def __init__(self, toolset):
			super( MeshEditEdgeToolset.EdgePick, self ).__init__()
			self._vpType = None
			self._toolset = toolset
			self._bMarked = False
			self._edgeSegment = None


		def attachView(self, view, vpType):
			MeshPickHelper.attachView( self, view )
			self._vpType = vpType
			view.addEventHandler( self, vpType, TOOLSET_EVENTHANDLER_PRIORITY )
			view.addPainter( self, vpType )
			view.postRedraw()

		def detachView(self):
			self._meshPickHelperView.removeEventHandler( self, self._vpType )
			self._meshPickHelperView.removePainter( self, self._vpType )
			self._meshPickHelperView.postRedraw()
			MeshPickHelper.detachView( self )
			self._vpType = None


		def _p_seedChosen(self, mesh, seedEdgeIndex, targetType):
			self._bMarked = mesh.isEdgeMarked( seedEdgeIndex )

		def _p_paintValidHighlight(self, viewport, paintLayer, segmentList, seedSegment):
			for segment in segmentList:
				if self._bMarked:
					ModelDraw.drawEdgePickHighlight( segment, ModelDraw.MARKED_TARGET_HIGHLIGHT_COLOUR, paintLayer )
				else:
					ModelDraw.drawEdgePickHighlight( segment, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )

		def _p_paintInvalidHighlight(self, viewport, paintLayer, seedSegment):
			ModelDraw.drawEdgePickHighlight( seedSegment, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer )



		def _p_getTargetType(self, viewport):
			return meshPickEdgesGetTargetType( viewport )


		def _p_discoverEdgeList(self, mesh, seedEdgeIndex, targetType):
			discoveredEdgeIndices = meshPickEdgesDiscoverEdgeIndexList( mesh, seedEdgeIndex, targetType )
			if discoveredEdgeIndices is not None:
				discoveredSegments = self._o_edgeIndexListToSegmentList( mesh, discoveredEdgeIndices )
				return True, discoveredEdgeIndices, discoveredSegments
			else:
				return False, None, None


		def _p_pickConfirmed(self, viewport, pick):
			if self._targetType == MeshPickEdgesTargetType.EDGE:
				self._toolset._p_onPick( pick )
			else:
				self._toolset._p_onPick( MeshPickEdgesTarget( copy( pick ), self._targetType ) )







	def __init__(self, nodeEditor):
		super( MeshEditEdgeToolset, self ).__init__( _( 'Edges' ), nodeEditor )
		meshEditEdgeGroup = MeshEditEdgeToolGroup( nodeEditor )
		self.addToolGroup( meshEditEdgeGroup )

		self._layout.row()
		self._layout.textBlock( _( 'Click on edges to select them.\n'   '<shift>-click to select loops.\n'  '<control>-click to select rings.\n'  '<shift>-<control>-click to select boundaries.\n' ) )



	def _p_createMeshPick(self):
		return MeshEditEdgeToolset.EdgePick( self )


	def _p_getPickProcedureType(self):
		return ProcMeshMarkPickEdges
