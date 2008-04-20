##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.ProceduralTool.GSSceneNodeToolbox import GSSceneNodeToolbox

from Britefury.PrimitiveTools.PrimitiveToolkit import PrimitiveToolkit



class PrimitiveToolbox (GSSceneNodeToolbox):
	def __init__(self, acceleratorManager, view, nodeEditor, editorSettings, parentWindow, commandHistory):
		super( PrimitiveToolbox, self ).__init__( _( 'Prim.' ), acceleratorManager, view, editorSettings, parentWindow, commandHistory )
		primToolkit = PrimitiveToolkit( nodeEditor )
		self.addToolkit( primToolkit, _( 'Primitives' ) )

