##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import GSProductMesh



class ProcMeshFillEdges (GSProcedure):
	description = _( 'Fill edges' )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.fillMarkedEdges()
		return inputObject



class ToolMeshFillEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshFillEdges )

	functionClass = ProcMeshFillEdges
	pageTitle = _( 'Fill' )
	titleText = _( 'Fill edges' )

	layout = FormLayoutFactory()
	layout.textBlock( _( 'Will not work if any non-boundary edges are marked.\n\n'  'All closed edge runs will be filled.\n'  'Will fill up to 2 open edge runs.' ) )
	layout.row()
	layout.finalise()
