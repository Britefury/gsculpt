##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Sheet.Sheet import *

from Britefury.MarkTools.RectangularMarkRegionManipulator import RectangularMarkRegionFieldManipulator

from Britefury.MeshTools.Mark.ToolMeshMarkRegion import ProcMeshMarkRegion, ToolMeshMarkRegion, MeshMarkRegionTarget

from Britefury.Mesh.Mesh import GSProductMesh





class ProcMeshMarkRectangular (ProcMeshMarkRegion):
	description = _( 'Rectangular select' )
	inputClass = GSProductMesh


	def getDescription(self):
		if self.target == MeshMarkRegionTarget.VERTICES:
			return 'Rectangular select vertices'
		elif self.target == MeshMarkRegionTarget.EDGES:
			return 'Rectangular select edges'
		elif self.target == MeshMarkRegionTarget.FACES:
			return 'Rectangular select faces'





class ToolMeshMarkRectangular (ToolMeshMarkRegion):
	sheet = SheetRefField( ProcMeshMarkRectangular )

	functionClass = ProcMeshMarkRectangular
	pageTitle = _( 'Rect.Sel.' )
	titleText = _( 'Rectangular Select' )


	regionManip = RectangularMarkRegionFieldManipulator( sheet.predicate, sheet.region, sheet.backfaceCull, sheet.bEnabled )



