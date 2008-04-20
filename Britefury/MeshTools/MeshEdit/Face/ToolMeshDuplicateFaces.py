##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Vector3

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import MeshVectorExtrudeFacesAdjuster, GSProductMesh




class ProcMeshDuplicateFaces (GSProcedure):
	description = _( 'Duplicate faces' )

	offset = Field( Vector3, Vector3() )
	bMarkOriginalFaces = Field( bool, False )
	bMarkDuplicateFaces = Field( bool, True )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		adjuster = MeshVectorExtrudeFacesAdjuster()
		inputObject.duplicateMarkedFaces( adjuster, self.bMarkOriginalFaces, self.bMarkDuplicateFaces )
		adjuster.setParameters( self.offset )
		return inputObject



class DuplicateFacesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshDuplicateFaces

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.adjuster = MeshVectorExtrudeFacesAdjuster()
		self.mesh.duplicateMarkedFaces( self.adjuster, self._proc.bMarkOriginalFaces, self._proc.bMarkDuplicateFaces )

	def adjust(self):
		self.adjuster.setParameters( self._proc.offset )

	def getResult(self):
		return self.mesh




class ToolMeshDuplicateFaces (GSProcedureTool):
	sheet = SheetRefField( ProcMeshDuplicateFaces )

	functionClass = ProcMeshDuplicateFaces
	adjustableInvoker = DuplicateFacesAdjustableInvoker
	pageTitle = _( 'DupF' )
	titleText = _( 'Duplicate Faces' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return product.getMarkedFacesCentre()
		else:
			return None

	focalPoint = FunctionField( _p_focalPoint )

	offsetLabel = FieldLabelWithFn( sheet.offset, _( 'Offset:' ) )
	offsetEntry =Vector3FieldEditEntry( sheet.offset, -1.0e8, 1.0e8, 0.1, 10.0, 4 )
	bMarkOriginalFacesCheck = BoolFieldEditCheckWithFn( sheet.bMarkOriginalFaces, _( 'Mark original faces' ) )
	bMarkDuplicateFacesCheck = BoolFieldEditCheckWithFn( sheet.bMarkDuplicateFaces, _( 'Mark duplicate faces' ) )

	offsetManip = Translation3FieldManipulator3d( sheet.offset, focalPoint, snapping=FieldManipulatorSnappingDistance() )
	offsetManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  offsetLabel.label
	layout.row()  <<  'X:'  <<  offsetEntry.x
	layout.row()  <<  'Y:'  <<  offsetEntry.y
	layout.row()  <<  'Z:'  <<  offsetEntry.z
	layout.row()
	layout.row()  <<  bMarkOriginalFacesCheck.check
	layout.row()  <<  bMarkDuplicateFacesCheck.check
	layout.row()
	layout.finalise()
