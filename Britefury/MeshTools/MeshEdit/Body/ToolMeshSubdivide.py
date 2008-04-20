##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Vector3

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegisterPrimitive

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Kernel.Enum import Enum

from Britefury.Cell.Cell import *

from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model.MarkPredicateFieldEditor import *

from Britefury.Mesh.Mesh import GSProductMesh, MeshPreserveNormalSharpness
from Britefury.Mesh.MeshPreserveNormalSharpnessFieldEdit import *






# MeshSubdivisionScheme enumeration
class MeshSubdivisionScheme (Enum):
	LINEAR = 0
	CATMULL_CLARK = 1




# XML IO for MeshSubdivisionScheme

subdivSchemeIOXmlEnum = IOXmlEnum( MeshSubdivisionScheme, 'MeshSubdivisionScheme', MeshSubdivisionScheme.CATMULL_CLARK )
subdivSchemeIOXmlEnum.value( MeshSubdivisionScheme.LINEAR, 'linear' )
subdivSchemeIOXmlEnum.value( MeshSubdivisionScheme.CATMULL_CLARK, 'catmull_clark' )

ioXmlReadMeshSubdivisionSchemeProp = subdivSchemeIOXmlEnum.propReader
ioXmlReadMeshSubdivisionSchemeNode = subdivSchemeIOXmlEnum.nodeReader
ioXmlWriteMeshSubdivisionSchemeProp = subdivSchemeIOXmlEnum.propWriter
ioXmlWriteMeshSubdivisionSchemeNode = subdivSchemeIOXmlEnum.nodeWriter


# Cell for MeshSubdivisionScheme

class MeshSubdivisionSchemeCell (Cell):
	valueClass = MeshSubdivisionScheme
	bAllowNone = False

registerCellType( MeshSubdivisionScheme, MeshSubdivisionSchemeCell )


# Cell editor for MeshSubdivisionScheme

class MeshSubdivisionSchemeCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshSubdivisionSchemeCellEditRadioGroup, self ).__init__()
		self.linear = self.addChoice( MeshSubdivisionScheme.LINEAR, _( 'Linear' ) )
		self.catmullClark = self.addChoice( MeshSubdivisionScheme.CATMULL_CLARK, _( 'Catmull-Clark' ) )


# Field editor for MeshSubdivisionScheme

class MeshSubdivisionSchemeFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'linear', 'catmullClark' ]

	def createElement(self, rowEditContext):
		return MeshSubdivisionSchemeCellEditRadioGroup()






class ProcMeshSubdivide (GSProcedure):
	description = _( 'Subdivide' )

	bMarkedFacesOnly = Field( bool, False )
	scheme = Field( MeshSubdivisionScheme, MeshSubdivisionScheme.CATMULL_CLARK )
	preserveNormalSharpness = Field( MeshPreserveNormalSharpness, MeshPreserveNormalSharpness.IF_SHARP )

	inputClass = GSProductMesh
	outputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		bCatmullClark = self.scheme == MeshSubdivisionScheme.CATMULL_CLARK
		inputObject.subdivide( self.bMarkedFacesOnly, bCatmullClark, self.preserveNormalSharpness )
		return inputObject




class ToolMeshSubdivide (GSProcedureTool):
	sheet = SheetRefField( ProcMeshSubdivide )

	functionClass = ProcMeshSubdivide
	pageTitle = _( 'Subd.' )
	titleText = _( 'Subdivide' )

	bMarkedFacesOnlyCheck = BoolFieldEditCheckWithFn( sheet.bMarkedFacesOnly, _( 'Marked faces only' ) )
	schemeLabel = FieldLabelWithFn( sheet.scheme, _( 'Scheme:' ) )
	schemeRadio = MeshSubdivisionSchemeFieldEditRadioGroup( sheet.scheme )
	preserveSharpnessLabel = FieldLabelWithFn( sheet.preserveNormalSharpness, _( 'Preserve normal sharpness:' ) )
	preserveSharpnessRadio = MeshPreserveNormalSharpnessFieldEditRadioGroup( sheet.preserveNormalSharpness )

	layout = FormLayoutFactory()
	layout.row()  <<  bMarkedFacesOnlyCheck.check
	layout.row()
	layout.row()  <<  schemeLabel.label
	layout.row()  <<  schemeRadio.linear
	layout.row()  <<  schemeRadio.catmullClark
	layout.row()
	layout.row()  <<  preserveSharpnessLabel.label
	layout.row()  <<  preserveSharpnessRadio.preserve
	layout.row()  <<  preserveSharpnessRadio.ifSharp
	layout.row()  <<  preserveSharpnessRadio.smooth
	layout.row()
	layout.finalise()
