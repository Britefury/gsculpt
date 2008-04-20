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



class ProcMeshDissolveVertices (GSProcedure):
	description = _( 'Dissolve vertices' )

	bMarkCreatedFaces = Field( bool, False )
	bTwoEdgesOnly = Field( bool, False )
	bTwoFacesOnly = Field( bool, False )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.dissolveMarkedVertices( self.bTwoEdgesOnly, self.bTwoFacesOnly, self.bMarkCreatedFaces )
		return inputObject



class ToolMeshDissolveVertices (GSProcedureTool):
	sheet = SheetRefField( ProcMeshDissolveVertices )

	functionClass = ProcMeshDissolveVertices
	pageTitle = _( 'Dissolve' )
	titleText = _( 'Dissolve Vertices' )

	bMarkCreatedFacesCheck = BoolFieldEditCheckWithFn( sheet.bMarkCreatedFaces, _( 'Mark created faces' ) )
	bTwoEdgesOnlyCheck = BoolFieldEditCheckWithFn( sheet.bTwoEdgesOnly, _( 'Two edges only' ) )
	bTwoFacesOnlyCheck = BoolFieldEditCheckWithFn( sheet.bTwoFacesOnly, _( 'Two faces only' ) )

	layout = FormLayoutFactory()
	layout.row()  <<  bMarkCreatedFacesCheck.check
	layout.row()  <<  bTwoEdgesOnlyCheck.check
	layout.row()  <<  bTwoFacesOnlyCheck.check
	layout.row()
	layout.finalise()
