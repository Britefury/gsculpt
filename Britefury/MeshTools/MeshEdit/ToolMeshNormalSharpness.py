##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.MeshTools.MeshEditComponent import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import GSProductMesh




# Cell editor for the mesh normal sharpness boolean setting

class MeshNormalSharpnessCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshNormalSharpnessCellEditRadioGroup, self ).__init__()
		self.smooth = self.addChoice( False, _( 'Smooth' ) )
		self.sharp = self.addChoice( True, _( 'Sharp' ) )


# Field editor mesh normal sharpness boolean setting

class MeshNormalSharpnessFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'smooth', 'sharp' ]

	def createElement(self, rowEditContext):
		return MeshNormalSharpnessCellEditRadioGroup()







class ProcMeshNormalSharpness (GSProcedure):
	description = _( 'Sharpness' )

	targetComponent = Field( MeshEditComponent, MeshEditComponent.EDGES )
	sharpness = Field( bool, True )


	def getDescription(self):
		if self.targetComponent == MeshEditComponent.VERTICES:
			if self.sharpness:
				return _( 'Sharpen vertices' )
			else:
				return _( 'Smooth vertices' )
		elif self.targetComponent == MeshEditComponent.EDGES:
			if self.sharpness:
				return _( 'Sharpen edges' )
			else:
				return _( 'Smooth edges' )


	inputClass = GSProductMesh


	def procedureInvoke(self, inputObject, errorHandler):
		if self.targetComponent == MeshEditComponent.VERTICES:
			inputObject.setMarkedVertexNormalSharpness( self.sharpness )
		elif self.targetComponent == MeshEditComponent.EDGES:
			inputObject.setMarkedEdgeNormalSharpness( self.sharpness )
		return inputObject



class ToolMeshNormalSharpness (GSProcedureTool):
	sheet = SheetRefField( ProcMeshNormalSharpness )

	functionClass = ProcMeshNormalSharpness
	pageTitle = _( 'Sharp' )
	titleText = _( 'Sharpness' )

	sharpnessLabel = FieldLabelWithFn( sheet.sharpness, _( 'Sharpness:' ) )
	sharpnessRadio = MeshNormalSharpnessFieldEditRadioGroup( sheet.sharpness )

	layout = FormLayoutFactory()
	layout.row()  <<  sharpnessLabel.label
	layout.row()  <<  sharpnessRadio.smooth
	layout.row()  <<  sharpnessRadio.sharp
	layout.row()
	layout.textBlock( _( 'This tool affects the normals of the mesh. Enable \'smoothing\' in the viewport drawing settings to see the effect.' ) )
	layout.row()  <<  ''
	layout.finalise()
