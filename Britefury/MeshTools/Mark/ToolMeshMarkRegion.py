##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO.IOXml import IOXmlEnum

from Britefury.UI.FormLayout import FormLayout

from Britefury.Graphics.Graphics import BackfaceCull, Region3d
from Britefury.Graphics.BackfaceCullCell import BackfaceCullCell
from Britefury.Graphics.Region3dCell import Region3dCell

from Britefury.Kernel.Enum import Enum

from Britefury.Event.QueuedEvent import queueEvent

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker
from Britefury.ProceduralTool.GSProcedureTool import *

from Britefury.Manipulator.Manipulator import Manipulator
from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model.Model import MarkPredicate
from Britefury.Model.MarkPredicateCell import *
from Britefury.Model.MarkPredicateFieldEditor import MarkPredicateFieldEditRadioGroup

from Britefury.Mesh.Mesh import GSProductMesh





class MeshMarkRegionTarget (Enum):
	VERTICES = 0
	EDGES = 1
	FACES = 2



targetIOXmlEnum = IOXmlEnum( MeshMarkRegionTarget, 'MeshMarkRegionTarget', MeshMarkRegionTarget.VERTICES )
targetIOXmlEnum.value( MeshMarkRegionTarget.VERTICES, 'vertices' )
targetIOXmlEnum.value( MeshMarkRegionTarget.EDGES, 'edges' )
targetIOXmlEnum.value( MeshMarkRegionTarget.FACES, 'faces' )

ioXmlReadMeshMarkRegionTargetProp = targetIOXmlEnum.propReader
ioXmlReadMeshMarkRegionTargetNode = targetIOXmlEnum.nodeReader
ioXmlWriteMeshMarkRegionTargetProp = targetIOXmlEnum.propWriter
ioXmlWriteMeshMarkRegionTargetNode = targetIOXmlEnum.nodeWriter



# Cell

class MeshMarkRegionTargetCell (Cell):
	valueClass = MeshMarkRegionTarget
	bAllowNone = False

registerCellType( MeshMarkRegionTarget, MeshMarkRegionTargetCell )






class ProcMeshMarkRegion (GSProcedure):
	target = Field( MeshMarkRegionTarget, MeshMarkRegionTarget.VERTICES )
	predicate = Field( MarkPredicate, MarkPredicate.MARK )
	region = Field( Region3d, None )
	backfaceCull = Field( BackfaceCull, None )
	bEnabled = Field( bool, False )
	bCullEnabled = Field( bool, False )
	bCompletelyContained = Field( bool, False )

	description = _( 'Abstract region select' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		if self.bEnabled:
			cull = None
			if self.bCullEnabled:
				cull = self.backfaceCull

			target = self.target
			if target == MeshMarkRegionTarget.VERTICES:
				inputObject.markVertices_region( self.region, cull, self.predicate )
			elif target == MeshMarkRegionTarget.EDGES:
				inputObject.markEdges_region( self.region, cull, self.predicate, self.bCompletelyContained )
			elif target == MeshMarkRegionTarget.FACES:
				inputObject.markFaces_region( self.region, cull, self.predicate, self.bCompletelyContained )
		return inputObject





class MeshMarkRegionAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshMarkRegion

	def save(self):
		if self._proc.target == MeshMarkRegionTarget.VERTICES:
			self._inputObject.markVertices_save()
		elif self._proc.target == MeshMarkRegionTarget.EDGES:
			self._inputObject.markEdges_save()
		elif self._proc.target == MeshMarkRegionTarget.FACES:
			self._inputObject.markFaces_save()

	def restore(self):
		if self._proc.target == MeshMarkRegionTarget.VERTICES:
			self._inputObject.markVertices_restore()
		elif self._proc.target == MeshMarkRegionTarget.EDGES:
			self._inputObject.markEdges_restore()
		elif self._proc.target == MeshMarkRegionTarget.FACES:
			self._inputObject.markFaces_restore()

	def adjust(self):
		if self._proc.bEnabled:
			cull = None
			if self._proc.bCullEnabled:
				cull = self._proc.backfaceCull

			if self._proc.target == MeshMarkRegionTarget.VERTICES:
				self._inputObject.markVertices_region( self._proc.region, cull, self._proc.predicate )
			elif self._proc.target == MeshMarkRegionTarget.EDGES:
				self._inputObject.markEdges_region( self._proc.region, cull, self._proc.predicate, self._proc.bCompletelyContained )
			elif self._proc.target == MeshMarkRegionTarget.FACES:
				self._inputObject.markFaces_region( self._proc.region, cull, self._proc.predicate, self._proc.bCompletelyContained )

	def getResult(self):
		return self._inputObject




class ToolMeshMarkRegion (GSProcedureTool):
	sheet = SheetRefField( ProcMeshMarkRegion )

	functionClass = ProcMeshMarkRegion
	adjustableInvoker = MeshMarkRegionAdjustableInvoker
	pageTitle = _( 'Reg.Sel.' )
	titleText = _( 'Abstract Region Select' )


	predicateLabel = FieldLabelWithFn( sheet.predicate, _( 'Mode:' ) )
	predicateRadio = MarkPredicateFieldEditRadioGroup( sheet.predicate )
	bCullEnabledCheck = BoolFieldEditCheckWithFn( sheet.bCullEnabled, _( 'Ignore back-facing' ) )
	bCompletelyContainedCheck = BoolFieldEditCheckWithFn( sheet.bCompletelyContained, _( 'Completely contained' ) )



	layout = FormLayoutFactory()
	layout.row()  <<  predicateLabel.label
	layout.row()  <<  predicateRadio.mark
	layout.row()  <<  predicateRadio.unmark
	layout.row()  <<  predicateRadio.add
	layout.row()  <<  predicateRadio.filter
	layout.row()  <<  predicateRadio.invert
	layout.row()
	layout.row()  <<  bCullEnabledCheck.check
	layout.row()  <<  bCompletelyContainedCheck.check
	layout.row()
	layout.finalise()




	def _p_isTargetEdgesOrFaces(self):
		sheet = self.sheet
		if sheet is not None:
			return sheet.target != MeshMarkRegionTarget.VERTICES
		else:
			return False


	_isTargetEdgesOrFaces = FunctionField( _p_isTargetEdgesOrFaces )


	def __init__(self, view, editorSettings, parentWindow, commandHistory, painterFactory):
		super( ToolMeshMarkRegion, self ).__init__( view, editorSettings, parentWindow, commandHistory, painterFactory )
		self.cells._isTargetEdgesOrFaces.changedSignal.connect( self._p_on_isTargetEdgesOrFaces )
		self._p_on_isTargetEdgesOrFaces()


	def _p_on_isTargetEdgesOrFaces(self):
		queueEvent( self._p_setCompletelyContainedVisibility )


	def _p_setCompletelyContainedVisibility(self):
		bVisible = self._isTargetEdgesOrFaces
		if bVisible:
			self.bCompletelyContainedCheck.show()
		else:
			self.bCompletelyContainedCheck.hide()






