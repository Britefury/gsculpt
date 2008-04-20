##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Point3, Quaternion, Vector2, Matrix4, Colour3f, Segment3

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.SheetEdit import *
from Britefury.SheetEdit.SheetEditorTool import *
from Britefury.SheetEdit.FieldEditors import *
from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model.Model import VisualPlane

from Britefury.Model import ModelDraw

from Britefury.UI.FormLayout import FormLayoutFactory
from Britefury.UI.UIAction import UIAction

from Britefury.WorkArea.Viewport3d import Viewport3dPainter

from Britefury.View.View import PaintLayer
from Britefury.View.DrawingTemplate import DrawingTemplate




class DrawingPlane (Sheet, Viewport3dPainter, DrawingTemplate):
	position = Field( Point3, Point3() )
	rotation = Field( Quaternion, Quaternion() )
	size = Field( float, 100.0 )
	bEnabled = Field( bool, False )


	def _p_visualPlane(self):
		return VisualPlane( self.position, self.rotation, self.size, self.bEnabled )

	def _p_plane(self):
		return self.visualPlane.plane


	visualPlane = FunctionField( _p_visualPlane )
	plane = FunctionField( _p_plane )





	def __init__(self):
		super( DrawingPlane, self ).__init__()
		self.cells.visualPlane.changedSignal.connect( self._p_onVisualPlane )
		self._viewports = []



	def evPaint3d(self, viewport, paintLayer):
		self.visualPlane.paint( ModelDraw.DRAWING_PLANE_COLOUR, ModelDraw.DRAWING_PLANE_ALPHA, paintLayer )


	def painterAttachedToViewport(self, viewport):
		super( DrawingPlane, self ).painterAttachedToViewport( viewport )
		self._viewports.append( viewport )


	def painterDetachedFromViewport(self, viewport):
		super( DrawingPlane, self ).painterDetachedFromViewport( viewport )
		self._viewports.remove( viewport )



	def templateDrawingPoint3d(self, ray, bBackfaceCulling):
		if self.bEnabled:
			bSuccess, t, intersection = ray.intersect( self.plane )
			if bSuccess:
				return True, t, intersection, Segment3( ray.a, intersection )
		return False, None, None, None



	def _p_postRedraw(self):
		for viewport in self._viewports:
			viewport.postRedraw()

	def _p_onVisualPlane(self):
		self._p_postRedraw()





_positionAction = UIAction( 'Tools.Main.DrawingPlaneMode.Position', ( _( 'Tools' ), _( 'Main' ), _( 'Drawing plane mode' ), _( 'Position mode' ) ),
			_( 'Position' ), _( 'Position' ), 'w', _( 'Position mode' ) )
_rotationAction = UIAction( 'Tools.Main.DrawingPlaneMode.Rotation', ( _( 'Tools' ), _( 'Main' ), _( 'Drawing plane mode' ), _( 'Rotation mode' ) ),
			_( 'Rotation' ), _( 'Rotation' ), 'r', _( 'Rotation mode' ) )
_sizeAction = UIAction( 'Tools.Main.DrawingPlaneMode.Size', ( _( 'Tools' ), _( 'Main' ), _( 'Drawing plane mode' ), _( 'Size mode' ) ),
			_( 'Size' ), _( 'Size' ), 'e', _( 'Size mode' ) )



class DrawingPlaneTool (SheetEditorTool):
	sheet = SheetRefField( DrawingPlane )

	pageTitle = _( 'DP' )
	titleText = _( 'Drawing plane' )


	positionManip = Point3FieldManipulator( sheet.position, snapping=FieldManipulatorSnappingDistance() )
	rotationManip = RotationFieldManipulator( sheet.rotation, sheet.position, snapping=FieldManipulatorSnappingAngle() )
	sizeManip = Real1FieldManipulator( sheet.size, 0.0, 1.0e18, RealFieldManipulator.WorldSpaceScaleFactory( sheet.position ), snapping=FieldManipulatorSnappingDistance() )

	manipulatorSwitch = FieldManipulatorSwitch( [ positionManip, rotationManip, sizeManip ] )
	manipulatorSwitch.enable()

	manipulatorSelector = FieldManipulatorSwitchRadioGroup( manipulatorSwitch )
	positionButton = manipulatorSelector.addChoice( positionManip, _positionAction )
	rotationButton = manipulatorSelector.addChoice( rotationManip, _rotationAction )
	sizeButton = manipulatorSelector.addChoice( sizeManip, _sizeAction )

	positionLabel = FieldLabelWithFn( sheet.position, _( 'Position:' ) )
	positionEntry = Point3FieldEditEntry( sheet.position, -1.0e18, 1.0e18, 0.1, 10.0, 4 )
	rotationlabel = FieldLabelWithFn( sheet.rotation, _( 'Rotation:' ) )
	rotationPopup = QuaternionFieldEditDirectionPopup( sheet.rotation, _( 'Set normal' ) )
	rotationEntry = QuaternionFieldEditEntry( sheet.rotation )
	sizeLabel = FieldLabelWithFn( sheet.size, _( 'Size:' ) )
	sizeEntry = FloatFieldEditEntry( sheet.size, 0.0, 1.0e18, 0.1, 10.0, 4 )



	layout = FormLayoutFactory()
	layout.row()  <<  _( 'Mouse function:' )
	layout.row()  <<  positionButton
	layout.row()  <<  rotationButton
	layout.row()  <<  sizeButton
	layout.row()
	layout.row()  <<  positionLabel.label
	layout.row()  <<  'X:'  <<  positionEntry.x
	layout.row()  <<  'Y:'  <<  positionEntry.y
	layout.row()  <<  'Z:'  <<  positionEntry.z
	layout.row()
	layout.row()  <<  rotationlabel.label
	layout.row()  <<  rotationPopup.popup
	layout.row()  <<  'X:'  <<  rotationEntry.x
	layout.row()  <<  'Y:'  <<  rotationEntry.y
	layout.row()  <<  'Z:'  <<  rotationEntry.z
	layout.row()
	layout.row()  <<  sizeLabel.label  <<  sizeEntry.entry
	layout.row()
	layout.finalise()
