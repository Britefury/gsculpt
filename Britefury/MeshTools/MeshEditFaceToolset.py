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
from Britefury.MeshTools.MeshEditFaceToolGroup import MeshEditFaceToolGroup

from Britefury.MeshTools.Mark.ToolMeshMarkPickFaces import ProcMeshMarkPickFaces



class MeshEditFaceToolset (MeshToolsetWithPicking):
	class FacePick (MeshToolsetWithPicking.MeshPick):
		def __init__(self, toolset):
			super( MeshEditFaceToolset.FacePick, self ).__init__( toolset )
			self._bMarked = False
			self._faceTriangles = []


		def _p_refreshHit(self, viewport, mesh, pick):
			faceIndex, intersectionPoint = mesh.pickFace( pick.toMPick() )

			if faceIndex != -1:
				self._faceTriangles = mesh.getFaceTriangles( faceIndex )
				self._bMarked = mesh.isFaceMarked( faceIndex )
				return True
			else:
				self._faceTriangles = []
				self._bMarked = False
				return False


		def _p_paintHighlight(self, viewport, paintLayer):
			if self._bMarked:
				ModelDraw.drawFacePickHighlight( self._faceTriangles, ModelDraw.MARKED_TARGET_HIGHLIGHT_COLOUR, paintLayer );
			else:
				ModelDraw.drawFacePickHighlight( self._faceTriangles, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer );



	def __init__(self, nodeEditor, sceneEditorTools, parentWindow, commandHistory):
		super( MeshEditFaceToolset, self ).__init__( _( 'Faces' ), nodeEditor )
		meshEditFaceGroup = MeshEditFaceToolGroup( nodeEditor, sceneEditorTools, parentWindow, commandHistory )
		self.addToolGroup( meshEditFaceGroup )

		self._layout.row()
		self._layout.textBlock( _( 'Click on faces to select them.' ) )



	def _p_createMeshPick(self):
		return MeshEditFaceToolset.FacePick( self )


	def _p_getPickProcedureType(self):
		return ProcMeshMarkPickFaces
