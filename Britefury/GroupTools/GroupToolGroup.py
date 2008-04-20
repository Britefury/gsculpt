##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIAction

from Britefury.ProceduralTool.ProcToolGroup import ProcToolGroup, ToolGroupActionInstanceProc, ToolGroupActionInstanceSubmenu

from Britefury.GroupTools.ToolGroupMergeContents import ProcGroupMergeContents




_mergeAction = UIAction( 'Tools.Group.Group.MergeContents', ( _( 'Tools' ), _( 'Group' ), _( 'Group' ), _( 'MergeContents' ) ),
			_( 'Merge contents' ), _( 'Merge contents' ), 'm', _( 'Merge contents' ) )



class GroupToolGroup (ProcToolGroup):
	def __init__(self, nodeEditor):
		super( GroupToolGroup, self ).__init__( nodeEditor )
		self._mergeAction = _mergeAction.instance( ToolGroupActionInstanceProc, self, ProcGroupMergeContents )


	def layoutContents(self, layout):
		layout.row()  <<  _( 'Group' )
		layout.row()  <<  ( self._mergeAction.button(), 2 )  <<  None  <<  None
		layout.row()


	def initContextMenu(self, contextMenu):
		contextMenu.layout.row()  <<  ( self._mergeAction.contextMenuItem( contextMenu ), 2 )


