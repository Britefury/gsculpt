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
from Britefury.Mesh.Mesh import GSProductMesh, MeshTransformation




class ProcMeshEditMove (GSProcedure):
	description = _( 'Abstract move' )

	translation = Field( Vector3, Vector3() )

	inputClass = GSProductMesh



class ToolMeshEditMove (GSProcedureTool):
	sheet = SheetRefField( ProcMeshEditMove )

	functionClass = ProcMeshEditMove
	pageTitle = _( 'Move' )
	titleText = _( 'Abstract Move' )

	translationLabel = FieldLabelWithFn( sheet.translation, _( 'Move:' ) )
	translationEntry = Vector3FieldEditEntry( sheet.translation, -1.0e8, 1.0e8, 0.1, 10.0, 4 )

	layout = FormLayoutFactory()
	layout.row()  <<  translationLabel.label
	layout.row()  <<  'X:'  <<  translationEntry.x
	layout.row()  <<  'Y:'  <<  translationEntry.y
	layout.row()  <<  'Z:'  <<  translationEntry.z
	layout.row()
	layout.finalise()
