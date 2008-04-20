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

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import GSProductMesh



class ProcMeshNConnectEdges (GSProcedure):
	description = _( 'N-Connect edges' )

	numCuts = Field( int, 1 )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.nConnectMarkedEdges( self.numCuts + 1 )
		return inputObject



class ToolMeshNConnectEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshNConnectEdges )

	functionClass = ProcMeshNConnectEdges
	pageTitle = _( 'N-Conn.' )
	titleText = _( 'N-Connect Edges' )

	numCutsLabel = FieldLabelWithFn( sheet.numCuts, _( 'Num. cuts:' ) )
	numCutsEntry = IntFieldEditEntry( sheet.numCuts, 1, 1048576, 1, 10 )

	numCutsManip = IntFieldScrollManipulator( sheet.numCuts, 1, 1048576 )
	numCutsManip.enable()

	layout = FormLayoutFactory()
	layout.textBlock( _( 'Use the scroll wheel to change the number of cuts' ) )
	layout.row()
	layout.row()  <<  numCutsLabel.label  <<  numCutsEntry.entry
	layout.row()
	layout.finalise()
