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
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import GSProductMesh



class ProcMeshShrinkVertexSelection (GSProcedure):
	description = _( 'Shrink vertex selection' )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.shrinkMarkedVertices()
		return inputObject



class ToolMeshShrinkVertexSelection (GSProcedureTool):
	sheet = SheetRefField( ProcMeshShrinkVertexSelection )

	functionClass = ProcMeshShrinkVertexSelection
	pageTitle = _( 'Shrink' )
	titleText = _( 'Shrink Vertex Selection' )
