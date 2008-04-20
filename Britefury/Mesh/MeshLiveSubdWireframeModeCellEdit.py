##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Mesh.Mesh import MeshLiveSubdWireframeMode
from Britefury.Mesh.MeshLiveSubdWireframeModeCell import *


# Cell editor for MeshLiveSubdWireframeMode

class MeshLiveSubdWireframeModeCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshLiveSubdWireframeModeCellEditRadioGroup, self ).__init__()
		self.none = self.addChoice( MeshLiveSubdWireframeMode.NONE, _( 'None' ) )
		self.control = self.addChoice( MeshLiveSubdWireframeMode.CONTROL, _( 'Control edges' ) )
		self.full = self.addChoice( MeshLiveSubdWireframeMode.FULL, _( 'Full' ) )
