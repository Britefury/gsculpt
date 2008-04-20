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



class ProcMeshNSplitEdges (GSProcedure):
	description = _( 'N-Split edges' )

	numSplits = Field( int, 1 )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.nSplitMarkedEdges( self.numSplits + 1 )
		return inputObject



class ToolMeshNSplitEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshNSplitEdges )

	functionClass = ProcMeshNSplitEdges
	pageTitle = _( 'N-Split' )
	titleText = _( 'N-Split Edges' )

	numSplitsLabel = FieldLabelWithFn( sheet.numSplits, _( 'Splits:' ) )
	numSplitsEntry = IntFieldEditEntry( sheet.numSplits, 1, 1048576, 1, 10 )

	numSplitsManip = IntFieldScrollManipulator( sheet.numSplits, 1, 1048576 )
	numSplitsManip.enable()

	layout = FormLayoutFactory()
	layout.textBlock( _( 'Use the scroll wheel to change the number of vertices inserted into the marked edges' ) )
	layout.row()
	layout.row()  <<  numSplitsLabel.label  <<  numSplitsEntry.entry
	layout.row()
	layout.finalise()
