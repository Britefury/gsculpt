##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIActionInstance, UIActionInstanceSubmenu, UIActionInstanceSignal
from Britefury.UI.AcceleratorGroup import AcceleratorGroup

from Britefury.Tool.ToolGroup import ToolGroup



class ToolGroupActionInstanceProc (UIActionInstance):
	def __init__(self, action, procToolGroup, procFactory, toolInitialiser=None):
		super( ToolGroupActionInstanceProc, self ).__init__( action )
		self._procToolGroup = procToolGroup
		self._procFactory = procFactory
		self._toolInitialiser = toolInitialiser
		procToolGroup._p_registerAction( self )


	def _o_activate(self):
		proc = self._procFactory()
		tool = self._procToolGroup._nodeEditor.addProcedureSelectAndEdit( proc )
		if self._toolInitialiser is not None:
			self._toolInitialiser( tool )



class ToolGroupActionInstanceSubmenu (UIActionInstanceSubmenu):
	def __init__(self, action, procToolGroup):
		super( ToolGroupActionInstanceSubmenu, self ).__init__( action )
		self._procToolGroup = procToolGroup
		procToolGroup._p_registerAction( self )




class ToolGroupActionInstanceSignal (UIActionInstanceSignal):
	def __init__(self, action, procToolGroup):
		super( ToolGroupActionInstanceSignal, self ).__init__( action )
		self._procToolGroup = procToolGroup
		procToolGroup._p_registerAction( self )




class ProcToolGroup (ToolGroup):
	def __init__(self, nodeEditor):
		self._nodeEditor = nodeEditor
		self._accelGroup = AcceleratorGroup()
		self._actions = []
		self._bInstallAccelerators = True


	def installAccelerators(self, acceleratorManager):
		if self._bInstallAccelerators:
			self._accelGroup.install( acceleratorManager )

	def uninstallAccelerators(self, acceleratorManager):
		if self._bInstallAccelerators:
			self._accelGroup.uninstall( acceleratorManager )


	def _p_registerAction(self, action):
		action.installAccel( self._accelGroup )
