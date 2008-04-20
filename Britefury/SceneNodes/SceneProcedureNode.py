##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.ProceduralCore.GSSceneNode import *

from Britefury.ProceduralTool.GSSceneNodeTool import *




class SceneProcedureNode (GSSceneNode):
	description = _( 'Procedure' )


	inputModel = SceneNodeSinkSingleField( 'in', _( 'Input model sink' ) )


	def computeInitialProduct(self, errorHandler):
		if len( self.inputModel ) > 0:
			return self.inputModel.getInputProduct( 0 )
		else:
			return None

	def computeInitialProductClass(self):
		if len( self.inputModel ) > 0:
			return self.inputModel.getInputProductClass( 0 )
		else:
			return None



class SceneProcedureNodeTool (GSSceneNodeTool):
	functionClass = SceneProcedureNode
	pageTitle = _( 'Procedure' )
	titleText = _( 'Procedure' )
