##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Sheet.Sheet import *

from Britefury.MarkTools.CircularMarkRegionManipulator import CircularMarkRegionFieldManipulator

from Britefury.MeshTools.Mark.ToolMeshMarkRegion import ProcMeshMarkRegion, ToolMeshMarkRegion, MeshMarkRegionTarget

from Britefury.Mesh.Mesh import GSProductMesh





class ProcMeshMarkCircular (ProcMeshMarkRegion):
	description = 'Circular select'
	inputClass = GSProductMesh


	def getDescription(self):
		if self.target == MeshMarkRegionTarget.VERTICES:
			return 'Circular select vertices'
		elif self.target == MeshMarkRegionTarget.EDGES:
			return 'Circular select edges'
		elif self.target == MeshMarkRegionTarget.FACES:
			return 'Circular select faces'





class ToolMeshMarkCircular (ToolMeshMarkRegion):
	sheet = SheetRefField( ProcMeshMarkCircular )

	functionClass = ProcMeshMarkCircular
	pageTitle = 'CircSel'
	titleText = 'Circular Select'


	regionManip = CircularMarkRegionFieldManipulator( sheet.predicate, sheet.region, sheet.backfaceCull, sheet.bEnabled )



