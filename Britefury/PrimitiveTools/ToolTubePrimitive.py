##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Point3, Matrix4, Axis
from Britefury.Math.MathCellTypes import *

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegisterPrimitiveWithReaderAndWriter

from Britefury.Model.Model import GSProductModelBlank

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Cell.Cell import *

from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import TubePrimitive, TubePrimitiveCapMaterialMapping, MeshVertexList, GSProductMesh




# XML IO for TubePrimitiveCapMaterialMapping

capMappingIOXmlEnum = IOXmlEnum( TubePrimitiveCapMaterialMapping, 'TubePrimitiveCapMaterialMapping', TubePrimitiveCapMaterialMapping.PLANAR )
capMappingIOXmlEnum.value( TubePrimitiveCapMaterialMapping.PLANAR, 'planar' )
capMappingIOXmlEnum.value( TubePrimitiveCapMaterialMapping.POLAR, 'polar' )

ioXmlReadTubePrimitiveCapMaterialMappingProp = capMappingIOXmlEnum.propReader
ioXmlReadTubePrimitiveCapMaterialMappingNode = capMappingIOXmlEnum.nodeReader
ioXmlWriteTubePrimitiveCapMaterialMappingProp = capMappingIOXmlEnum.propWriter
ioXmlWriteTubePrimitiveCapMaterialMappingNode = capMappingIOXmlEnum.nodeWriter



# Cell for TubePrimitiveCapMaterialMapping

class TubePrimitiveCapMaterialMappingCell (Cell):
	valueClass = TubePrimitiveCapMaterialMapping
	bAllowNone = False

registerCellType( TubePrimitiveCapMaterialMapping, TubePrimitiveCapMaterialMappingCell )


# Cell editor for TubePrimitiveCapMaterialMapping

class TubePrimitiveCapMaterialMappingCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( TubePrimitiveCapMaterialMappingCellEditRadioGroup, self ).__init__()
		self.planar = self.addChoice( TubePrimitiveCapMaterialMapping.PLANAR, _( 'Planar' ) )
		self.polar = self.addChoice( TubePrimitiveCapMaterialMapping.POLAR, _( 'Polar' ) )


# Field editor for TubePrimitiveCapMaterialMapping

class TubePrimitiveCapMaterialMappingFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ =  [ 'planar', 'polar' ]

	def createElement(self, sheetEditContext):
		return TubePrimitiveCapMaterialMappingCellEditRadioGroup()





class ProcTubePrimitive (GSProcedure):
	description = _( 'Tube' )

	axis = Field( Axis, Axis.AXIS_Y )
	slices = Field( int, 12 )
	lengthSegments = Field( int, 1 )
	radialSegments = Field( int, 1 )
	innerRadius = Field( float, 40.0 )
	outerRadius = Field( float, 50.0 )
	length = Field( float, 100.0 )
	bInnerSection = Field( bool, True )
	bOuterSection = Field( bool, True )
	bCapStart = Field( bool, True )
	bCapEnd = Field( bool, True )
	capMapping = Field( TubePrimitiveCapMaterialMapping, TubePrimitiveCapMaterialMapping.PLANAR )
	position = Field( Point3, Point3() )


	inputClass = GSProductModelBlank
	outputClass = GSProductMesh


	def procedureInvoke(self, inputObject, errorHandler):
		prim = TubePrimitive( self.axis, self.radialSegments, self.lengthSegments, self.slices, self.innerRadius, self.outerRadius, self.length, self.bInnerSection, self.bOuterSection, self.bCapStart, self.bCapEnd, self.capMapping )
		mesh = GSProductMesh()
		verts = MeshVertexList()
		prim.createTube( mesh, verts )
		verts.transform( Matrix4.translate( self.position.toVector3() ), mesh )
		return mesh



class TubeAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcTubePrimitive

	def initInvoke(self):
		self.verts = MeshVertexList()

	def main(self):
		prim = TubePrimitive( self._proc.axis, self._proc.radialSegments, self._proc.lengthSegments, self._proc.slices, self._proc.innerRadius, self._proc.outerRadius, self._proc.length, self._proc.bInnerSection, self._proc.bOuterSection, self._proc.bCapStart, self._proc.bCapEnd, self._proc.capMapping )
		self.verts.clear()
		self.mesh = GSProductMesh()
		prim.createTube( self.mesh, self.verts )
		return self.mesh

	def save(self):
		self.verts.savePositions( self.mesh )

	def restore(self):
		self.verts.restorePositions( self.mesh )

	def adjust(self):
		self.verts.transform( Matrix4.translate( self._proc.position.toVector3() ), self.mesh )

	def getResult(self):
		return self.mesh




class ToolTubePrimitive (GSProcedureTool):
	sheet = SheetRefField( ProcTubePrimitive )

	functionClass = ProcTubePrimitive
	adjustableInvoker = TubeAdjustableInvoker
	pageTitle = _( 'Tube' )
	titleText = _( 'Create Tube' )

	axisLabel = FieldLabelWithFn( sheet.axis, _( 'Tube axis:' ) )
	axisRadio = AxisFieldEditRadioGroup( sheet.axis )
	slicesLabel = FieldLabelWithFn( sheet.slices, _( 'Slices:' ) )
	slicesEntry = IntFieldEditEntry( sheet.slices, 3, 1048576, 1, 10 )
	lengthSegsLabel = FieldLabelWithFn( sheet.lengthSegments, _( 'Length segs.:' ) )
	lengthSegsEntry = IntFieldEditEntry( sheet.lengthSegments, 1, 1048576, 1, 10 )
	radialSegsLabel = FieldLabelWithFn( sheet.radialSegments, _( 'Radial segs.:' ) )
	radialSegsEntry = IntFieldEditEntry( sheet.radialSegments, 1, 1048576, 1, 10 )
	innerRadiusLabel = FieldLabelWithFn( sheet.innerRadius, _( 'Inner radius:' ) )
	innerRadiusEntry = FloatFieldEditEntry( sheet.innerRadius, 0.0, 1.0e8, 0.1, 10.0, 4 )
	outerRadiusLabel = FieldLabelWithFn( sheet.outerRadius, _( 'Outer radius:' ) )
	outerRadiusEntry = FloatFieldEditEntry( sheet.outerRadius, 0.0, 1.0e8, 0.1, 10.0, 4 )
	lengthLabel = FieldLabelWithFn( sheet.length, _( 'Length:' ) )
	lengthEntry = FloatFieldEditEntry( sheet.length, 0.0, 1.0e8, 1.0, 10.0, 4 )
	bInnerSectionCheck = BoolFieldEditCheckWithFn( sheet.bInnerSection, _( 'Inner section' ) )
	bOuterSectionCheck = BoolFieldEditCheckWithFn( sheet.bOuterSection, _( 'Outer section' ) )
	bCapStartCheck = BoolFieldEditCheckWithFn( sheet.bCapStart, _( 'Cap start' ) )
	bCapEndCheck = BoolFieldEditCheckWithFn( sheet.bCapEnd, _( 'Cap end' ) )
	capMappingLabel = FieldLabelWithFn( sheet.capMapping, _( 'Cap material mapping:' ) )
	capMappingRadio = TubePrimitiveCapMaterialMappingFieldEditRadioGroup( sheet.capMapping )
	positionLabel = FieldLabelWithFn( sheet.position, _( 'Position:' ) )
	positionEntry = Point3FieldEditEntry( sheet.position, -1.0e8, 1.0e8, 1.0, 10.0, 4 )

	positionManip = Point3FieldManipulator( sheet.position, snapping=FieldManipulatorSnappingDistance() )
	positionManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  axisLabel.label
	layout.row()  <<  axisRadio.x  <<  axisRadio.y  <<  axisRadio.z
	layout.row()
	layout.row()  <<  slicesLabel.label  <<  slicesEntry.entry
	layout.row()  <<  lengthSegsLabel.label  <<  lengthSegsEntry.entry
	layout.row()  <<  radialSegsLabel.label  <<  radialSegsEntry.entry
	layout.row()
	layout.row()  <<  innerRadiusLabel.label  <<  innerRadiusEntry.entry
	layout.row()  <<  outerRadiusLabel.label  <<  outerRadiusEntry.entry
	layout.row()  <<  lengthLabel.label  <<  lengthEntry.entry
	layout.row()
	layout.row()  <<  bInnerSectionCheck.check  <<  bOuterSectionCheck.check
	layout.row()  <<  bCapStartCheck.check  <<  bCapEndCheck.check
	layout.row()
	layout.row()  <<  capMappingLabel.label
	layout.row()  <<  capMappingRadio.planar  <<  capMappingRadio.polar
	layout.row()
	layout.row()  <<  positionLabel.label
	layout.row()  <<  'X:'  <<  positionEntry.x
	layout.row()  <<  'Y:'  <<  positionEntry.y
	layout.row()  <<  'Z:'  <<  positionEntry.z
	layout.row()
	layout.finalise()
