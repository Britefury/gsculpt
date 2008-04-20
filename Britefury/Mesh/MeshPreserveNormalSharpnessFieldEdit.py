##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.SheetEdit.SheetEdit import *

from Britefury.Mesh.MeshPreserveNormalSharpnessCellEdit import *


# Field editor for MeshPreserveNormalSharpness

class MeshPreserveNormalSharpnessFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'preserve', 'ifSharp', 'smooth' ]

	def createElement(self, rowEditContext):
		return MeshPreserveNormalSharpnessCellEditRadioGroup()


