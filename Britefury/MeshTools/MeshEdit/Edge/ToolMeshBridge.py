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
from Britefury.SheetEdit.SheetEdit import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.Manipulator.FieldManipulators import *

from Britefury.ProceduralCore.GSProcedure import *
from Britefury.ProceduralTool.GSProcedureTool import *

from Britefury.PolyBlend.PolyBlendComponent import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import GSProductMesh



class ProcMeshBridge (GSProcedure):
	description = _( 'Bridge' )

	blend = CompositeField( PolyBlendComponent )
	segments = Field( int, 1 )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.bridge( self.blend.makePolyBlend(), self.segments )
		return inputObject



class ToolMeshBridge (GSProcedureTool):
	sheet = SheetRefField( ProcMeshBridge )

	functionClass = ProcMeshBridge
	pageTitle = _( 'Bridge' )
	titleText = _( 'Bridge' )

	blendEdit = PolyBlendComponentFieldEdit( sheet.blend )
	numSegmentsLabel = FieldLabelWithFn( sheet.segments, _( 'Segments:' ) )
	numSegmentsEntry = IntFieldEditEntry( sheet.segments, 1, 1048576, 1, 10 )

	numSegmentsManip = IntFieldScrollManipulator( sheet.segments, 1, 1048576 )
	numSegmentsManip.enable()

	layout = FormLayoutFactory()
	layout.textBlock( _( 'Use the scroll wheel to change the number of segments.' ) )
	layout.row()
	layout.block( 4 )  <<  blendEdit.sheetEditor
	layout.row()
	layout.row()  <<  numSegmentsLabel.label  <<  numSegmentsEntry.entry
	layout.row()
	layout.finalise()
