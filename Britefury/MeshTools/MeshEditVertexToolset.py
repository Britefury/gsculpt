##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Colour3f

from Britefury.Model import ModelDraw

from Britefury.MeshTools.MeshToolsetWithPicking import MeshToolsetWithPicking
from Britefury.MeshTools.MeshEditVertexToolGroup import MeshEditVertexToolGroup
from Britefury.TransformationTools.TransformationToolGroup import TransformationToolGroup

from Britefury.TransformationTools.ToolTransformationBase import TransformationTarget

from Britefury.MeshTools.Mark.ToolMeshMarkPickVertices import ProcMeshMarkPickVertices



class MeshEditVertexToolset (MeshToolsetWithPicking):
	class VertexPick (MeshToolsetWithPicking.MeshPick):
		def __init__(self, toolset):
			super( MeshEditVertexToolset.VertexPick, self ).__init__( toolset )
			self._bMarked = False
			self._vertexPosition = None


		def _p_refreshHit(self, viewport, mesh, pick):
			vertexIndex, thruFaceIndex = mesh.pickVertex( pick.toMPick() )

			if vertexIndex != -1:
				self._vertexPosition = mesh.getVertexPosition( vertexIndex )
				self._bMarked = mesh.isVertexMarked( vertexIndex )
				return True
			else:
				self._vertexPosition = None
				self._bMarked = False
				return False


		def _p_paintHighlight(self, viewport, paintLayer):
			if self._bMarked:
				ModelDraw.drawVertexPickHighlight( self._vertexPosition, ModelDraw.MARKED_TARGET_HIGHLIGHT_COLOUR, paintLayer );
			else:
				ModelDraw.drawVertexPickHighlight( self._vertexPosition, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer );



	def __init__(self, nodeEditor):
		super( MeshEditVertexToolset, self ).__init__( _( 'Vertices' ), nodeEditor )
		meshEditVertexGroup = MeshEditVertexToolGroup( nodeEditor )
		xformGroup = TransformationToolGroup( nodeEditor, TransformationTarget.POINTS, False, False )
		self.addToolGroup( meshEditVertexGroup )
		self.addToolGroup( xformGroup )

		self._layout.row()
		self._layout.textBlock( _( 'Click on vertices to select them.' ) )



	def _p_createMeshPick(self):
		return MeshEditVertexToolset.VertexPick( self )


	def _p_getPickProcedureType(self):
		return ProcMeshMarkPickVertices

