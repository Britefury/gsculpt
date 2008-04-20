##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Vector3

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import GSProductMesh




class ProcMeshEditScale (GSProcedure):
	description = _( 'Abstract scale' )

	bPerGroup = Field( bool, True )
	scale = Field( Vector3, Vector3( 1.0, 1.0, 1.0 ) )

	inputClass = GSProductMesh




class ToolMeshEditScale (GSProcedureTool):
	sheet = SheetRefField( ProcMeshEditScale )

	functionClass = ProcMeshEditScale
	pageTitle = _( 'Scale' )
	titleText = _( 'Abstract scale' )

	bPerGroupCheck = BoolFieldEditCheckWithFn( sheet.bPerGroup, _( 'Per group' ) )
	scaleLabel = FieldLabelWithFn( sheet.scale, _( 'Scale:' ) )
	scaleEntry = Vector3FieldEditEntry( sheet.scale, -1.0e8, 1.0e8, 0.001, 0.1, 4 )

	layout = FormLayoutFactory()
	layout.row()  << bPerGroupCheck.check
	layout.row()
	layout.row()  <<  scaleLabel.label
	layout.row()  <<  'X:'  <<  scaleEntry.x
	layout.row()  <<  'Y:'  <<  scaleEntry.y
	layout.row()  <<  'Z:'  <<  scaleEntry.z
	layout.row()
	layout.finalise()
