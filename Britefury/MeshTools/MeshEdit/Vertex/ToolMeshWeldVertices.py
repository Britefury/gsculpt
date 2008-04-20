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



class ProcMeshWeldVertices (GSProcedure):
	description = _( 'Weld vertices' )

	threshold = Field( float, 0.0 )
	bWeldLinkedVertices = Field( bool, True )
	bMarkMergedEdges = Field( bool, False )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.weldMarkedVertices( self.threshold, self.bWeldLinkedVertices, self.bMarkMergedEdges )
		return inputObject



class ToolMeshWeldVertices (GSProcedureTool):
	sheet = SheetRefField( ProcMeshWeldVertices )

	functionClass = ProcMeshWeldVertices
	pageTitle = _( 'Weld' )
	titleText = _( 'Weld Vertices' )

	thresholdLabel = FieldLabelWithFn( sheet.threshold, _( 'Threshold:' ) )
	thresholdEntry = FloatFieldEditEntry( sheet.threshold, 0.0, 1.0e8, 0.01, 1.0, 4 )
	bWeldLinkedVerticesCheck = BoolFieldEditCheckWithFn( sheet.bWeldLinkedVertices, _( 'Weld linked vertices' ) )
	bMarkMergedEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkMergedEdges, _( 'Mark merged edges' ) )

	layout = FormLayoutFactory()
	layout.row()  <<  thresholdLabel.label  <<  thresholdEntry.entry
	layout.row()  <<  bWeldLinkedVerticesCheck.check
	layout.row()  <<  bMarkMergedEdgesCheck.check
	layout.row()
	layout.finalise()
