##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *
from Britefury.Manipulator import RealManipulator

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import MeshSlideEdgesAdjuster, GSProductMesh




class ProcMeshSlideEdges (GSProcedure):
	__version__ = '1'
	description = _( 'Slide edges' )

	slide = Field( float, 0.0 )

	inputClass = GSProductMesh


	def procedureInvoke(self, inputObject, errorHandler):
		adjuster = MeshSlideEdgesAdjuster()
		inputObject.slideMarkedEdges( adjuster, self.getSlideVersion() )
		adjuster.setParameters( self.slide )
		return inputObject


	def getSlideVersion(self):
		try:
			v = self.__version__
		except AttributeError:
			v = None

		if v is None:
			return 0
		elif v == '1':
			return 1
		else:
			return 1



class SlideEdgesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshSlideEdges

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.adjuster = MeshSlideEdgesAdjuster()
		self.mesh.slideMarkedEdges( self.adjuster, self._proc.getSlideVersion() )

	def adjust(self):
		self.adjuster.setParameters( self._proc.slide )

	def getResult(self):
		return self.mesh




class ToolMeshSlideEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshSlideEdges )

	functionClass = ProcMeshSlideEdges
	adjustableInvoker = SlideEdgesAdjustableInvoker
	pageTitle = _( 'Slide' )
	titleText = _( 'Slide Edges' )

	slideLabel = FieldLabelWithFn( sheet.slide,  _( 'Slide:' ) )
	slideEntry = FloatFieldEditEntry( sheet.slide, -1.0, 1.0, 0.001, 0.1, 4 )

	slideManip = Real1FieldManipulator( sheet.slide, -1.0, 1.0, RealManipulator.RealManipulator.FixedScale( 0.0025 ), snapping=FieldManipulatorSnappingFraction() )
	slideManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  slideLabel.label  <<  slideEntry.entry
	layout.row()
	layout.finalise()
