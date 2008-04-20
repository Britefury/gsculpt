##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegisterPrimitive

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *
from Britefury.Manipulator import RealManipulator

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import MeshBevelFacesAdjuster, GSProductMesh

from Britefury.MeshTools.MeshEdit.MeshInsetFacesStyle import *

from Britefury.Event.QueuedEvent import *





class ProcMeshBevelFaces (GSProcedure):
	description = _( 'Bevel faces' )

	inset = Field( float, 0.0 )
	extrude = Field( float, 0.0 )
	style = Field( MeshInsetFacesStyle, MeshInsetFacesStyle.FROMEDGES )
	bSingular = Field( bool, False )
	bFlowThruMarkedEdges = Field( bool, False )
	bMarkBoundaryEdges = Field( bool, False )
	bMarkInsetEdges = Field( bool, False )
	bNSharpBoundaryEdges = Field( bool, True )
	bNSharpInsetEdges = Field( bool, False )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		adjuster = MeshBevelFacesAdjuster()
		if self.bSingular:
			inputObject.bevelMarkedFacesSingular( adjuster, self.style, self.bMarkBoundaryEdges, self.bMarkInsetEdges, self.bNSharpBoundaryEdges, self.bNSharpInsetEdges )
		else:
			inputObject.bevelMarkedFaces( adjuster, self.style, self.bFlowThruMarkedEdges, self.bMarkBoundaryEdges, self.bMarkInsetEdges, self.bNSharpBoundaryEdges, self.bNSharpInsetEdges )
		adjuster.setParameters( self.inset, self.extrude )
		return inputObject



class BevelFacesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshBevelFaces

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.adjuster = MeshBevelFacesAdjuster()
		if self._proc.bSingular:
			self.mesh.bevelMarkedFacesSingular( self.adjuster, self._proc.style, self._proc.bMarkBoundaryEdges, self._proc.bMarkInsetEdges, self._proc.bNSharpBoundaryEdges, self._proc.bNSharpInsetEdges )
		else:
			self.mesh.bevelMarkedFaces( self.adjuster, self._proc.style, self._proc.bFlowThruMarkedEdges, self._proc.bMarkBoundaryEdges, self._proc.bMarkInsetEdges, self._proc.bNSharpBoundaryEdges, self._proc.bNSharpInsetEdges )

	def adjust(self):
		self.adjuster.setParameters( self._proc.inset, self._proc.extrude )

	def getResult(self):
		return self.mesh




class ToolMeshBevelFaces (GSProcedureTool):
	sheet = SheetRefField( ProcMeshBevelFaces )

	functionClass = ProcMeshBevelFaces
	adjustableInvoker = BevelFacesAdjustableInvoker
	pageTitle = _( 'Bevel' )
	titleText = _( 'Bevel Faces' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return product.getMarkedFacesCentre()
		else:
			return None

	def _p_worldSpaceScale(self):
		return RealManipulator.RealManipulator.WorldSpaceScale( self.cells.focalPoint )

	def _p_fixedScale(self):
		return RealManipulator.RealManipulator.FixedScale( 0.0025 )

	def _p_insetScale(self):
		style = self._style
		if style == MeshInsetFacesStyle.FROMEDGES:
			return self._worldSpaceScale
		elif style == MeshInsetFacesStyle.TOCENTRE:
			return self._fixedScale
		else:
			return self._fixedScale

	def _p_insetSnapping(self):
		style = self._style
		if style == MeshInsetFacesStyle.FROMEDGES:
			return self._editorSettings.snapSettings.distance
		elif style == MeshInsetFacesStyle.TOCENTRE:
			return self._editorSettings.snapSettings.fraction


	_style = FieldProxy( sheet.style )
	focalPoint = FunctionField( _p_focalPoint )
	_worldSpaceScale = FunctionField( _p_worldSpaceScale )
	_fixedScale = FunctionField( _p_fixedScale )
	_insetScale = FunctionField( _p_insetScale )
	_insetSnapping = FunctionField( _p_insetSnapping )

	insetLabel = FieldLabelWithFn( sheet.inset, _( 'Inset:' ) )
	insetEntry = FloatFieldEditEntry( sheet.inset, -1.0e8, 1.0e8, 0.1, 10.0, 4 )
	extrudeLabel = FieldLabelWithFn( sheet.extrude, _( 'Extrude:' ) )
	extrudeEntry = FloatFieldEditEntry( sheet.extrude, -1.0e8, 1.0e8, 0.1, 10.0, 4 )
	styleLabel = FieldLabelWithFn( sheet.style, _( 'Style:' ) )
	styleRadio = MeshInsetFacesStyleFieldEditRadioGroup( sheet.style )
	bSingularCheck = BoolFieldEditCheckWithFn( sheet.bSingular, _( 'Singular' ) )
	bFlowThruMarkedEdgesCheck = BoolFieldEditCheckWithFn( sheet.bFlowThruMarkedEdges, _( 'Flow thru marked edges' ) )
	bMarkBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkBoundaryEdges, _( 'Mark boundary edges' ) )
	bMarkInsetEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkInsetEdges, _( 'Mark inset edges' ) )
	bNSharpBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bNSharpBoundaryEdges, _( 'Normal sharpen boundary edges' ) )
	bNSharpInsetEdgesCheck = BoolFieldEditCheckWithFn( sheet.bNSharpInsetEdges, _( 'Normal sharpen inset edges' ) )

	bevelManip = Real2FieldManipulator( sheet.inset, -1.0e8, 1.0e8, _insetScale,
								sheet.extrude, -1.0e8, 1.0e8, RealFieldManipulator.WorldSpaceScaleFactory( focalPoint ),
								_insetSnapping, ySnapping=FieldManipulatorSnappingDistance() )
	bevelManip.enable()


	def _p_notSingular(self):
		if self.sheet is not None:
			return not self.sheet.bSingular
		else:
			return True

	_notSingular = FunctionField( _p_notSingular )
	bFlowThruMarkedEdgesCheck.sensitivityField = _notSingular


	layout = FormLayoutFactory()
	layout.row()  <<  bSingularCheck.check
	layout.row()
	layout.row()  <<  styleLabel.label  <<  styleRadio.fromEdges
	layout.row()  <<  None  <<  styleRadio.toCentre
	layout.row()
	layout.row()  <<  insetLabel.label  <<  insetEntry.entry
	layout.row()  <<  extrudeLabel.label  <<  extrudeEntry.entry
	layout.row()
	layout.row()  <<  bFlowThruMarkedEdgesCheck.check
	layout.row()
	layout.row()  <<  bMarkBoundaryEdgesCheck.check
	layout.row()  <<  bMarkInsetEdgesCheck.check
	layout.row()  <<  bNSharpBoundaryEdgesCheck.check
	layout.row()  <<  bNSharpInsetEdgesCheck.check
	layout.row()
	layout.finalise()
