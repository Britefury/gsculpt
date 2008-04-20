##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Sheet.Sheet import *

from Britefury.Model import ModelDraw
from Britefury.Model.Model import pyMPickList_to_MPickList, MPickList

from Britefury.MeshTools.Mark.ToolMeshMarkPick import *



class MeshMarkPickVerticesManipulator (MeshMarkPickManipulator):
	def __init__(self):
		super( MeshMarkPickVerticesManipulator, self ).__init__()
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


	def _p_pickConfirmed(self, viewport, pick):
		self._p_commitPick( viewport, pick )



class MeshMarkPickVerticesFieldManipulator (MeshMarkPickFieldManipulator):
	def createElement(self, rowEditContext):
		return MeshMarkPickVerticesManipulator()




class ProcMeshMarkPickVertices (ProcMeshMarkPick):
	description = _( 'Pick vertices' )

	def procedureInvoke(self, inputObject, errorHandler):
		mpicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self.targetList ], mpicks )
		inputObject.markVertices_pick( mpicks )
		return inputObject



class MeshMarkPickVerticesAdjustableInvoker (MeshMarkPickAdjustableInvoker):
	functionClass = ProcMeshMarkPickVertices

	def save(self):
		self._inputObject.markVertices_save()

	def restore(self):
		self._inputObject.markVertices_restore()

	def adjust(self):
		mpicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self._proc.targetList ], mpicks )
		self._inputObject.markVertices_pick( mpicks )



class ToolMeshMarkPickVertices (ToolMeshMarkPick):
	sheet = SheetRefField( ProcMeshMarkPickVertices )

	functionClass = ProcMeshMarkPickVertices
	adjustableInvoker = MeshMarkPickVerticesAdjustableInvoker
	pageTitle = _( 'Pick' )
	titleText = _( 'Pick Vertices' )

	targetListManip = MeshMarkPickVerticesFieldManipulator( sheet.targetList )
	targetListManip.enable()



