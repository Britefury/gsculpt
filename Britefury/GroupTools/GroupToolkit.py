##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIAction

from Britefury.Tool.Toolkit import Toolkit

from Britefury.GroupTools.GroupToolset import GroupToolset



_groupAction = UIAction( 'Toolsets.Group.Group', ( _( 'Toolsets' ), _( 'Group' ), _( 'Group tools' ) ), accelerator='<alt>g', tooltip=_( 'Group tools' ) )



class GroupToolkit (Toolkit):
	def __init__(self, nodeEditor):
		super( GroupToolkit, self ).__init__()
		groupToolset = GroupToolset( nodeEditor )
		self.addToolset( groupToolset, 'group.png', _groupAction )
