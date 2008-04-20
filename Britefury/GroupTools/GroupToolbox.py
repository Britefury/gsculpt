##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.ProceduralTool.GSSceneNodeToolbox import GSSceneNodeToolbox

from Britefury.GroupTools.GroupToolkit import GroupToolkit



class GroupToolbox (GSSceneNodeToolbox):
	def __init__(self, acceleratorManager, view, nodeEditor, editorSettings, parentWindow, commandHistory):
		super( GroupToolbox, self ).__init__( _( 'Group' ), acceleratorManager, view, editorSettings, parentWindow, commandHistory )
		primToolkit = GroupToolkit( nodeEditor )
		self.addToolkit( primToolkit, _( 'Group' ) )

