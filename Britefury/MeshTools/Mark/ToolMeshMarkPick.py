##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Colour3f

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.WorkArea.Viewport3d import Viewport3d

from Britefury.Manipulator.Manipulator import Manipulator

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker
from Britefury.ProceduralTool.GSProcedureTool import *

from Britefury.Manipulator.Manipulator import Manipulator
from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit
from Britefury.Model import ModelDraw

from Britefury.Mesh.Mesh import GSProductMesh
from Britefury.Mesh.MeshPickHelper import MeshPickHelper, MeshPickSeededEdgeListHelper





class MeshMarkPickManipulatorBase (Manipulator):
	def __init__(self):
		super( MeshMarkPickManipulatorBase, self ).__init__()
		self._cell = None

		self._o_setUISensitivity( False )


	def manipAttachToView(self, view):
		super( MeshMarkPickManipulatorBase, self ).manipAttachToView( view )
		self.attachView( view )
		self._view.addEventHandler( self, Viewport3d )		# HACK: need to handle other viewport types
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		self.detachView()
		super( MeshMarkPickManipulatorBase, self ).manipDetachFromView()


	def attachCell(self, cell):
		assert self._cell is None, 'already attached to a cell'
		self._cell = cell
		self._o_watchCellValidity( self._cell )
		self._o_refreshSensitivity()

	def detachCell(self):
		assert self._cell is not None, 'not attached to a cell'
		self._o_stopWatchingCellValidity( self._cell )
		self._cell = None
		self._o_refreshSensitivity()


	def _o_isValid(self):
		if self._cell is not None:
			return self._cell.isValid()
		else:
			return False




class MeshMarkPickManipulator (MeshMarkPickManipulatorBase, MeshPickHelper):
	def _o_setUISensitivity(self, bSensitive):
		self._o_setMeshPickHelperSensitivity( bSensitive )


	def _p_commitPick(self, viewport, pick):
		"Protected - call to commit a pick"
		assert self._cell is not None, 'no celliable attached'
		self._cell.literalValue = self._cell.value + [ pick ]
		self._p_commandHistoryBreak()




class MeshMarkPickEdgeListManipulator (MeshMarkPickManipulatorBase, MeshPickSeededEdgeListHelper):
	def __init__(self):
		super( MeshMarkPickEdgeListManipulator, self ).__init__()
		self._bMarked = False


	def _o_setUISensitivity(self, bSensitive):
		self._o_setMeshPickHelperSensitivity( bSensitive )


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


	def _p_commitPick(self, viewport, pick):
		"Protected - call to commit a pick"
		assert self._cell is not None, 'no celliable attached'
		self._cell.literalValue = self._cell.value + [ pick ]
		self._p_commandHistoryBreak()




class MeshMarkPickFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, field):
		super( MeshMarkPickFieldManipulator, self ).__init__()
		self._field = field


	def createElement(self, sheetEditContext):
		assert False, 'abstract'

	def initialiseElement(self, element, sheetEditContext):
		element.attachCell( self._p_getCell( self._field, sheetEditContext ) )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		element.detachCell()
		element.commandHistory = None


	def installElementEventHandlers(self, element, sheetEditContext):
		element.manipAttachToView( sheetEditContext.view )

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		element.manipDetachFromView()


	def attachOutputProductCell(self, element, cell):
		element.attachMeshCell( cell )

	def detachOutputProductCell(self, element, cell):
		element.detachMeshCell()



class ProcMeshMarkPick (GSProcedure):
	description = _( 'Abstract mesh pick' )

	targetList = Field( TargetList, TargetList() )


	inputClass = GSProductMesh



class MeshMarkPickAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshMarkPick

	def getResult(self):
		return self._inputObject



class ToolMeshMarkPick (GSProcedureTool):
	sheet = SheetRefField( ProcMeshMarkPick )

	functionClass = ProcMeshMarkPick
	adjustableInvoker = MeshMarkPickAdjustableInvoker
	pageTitle = _( 'Pick' )
	titleText = _( 'Pick' )


	targetListLabel = FieldLabelWithFn( sheet.targetList, _( 'Pick targets:' ) )
	targetListEditor = TargetListFieldEdit( sheet.targetList )


	layout = FormLayoutFactory()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()





