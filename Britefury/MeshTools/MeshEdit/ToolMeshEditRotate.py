##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Vector3, Matrix4, Quaternion

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import GSProductMesh




class ProcMeshEditRotate (GSProcedure):
	description = _( 'Abstract rotate' )

	bPerGroup = Field( bool, True )
	rotation = Field( Quaternion, Quaternion( 0.0, 0.0, 0.0, 1.0 ) )

	inputClass = GSProductMesh




class ToolMeshEditRotate (GSProcedureTool):
	sheet = SheetRefField( ProcMeshEditRotate )

	functionClass = ProcMeshEditRotate
	pageTitle = _( 'Rotate' )
	titleText = _( 'Abstract Rotate' )

	bPerGroupCheck = BoolFieldEditCheckWithFn( sheet.bPerGroup, _( 'Per group' ) )
	rotationLabel = FieldLabelWithFn( sheet.rotation, _( 'Rotate:' ) )
	rotationEntry = QuaternionFieldEditEntry( sheet.rotation )

	layout = FormLayoutFactory()
	layout.row()  <<  bPerGroupCheck.check
	layout.row()
	layout.row()  <<  rotationLabel.label
	layout.row()  <<  'X:'  <<  rotationEntry.x
	layout.row()  <<  'Y:'  <<  rotationEntry.y
	layout.row()  <<  'Z:'  <<  rotationEntry.z
	layout.row()
	layout.finalise()
