##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Tool.Toolset import Toolset

from Britefury.GroupTools.GroupToolGroup import GroupToolGroup
from Britefury.TransformationTools.TransformationToolGroup import TransformationToolGroup
from Britefury.TransformationTools.ToolTransformationBase import TransformationTarget


class GroupToolset (Toolset):
	def __init__(self, nodeEditor):
		super( GroupToolset, self ).__init__( _( 'Group' ) )
		deformation = TransformationToolGroup( nodeEditor, TransformationTarget.BODY, True, True )
		group = GroupToolGroup( nodeEditor )
		self.addToolGroup( deformation )
		self.addToolGroup( group )
