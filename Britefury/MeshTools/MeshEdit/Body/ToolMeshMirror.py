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

from Britefury.Mesh.Mesh import GSProductMesh




class ProcMeshMirror (GSProcedure):
	description = _( 'Mirror' )

	inputClass = GSProductMesh
	outputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.mirror( inputObject.virtualMirrorPlane.plane )
		inputObject.bVirtualMirrorEnabled = False
		return inputObject




class ToolMeshMirror (GSProcedureTool):
	sheet = SheetRefField( ProcMeshMirror )

	functionClass = ProcMeshMirror
	pageTitle = _( 'Mirror' )
	titleText = _( 'Mirror' )

	layout = FormLayoutFactory()
	layout.textBlock( _( 'Note: virtual mirror disabled as a side effect' ) )
	layout.row()  <<  ''
	layout.finalise()
