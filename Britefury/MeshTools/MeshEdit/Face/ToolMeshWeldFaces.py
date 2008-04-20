##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.SheetEdit import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *
from Britefury.ProceduralTool.GSProcedureTool import *

from Britefury.PolyBlend.PolyBlendComponent import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import GSProductMesh, MeshWeldStyle

from Britefury.MeshTools.MeshEdit.MeshWeldStyle import *



class ProcMeshWeldFaces (GSProcedure):
	description = _( 'Weld faces' )

	blend = CompositeField( PolyBlendComponent )
	style = Field( MeshWeldStyle, MeshWeldStyle.MIDPOINT )


	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.weldMarkedFaces( self.blend.makePolyBlend(), self.style )
		return inputObject



class ToolMeshWeldFaces (GSProcedureTool):
	sheet = SheetRefField( ProcMeshWeldFaces )

	functionClass = ProcMeshWeldFaces
	pageTitle = _( 'Weld' )
	titleText = _( 'Weld Faces' )

	blendEdit = PolyBlendComponentFieldEdit( sheet.blend )
	styleLabel = FieldLabelWithFn( sheet.style, _( 'Style:' ) )
	styleRadio = MeshWeldStyleFieldEditRadioGroup( sheet.style )

	layout = FormLayoutFactory()
	layout.block( 4 )  <<  blendEdit.sheetEditor
	layout.row()
	layout.row()  <<  styleLabel.label
	layout.row()  <<  styleRadio.midPoint
	layout.row()  <<  styleRadio.a
	layout.row()  <<  styleRadio.b
	layout.row()
	layout.finalise()
