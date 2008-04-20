##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Vector3

from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import MeshRiftEdgesAdjuster, MeshRiftEdgesStyle, GSProductMesh




# XML IO for MeshRiftEdgesStyle

riftStyleIOXmlEnum = IOXmlEnum( MeshRiftEdgesStyle, 'MeshRiftEdgesStyle', MeshRiftEdgesStyle.CENTRE )
riftStyleIOXmlEnum.value( MeshRiftEdgesStyle.A, 'a' )
riftStyleIOXmlEnum.value( MeshRiftEdgesStyle.B, 'b' )
riftStyleIOXmlEnum.value( MeshRiftEdgesStyle.CENTRE, 'centre' )

ioXmlReadMeshRiftEdgesStyleProp = riftStyleIOXmlEnum.propReader
ioXmlReadMeshRiftEdgesStyleNode = riftStyleIOXmlEnum.nodeReader
ioXmlWriteMeshRiftEdgesStyleProp = riftStyleIOXmlEnum.propWriter
ioXmlWriteMeshRiftEdgesStyleNode = riftStyleIOXmlEnum.nodeWriter


# Cell for MeshRiftEdgesStyle

class MeshRiftEdgesStyleCell (Cell):
	valueClass = MeshRiftEdgesStyle
	bAllowNone = False

registerCellType( MeshRiftEdgesStyle, MeshRiftEdgesStyleCell )


# Cell editor for MeshRiftEdgesStyle

class MeshRiftEdgesStyleCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshRiftEdgesStyleCellEditRadioGroup, self ).__init__()
		self.a = self.addChoice( MeshRiftEdgesStyle.A, 'A' )
		self.b = self.addChoice( MeshRiftEdgesStyle.B, 'B' )
		self.centre = self.addChoice( MeshRiftEdgesStyle.CENTRE, _( 'Centre' ) )


# Field editor for MeshRiftEdgesStyle

class MeshRiftEdgesStyleFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'a', 'b', 'centre' ]

	def createElement(self, rowEditContext):
		return MeshRiftEdgesStyleCellEditRadioGroup()







class ProcMeshRiftEdges (GSProcedure):
	description = _( 'Rift edges' )

	rift = Field( Vector3, Vector3() )
	segments = Field( int, 1 )
	style = Field( MeshRiftEdgesStyle, MeshRiftEdgesStyle.CENTRE )
	bFill = Field( bool, True )
	bPinchAtMarkedVertices = Field( bool, False )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		adjuster = MeshRiftEdgesAdjuster()
		inputObject.riftMarkedEdges( adjuster, self.style, self.segments, self.bFill, self.bPinchAtMarkedVertices )
		adjuster.setParameters( self.rift )
		return inputObject



class RiftEdgesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshRiftEdges

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.adjuster = MeshRiftEdgesAdjuster()
		self.mesh.riftMarkedEdges( self.adjuster, self._proc.style, self._proc.segments, self._proc.bFill, self._proc.bPinchAtMarkedVertices )

	def adjust(self):
		self.adjuster.setParameters( self._proc.rift )

	def getResult(self):
		return self.mesh




class ToolMeshRiftEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshRiftEdges )

	functionClass = ProcMeshRiftEdges
	adjustableInvoker = RiftEdgesAdjustableInvoker
	pageTitle = _( 'Rift' )
	titleText = _( 'Rift Edges' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return product.getMarkedEdgesCentre()
		else:
			return None

	focalPoint = FunctionField( _p_focalPoint )

	riftLabel = FieldLabelWithFn( sheet.rift, _( 'Rift:' ) )
	riftEntry = Vector3FieldEditEntry( sheet.rift, -1.0e8, 1.0e8, 0.1, 10.0, 4 )
	segmentsLabel = FieldLabelWithFn( sheet.segments, _( 'Segments:' ) )
	segmentsEntry = IntFieldEditEntry( sheet.segments, 1, 1048576, 1, 10 )
	styleLabel = FieldLabelWithFn( sheet.style, _( 'Style:' ) )
	styleRadio = MeshRiftEdgesStyleFieldEditRadioGroup( sheet.style )
	bFillCheck = BoolFieldEditCheckWithFn( sheet.bFill, _( 'Fill' ) )
	bPinchCheck = BoolFieldEditCheckWithFn( sheet.bPinchAtMarkedVertices, _( 'Pinch at marked vertices' ) )

	riftManip = Translation3FieldManipulator3d( sheet.rift, focalPoint, snapping=FieldManipulatorSnappingDistance() )
	riftManip.enable()

	numSegmentsManip = IntFieldScrollManipulator( sheet.segments, 1, 1048576 )
	numSegmentsManip.enable()

	layout = FormLayoutFactory()
	layout.textBlock( _( 'Use the scroll wheel to change the number of segments' ) )
	layout.row()
	layout.row()  <<  riftLabel.label
	layout.row()  <<  'X:'  <<  riftEntry.x
	layout.row()  <<  'Y:'  <<  riftEntry.y
	layout.row()  <<  'Z:'  <<  riftEntry.z
	layout.row()
	layout.row()  <<  segmentsLabel.label  <<  segmentsEntry.entry
	layout.row()
	layout.row()  <<  styleLabel.label
	layout.row()  <<  styleRadio.a  <<  styleRadio.b  <<  styleRadio.centre
	layout.row()
	layout.row()  <<  bFillCheck.check
	layout.row()  <<  bPinchCheck.check
	layout.row()
	layout.finalise()
