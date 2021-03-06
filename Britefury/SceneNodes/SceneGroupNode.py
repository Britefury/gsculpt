##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.ProceduralCore.GSSceneNode import *

from Britefury.ProceduralTool.GSSceneNodeTool import *

from Britefury.Group.Group import GSProductGroup



class SceneGroupNode (GSSceneNode):
	description = _( 'Group' )


	models = SceneNodeSinkMultipleField( 'models', _( 'Models' ) )


	def computeInitialProduct(self, errorHandler):
		group = GSProductGroup()
		group.extend( self.models.getInputProducts() )
		return group

	def computeInitialProductClass(self):
		return GSProductGroup



class SceneGroupNodeTool (GSSceneNodeTool):
	functionClass = SceneGroupNode
	pageTitle = _( 'Group' )
	titleText = _( 'Group' )
