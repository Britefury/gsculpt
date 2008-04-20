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



class ProcMeshSelectEdgeLoop (GSProcedure):
	description = _( 'Select edge loop' )

	bStopAtMarkedFaces = Field( bool, False )
	bCrossFacesMultipleTimes = Field( bool, True )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.markEdgeLoop( self.bStopAtMarkedFaces, self.bCrossFacesMultipleTimes )
		return inputObject



class ToolMeshSelectEdgeLoop (GSProcedureTool):
	sheet = SheetRefField( ProcMeshSelectEdgeLoop )

	functionClass = ProcMeshSelectEdgeLoop
	pageTitle = _( 'Loop' )
	titleText = _( 'Select Edge Loop' )

	bStopAtMarkedFacesCheck = BoolFieldEditCheckWithFn( sheet.bStopAtMarkedFaces, _( 'Stop at marked faces' ) )
	bCrossFacesMultipleTimesCheck = BoolFieldEditCheckWithFn( sheet.bCrossFacesMultipleTimes, _( 'Cross faces multiple times' ) )

	layout = FormLayoutFactory()
	layout.row()  <<  bStopAtMarkedFacesCheck.check
	layout.row()  <<  bCrossFacesMultipleTimesCheck.check
	layout.row()
	layout.finalise()
