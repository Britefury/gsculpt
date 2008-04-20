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

from Britefury.ProceduralCore import UnknownProduct
from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool

from Britefury.Group.Group import GSProductGroup



class ProcGroupMergeContents (GSProcedure):
	description = _( 'Merge contents' )

	inputClass = GSProductGroup
	outputClass = UnknownProduct

	def procedureInvoke(self, inputObject, errorHandler):
		return inputObject.merged()



class ToolGroupMergeContents (GSProcedureTool):
	sheet = SheetRefField( ProcGroupMergeContents )

	functionClass = ProcGroupMergeContents
	pageTitle = _( 'Merge' )
	titleText = _( 'Merge Contents' )

	layout = FormLayoutFactory()
	layout.row()
	layout.finalise()
