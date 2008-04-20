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



class ProcMeshSpinEdges (GSProcedure):
	description = _( 'Spin edges' )

	turns = Field( int, 0 )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.spinMarkedEdges( self.turns )
		return inputObject



class ToolMeshSpinEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshSpinEdges )

	functionClass = ProcMeshSpinEdges
	pageTitle = _( 'Spin' )
	titleText = _( 'Spin Edges' )

	turnsLabel = FieldLabelWithFn( sheet.turns, _( 'Turns:' ) )
	turnsEntry = IntFieldEditEntry( sheet.turns, -1048576, 1048576, 1, 10 )

	turnsManip = IntFieldScrollManipulator( sheet.turns, -1048576, 1048576 )
	turnsManip.enable()

	layout = FormLayoutFactory()
	layout.textBlock( _( 'Use the scroll wheel to change the number of turns' ) )
	layout.row()
	layout.row()  <<  turnsLabel.label  <<  turnsEntry.entry
	layout.row()
	layout.finalise()
