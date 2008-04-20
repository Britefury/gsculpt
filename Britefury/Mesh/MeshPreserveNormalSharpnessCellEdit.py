##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Mesh.Mesh import MeshPreserveNormalSharpness
from Britefury.Mesh.MeshPreserveNormalSharpnessCell import *


# Cell editor for MeshPreserveNormalSharpness

class MeshPreserveNormalSharpnessCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshPreserveNormalSharpnessCellEditRadioGroup, self ).__init__()
		self.preserve = self.addChoice( MeshPreserveNormalSharpness.PRESERVE, _( 'Preserve' ) )
		self.ifSharp = self.addChoice( MeshPreserveNormalSharpness.IF_SHARP, _( 'If sharp' ) )
		self.smooth = self.addChoice( MeshPreserveNormalSharpness.SMOOTH, _( 'Smooth' ) )
