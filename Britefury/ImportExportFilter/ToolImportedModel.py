##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Cell.Cell import *

from Britefury.Sheet.Sheet import *

from Britefury.ProceduralCore.Product import GSProduct
from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool

from Britefury.Model.Model import GSProductModelBlank



class GSProductCell (Cell):
	valueClass = GSProduct
	bAllowNone = True

registerCellType( GSProduct, GSProductCell )




class ProcImportedModel (GSProcedure):
	description = 'Imported model'

	model = Field( GSProduct, None )

	inputClass = GSProductModelBlank


	def getOutputClass(self, inputClass):
		if self.immutable.model is None:
			return GSProductModelBlank
		else:
			return type( self.immutable.model )

	def procedureInvoke(self, inputObject, errorHandler):
		if self.immutable.model is None:
			return GSProductModelBlank()
		else:
			# It is *VERY* important that a *COPY* of the model is returned, otherwise
			# the imported model can get altered by further operations.
			return deepcopy( self.immutable.model )



class ToolImportedModel (GSProcedureTool):
	sheet = SheetRefField( ProcImportedModel )

	functionClass = ProcImportedModel
	pageTitle = 'Import'
	titleText = 'Imported Model'

