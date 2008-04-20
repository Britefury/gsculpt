##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import math

from Britefury.Math.Math import Quaternion

from Britefury.UI.UIAction import UIAction

from Britefury.ProceduralTool.ProcToolGroup import ProcToolGroup, ToolGroupActionInstanceProc, ToolGroupActionInstanceSubmenu

from Britefury.ModelTools.ToolVirtualMirror import ProcVirtualMirror



_virtualMirrorAction = UIAction( 'Tools.Model.VirtualMirror', ( _( 'Tools' ), _( 'Model' ), _( 'Virtual mirror' ) ),
			_( 'Virtual mirror' ), _( 'Virtual mirror' ), None, _( 'Change virtual mirror settings' ) )



class ModelVirtualMirrorToolGroup (ProcToolGroup):
	def __init__(self, nodeEditor):
		super( ModelVirtualMirrorToolGroup, self ).__init__( nodeEditor )

		self._virtualMirrorAction = _virtualMirrorAction.instance( ToolGroupActionInstanceProc, self, self._p_onVirtualMirror )

		self._modelCell = None


	def layoutContents(self, layout):
		layout.row()  <<  _( 'Virtual mirror' )
		layout.row()  <<  self._virtualMirrorAction.button()  <<  None  <<  None
		layout.row()


	def attachModelCell(self, cell):
		self._modelCell = cell

	def detachModelCell(self):
		self._modelCell = None


	def initContextMenu(self, contextMenu):
		contextMenu.layout.row()  <<  self._virtualMirrorAction.contextMenuItem( contextMenu )


	def _p_onVirtualMirror(self):
		proc = ProcVirtualMirror()
		model = self._modelCell.getImmutableValue()

		if model is not None  and  model.bVirtualMirrorEnabled:
			proc.position = model.virtualMirrorPlane.position
			proc.rotation = model.virtualMirrorPlane.rotation
			proc.size = model.virtualMirrorPlane.size
			proc.bVisible = model.virtualMirrorPlane.bVisible
		else:
			proc.rotation = Quaternion.rotateY( math.radians( -90.0 ) )
		proc.bEnabled = True

		return proc

