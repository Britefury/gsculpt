##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.SheetEdit.SheetEdit import *

from Britefury.Mesh.MeshLiveSubdWireframeModeCellEdit import *


# Field editor for MeshLiveSubdWireframeMode

class MeshLiveSubdWireframeModeFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'none', 'control', 'full' ]

	def createElement(self, rowEditContext):
		return MeshLiveSubdWireframeModeCellEditRadioGroup()


