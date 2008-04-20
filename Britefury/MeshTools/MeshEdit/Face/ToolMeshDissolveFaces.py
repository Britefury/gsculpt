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



class ProcMeshDissolveFaces (GSProcedure):
	description = _( 'Dissolve faces' )

	bMarkDissolvedFaces = Field( bool, True )

	inputClass = GSProductMesh


	# Handle old version of 'ProcMeshDissolveFaces', which did not have the @bMarkDissolvedFaces field.
	# It should default to False. Any newer version will override this value if present.
	def __readxml__(self, xmlNode):
		self.bMarkDissolvedFaces = False
		super( ProcMeshDissolveFaces, self ).__readxml__( xmlNode )


	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.dissolveMarkedFaces( self.bMarkDissolvedFaces )
		return inputObject



class ToolMeshDissolveFaces (GSProcedureTool):
	sheet = SheetRefField( ProcMeshDissolveFaces )

	functionClass = ProcMeshDissolveFaces
	pageTitle = _( 'Dissolve' )
	titleText = _( 'Dissolve Faces' )
