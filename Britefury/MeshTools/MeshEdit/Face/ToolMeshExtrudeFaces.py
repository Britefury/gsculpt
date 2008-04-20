##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegisterPrimitive

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh.Mesh import MeshExtrudeFacesAdjuster, GSProductMesh, MeshExtrudeFacesDirection




# XML IO for MeshExtrudeFacesDirection

directionIOXmlEnum = IOXmlEnum( MeshExtrudeFacesDirection, 'MeshExtrudeFacesDirection', MeshExtrudeFacesDirection.EXPAND )
directionIOXmlEnum.value( MeshExtrudeFacesDirection.EXPAND, 'expand' )
directionIOXmlEnum.value( MeshExtrudeFacesDirection.PERGROUP, 'per_group' )

ioXmlReadMeshExtrudeFacesDirectionProp = directionIOXmlEnum.propReader
ioXmlReadMeshExtrudeFacesDirectionNode = directionIOXmlEnum.nodeReader
ioXmlWriteMeshExtrudeFacesDirectionProp = directionIOXmlEnum.propWriter
ioXmlWriteMeshExtrudeFacesDirectionNode = directionIOXmlEnum.nodeWriter


# Cell for MeshExtrudeFacesDirection

class MeshExtrudeFacesDirectionCell (Cell):
	valueClass = MeshExtrudeFacesDirection
	bAllowNone = False

registerCellType( MeshExtrudeFacesDirection, MeshExtrudeFacesDirectionCell )


# Cell editor for MeshExtrudeFacesDirection

class MeshExtrudeFacesDirectionCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshExtrudeFacesDirectionCellEditRadioGroup, self ).__init__()
		self.expand = self.addChoice( MeshExtrudeFacesDirection.EXPAND, _( 'Expand' ) )
		self.perGroup = self.addChoice( MeshExtrudeFacesDirection.PERGROUP, _( 'Per group' ) )



# Field editor for MeshExtrudeFacesDirection

class MeshExtrudeFacesDirectionFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'expand', 'perGroup' ]

	def createElement(self, rowEditContext):
		return MeshExtrudeFacesDirectionCellEditRadioGroup()








class ProcMeshExtrudeFaces (GSProcedure):
	description = _( 'Extrude faces' )

	extrusion = Field( float, 0.0 )
	direction = Field( MeshExtrudeFacesDirection, MeshExtrudeFacesDirection.EXPAND )
	segments = Field( int, 1 )
	bSingular = Field( bool, False )
	bMarkBoundaryEdges = Field( bool, False )
	bMarkExtrudedEdges = Field( bool, False )
	bNSharpBoundaryEdges = Field( bool, True )
	bNSharpExtrudedEdges = Field( bool, True )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		adjuster = MeshExtrudeFacesAdjuster()
		if self.bSingular:
			inputObject.extrudeMarkedFacesSingular( adjuster, self.segments, self.bMarkBoundaryEdges, self.bMarkExtrudedEdges, self.bNSharpBoundaryEdges, self.bNSharpExtrudedEdges )
		else:
			inputObject.extrudeMarkedFaces( adjuster, self.direction, self.segments, self.bMarkBoundaryEdges, self.bMarkExtrudedEdges, self.bNSharpBoundaryEdges, self.bNSharpExtrudedEdges )
		adjuster.setParameters( self.extrusion )
		return inputObject



class ExtrudeFacesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshExtrudeFaces

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.adjuster = MeshExtrudeFacesAdjuster()
		if self._proc.bSingular:
			self.mesh.extrudeMarkedFacesSingular( self.adjuster, self._proc.segments, self._proc.bMarkBoundaryEdges, self._proc.bMarkExtrudedEdges, self._proc.bNSharpBoundaryEdges, self._proc.bNSharpExtrudedEdges )
		else:
			self.mesh.extrudeMarkedFaces( self.adjuster, self._proc.direction, self._proc.segments, self._proc.bMarkBoundaryEdges, self._proc.bMarkExtrudedEdges, self._proc.bNSharpBoundaryEdges, self._proc.bNSharpExtrudedEdges )

	def adjust(self):
		self.adjuster.setParameters( self._proc.extrusion )

	def getResult(self):
		return self.mesh




class ToolMeshExtrudeFaces (GSProcedureTool):
	sheet = SheetRefField( ProcMeshExtrudeFaces )

	functionClass = ProcMeshExtrudeFaces
	adjustableInvoker = ExtrudeFacesAdjustableInvoker
	pageTitle = _( 'Extrude' )
	titleText = _( 'Extrude Faces' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return product.getMarkedFacesCentre()
		else:
			return None

	focalPoint = FunctionField( _p_focalPoint )

	directionLabel = FieldLabelWithFn( sheet.direction, _( 'Direction:' ) )
	directionRadio = MeshExtrudeFacesDirectionFieldEditRadioGroup( sheet.direction )
	extrudeLabel = FieldLabelWithFn( sheet.extrusion, _( 'Extrude:' ) )
	extrudeEntry = FloatFieldEditEntry( sheet.extrusion, -1.0e8, 1.0e8, 0.1, 10.0, 4 )
	bSingularCheck = BoolFieldEditCheck( sheet.bSingular, _( 'Singular' ) )
	segmentsLabel = FieldLabelWithFn( sheet.segments, _( 'Segments:' ) )
	segmentsEntry = IntFieldEditEntry( sheet.segments, 1, 1048576, 1, 10 )
	bMarkBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkBoundaryEdges, _( 'Mark boundary edges' ) )
	bMarkExtrudedEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkExtrudedEdges, _( 'Mark extruded edges' ) )
	bNSharpBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bNSharpBoundaryEdges, _( 'N-sharp boundary edges' ) )
	bNSharpExtrudedEdgesCheck = BoolFieldEditCheckWithFn( sheet.bNSharpExtrudedEdges, _( 'N-sharp extruded edges' ) )

	extrudeManip = Real1FieldManipulator( sheet.extrusion, -1.0e8, 1.0e8, RealFieldManipulator.WorldSpaceScaleFactory( focalPoint ),
									snapping=FieldManipulatorSnappingDistance() )
	extrudeManip.enable()

	numSegmentsManip = IntFieldScrollManipulator( sheet.segments, 1, 1048576 )
	numSegmentsManip.enable()


	def _p_notSingular(self):
		if self.sheet is not None:
			return not self.sheet.bSingular
		else:
			return True

	_notSingular = FunctionField( _p_notSingular )
	directionRadio.sensitivityField = _notSingular


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Use the scroll wheel to change the number of segments' ) )
	layout.row()
	layout.row()  <<  bSingularCheck.check
	layout.row()
	layout.row()  <<  directionLabel.label  <<  directionRadio.expand
	layout.row()  <<  None  <<  directionRadio.perGroup
	layout.row()
	layout.row()  <<  extrudeLabel.label  <<  extrudeEntry.entry
	layout.row()  <<  segmentsLabel.label  <<  segmentsEntry.entry
	layout.row()
	layout.row()  <<  bMarkBoundaryEdgesCheck.check
	layout.row()  <<  bMarkExtrudedEdgesCheck.check
	layout.row()  <<  bNSharpBoundaryEdgesCheck.check
	layout.row()  <<  bNSharpExtrudedEdgesCheck.check
	layout.row()
	layout.finalise()
