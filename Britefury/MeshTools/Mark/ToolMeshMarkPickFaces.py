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



class MeshMarkPickFacesManipulator (MeshMarkPickManipulator):
	def __init__(self):
		super( MeshMarkPickFacesManipulator, self ).__init__()
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


	def _p_pickConfirmed(self, viewport, pick):
		self._p_commitPick( viewport, pick )



class MeshMarkPickFacesFieldManipulator (MeshMarkPickFieldManipulator):
	def createElement(self, rowEditContext):
		return MeshMarkPickFacesManipulator()




class ProcMeshMarkPickFaces (ProcMeshMarkPick):
	description = _( 'Pick faces' )

	def procedureInvoke(self, inputObject, errorHandler):
		mpicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self.targetList ], mpicks )
		inputObject.markFaces_pick( mpicks )
		return inputObject



class MeshMarkPickFacesAdjustableInvoker (MeshMarkPickAdjustableInvoker):
	functionClass = ProcMeshMarkPickFaces

	def save(self):
		self._inputObject.markFaces_save()

	def restore(self):
		self._inputObject.markFaces_restore()

	def adjust(self):
		mpicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self._proc.targetList ], mpicks )
		self._inputObject.markFaces_pick( mpicks )



class ToolMeshMarkPickFaces (ToolMeshMarkPick):
	sheet = SheetRefField( ProcMeshMarkPickFaces )

	functionClass = ProcMeshMarkPickFaces
	adjustableInvoker = MeshMarkPickFacesAdjustableInvoker
	pageTitle = _( 'Pick' )
	titleText = _( 'Pick Faces' )

	targetListManip = MeshMarkPickFacesFieldManipulator( sheet.targetList )
	targetListManip.enable()



